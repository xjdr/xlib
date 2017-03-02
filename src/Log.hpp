#pragma once

#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/syscall.h>

#include <string>
#include <vector>
#include <iomanip>
#include <ostream>
#include <iostream>
#include <functional>

#define COMPACT_LOG_INFO LogMessage(__FILE__, __LINE__)
#define COMPACT_LOG_WARN LogMessage(__FILE__, __LINE__, LogSeverity::WARN)
#define COMPACT_LOG_ERROR LogMessage(__FILE__, __LINE__, LogSeverity::ERROR)
#define COMPACT_LOG_FATAL LogMessage(__FILE__, __LINE__, LogSeverity::FATAL)

#define LOG(severity) COMPACT_LOG_ ## severity.stream()

enum class LogSeverity { INFO = 0, WARN = 1, ERROR = 2, FATAL = 3 };
static const size_t kMaxLogMessageLen = 30000;

typedef double WallTime;

class LogStreamBuf : public std::streambuf {
 public:
  // REQUIREMENTS: "len" must be >= 2 to account for the '\n' and '\n'.
  LogStreamBuf(char *buf, int len) {
    setp(buf, buf + len - 2);
  }
  // This effectively ignores overflow.
  virtual int_type overflow(int_type ch) {
    return ch;
  }

  // Legacy public ostrstream method.
  size_t pcount() const { return pptr() - pbase(); }
  char* pbase() const { return std::streambuf::pbase(); }
};

class LogStream : public std::ostream {
 public:
  LogStream(char *buf, int len, int ctr)
    : std::ostream(NULL),
    streambuf_(buf, len),
    ctr_(ctr),
    self_(this) {
    rdbuf(&streambuf_);
  }

  int ctr() const { return ctr_; }
  void set_ctr(int ctr) { ctr_ = ctr; }
  LogStream* self() const { return self_; }

  // Legacy std::streambuf methods.
  size_t pcount() const { return streambuf_.pcount(); }
  char* pbase() const { return streambuf_.pbase(); }
  char* str() const { return pbase(); }
 private:

  LogStream(const LogStream&);
  LogStream& operator=(const LogStream&);
  LogStreamBuf streambuf_;
  int ctr_;          // Counter hack (for the LOG_EVERY_X() macro)
  LogStream *self_;  // Consistency check hack
};

enum class LogSink {};

struct LogMessageData  {

  LogMessageData()
     : stream_(message_text_, kMaxLogMessageLen, 0) {
  }

  int preserved_errno_;      // preserved errno

  // Buffer space; contains complete message text.
  char message_text_[kMaxLogMessageLen+1];
  LogStream stream_;
  LogSeverity severity_;      // What level is this LogMessage logged at?
  int line_;                  // line number where logging call is.
  std::function<void()> send_method_;  // Call this in destructor to send
  union {                              // At most one of these is used: union to keep the size low.
    LogSink* sink_;                    // NULL or sink to send message to
    std::vector<std::string>* outvec_; // NULL or vector to push message onto
    std::string* message_;             // NULL or string to write message into
  };
  time_t timestamp_;            // Time of creation of LogMessage
  struct ::tm tm_time_;         // Time of creation of LogMessage
  size_t num_prefix_chars_;     // # of chars of prefix in this message
  size_t num_chars_to_log_;     // # of chars of msg to send to log
  size_t num_chars_to_syslog_;  // # of chars of msg to send to syslog
  std::string basename_;        // basename of file that called LOG
  std::string fullname_;        // fullname of file that called LOG
  bool has_been_flushed_;       // false => data has not been flushed
  bool first_fatal_;            // true => this was first fatal msg

 private:
  LogMessageData(const LogMessageData&);
  void operator=(const LogMessageData&);
};

class LogMessage {
 public:
  LogMessage(std::string file, int line) {
    data_->first_fatal_ = false;

    populateLogMessageData(LogSeverity::INFO, file, line, nullptr);
  }

  LogMessage(std::string file, int line, LogSeverity severity) {
    data_->first_fatal_ = false;

    populateLogMessageData(severity, file, line, nullptr);
  }

  LogMessage(std::string file, int line, LogSeverity severity,
	     int ctr, std::function<void()> send_method) {
    if (severity != LogSeverity::FATAL) {
      data_->first_fatal_ = false;
    } else {

    }

    populateLogMessageData(LogSeverity::INFO, file, line, send_method);
  }

  std::ostream& stream() {
    return data_->stream_;
  }

  ~LogMessage() {
    Flush();
  }

 private:
  std::unique_ptr<LogMessageData> data_ = std::make_unique<LogMessageData>();
  std::function<void()> send_method;

  double CycleClock_Now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000000 + tv.tv_usec;
  }

  WallTime WallTime_Now() {
    return CycleClock_Now() * 0.000001;
  }

  double get_cpu_time(){
    return (double)clock() / CLOCKS_PER_SEC;
  }

  const char* const_basename(const char* filepath) {
    const char* base = strrchr(filepath, '/');

    return base ? (base+1) : filepath;
  }


  pid_t GetPID() {

    //static bool lacks_gettid = false;
    //if (!lacks_gettid) {
    //      pid_t tid = syscall(SYS_gettid);
      //if (tid != -1) {
    //	return tid;
	//}
	//}

    return getpid();
  }

  const char*const getSeverityChar(int sev) {

  const char*const LogSeverityNames[] = {
    "INFO", "WARNING", "ERROR", "FATAL"
  };

  return LogSeverityNames[sev];
  }

   void populateLogMessageData(LogSeverity severity, std::string file,
			      int line, std::function<void()> send_method) {

    stream().fill('0');
    data_->preserved_errno_ = errno;
    data_->severity_ = severity;
    data_->line_ = line;
    data_->send_method_ = send_method;
    data_->sink_ = nullptr;
    data_->outvec_ = nullptr;
    WallTime now = WallTime_Now();
    data_->timestamp_ = static_cast<time_t>(now);
    localtime_r(&data_->timestamp_, &data_->tm_time_);
    int usecs = static_cast<int>((now - data_->timestamp_) * 1000000);

    data_->num_chars_to_log_ = 0;
    data_->num_chars_to_syslog_ = 0;
    data_->basename_ = const_basename(file.c_str());
    data_->fullname_ = file;
    data_->has_been_flushed_ = false;

    stream() << getSeverityChar(static_cast<int>(severity))
	<< std::setw(2) << 1+data_->tm_time_.tm_mon
	<< std::setw(2) << data_->tm_time_.tm_mday
	<< ' '
	<< std::setw(2) << data_->tm_time_.tm_hour  << ':'
	<< std::setw(2) << data_->tm_time_.tm_min   << ':'
	<< std::setw(2) << data_->tm_time_.tm_sec   << "."
	<< std::setw(6) << usecs
	<< ' '
	<< std::setfill(' ') << std::setw(5)
	<< static_cast<unsigned int>(GetPID()) << std::setfill('0')
	       << ' '
	       << data_->basename_ << ':' << data_->line_ << "] ";

    data_->num_prefix_chars_ = data_->stream_.pcount();

   }

  void Flush() {
	  // TODO(JR): Need to determin better way to abstract the
	  std::cout << data_->message_text_ << '\n';
  }

};

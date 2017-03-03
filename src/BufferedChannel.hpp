/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#pragma once

#include <stdlib.h> // size_t

#include <atomic>   // atomic<int>
#include <memory>   // shared_ptr

#include "Buf.hpp"
#include "Preprocessor.hpp"

#define CHANNEL_UNDERFLOW -1
#define CHANNEL_OVERFLOW -2

/*
 * Buffered Channel is only for char[] as it is used for
 * specifically for network communication and concurrent
 * scatter / gather work (databases as an example). If you
 * need something more generic, use a normal channel and
 * compose as required.
 */

class BufferedChannel {
 public:
  BufferedChannel(int size) {
    buf = Buf::create(size);
    sizes = new size_t[size * sizeof(size_t)];
  }

  static std::shared_ptr<BufferedChannel> mk(int size) {
    return std::make_shared<BufferedChannel>(size);
  }

  int put(char *buf_, size_t size) {
    sizes[w_index.load()] = size;
    int resp = resp = buf->put(buf_, size);
    if (LIKELY(resp > 0)) w_index++;

    return resp;
  }

  int get(char *buf_) {
    int resp = buf->get(buf_, sizes[r_index.load()]);
    if (LIKELY(resp > 0)) r_index++;

    return resp;
  }

  void clear() {
    buf->clear();
    w_index.store(0);
    r_index.store(0);
  }

  ~BufferedChannel() {
    delete[] sizes;
  }

 private:
  size_t *sizes;
  std::unique_ptr<Buf> buf;
  std::atomic<int> w_index = {0};
  std::atomic<int> r_index = {0};

};


int operator<<(std::shared_ptr<BufferedChannel> &c, char *buf) {
  return c->put((char *)buf, strlen(buf));
}

int operator>>(std::shared_ptr<BufferedChannel> &c, char *buf) {
  return c->get(buf);
}

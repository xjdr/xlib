/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose


#pragma once

#include <stdio.h>

#include <exception>
#include <functional>

#include "Preprocessor.hpp"

//
// HERE BE DRAGONS! - This code is borderline unreadable ...
//
// Our coding standards dictate that templates should be avoided
// and code clairity should be paramount. The below code achieves
// neither. The ScopeGuard fuctionality is required and will be
// used as is until such time as we can re-write the macros to be
// more in line with our desired coding standards.
//


/*
 * ScopeGuards run functions when leaving scopes.
 *
 * Taken from: CppCon 2015: Andrei Alexandrescu “Declarative Control Flow"
 * https://www.youtube.com/watch?v=WjTrfoiB0MQ&t=2734s
 *
 * and from
 *
 * Folly's ScopeGuard
 * https://github.com/facebook/folly/blob/master/folly/ScopeGuard.h
 *
 */


class ScopeGuardImplBase {
 public:
  void dismiss() noexcept {
    dismissed_ = true;
  }

  template <typename T>
  INLINE static void runAndWarnAboutToCrashOnException(
      T& function) noexcept {
    try {
      function();
    } catch (...) {
      warnAboutToCrash();
      std::terminate();
    }
  }

 protected:
  ScopeGuardImplBase() noexcept : dismissed_(false) {}

  static ScopeGuardImplBase makeEmptyScopeGuard() noexcept {
    return ScopeGuardImplBase{};
  }

  template <typename T>
  static const T& asConst(const T& t) noexcept {
    return t;
  }

  bool dismissed_;

 private:
  static void warnAboutToCrash() noexcept {
    fprintf(stderr, "This program will now terminate because a ScopeGuard callback threw an \nexception.\n");

  };
};

template <typename FunctionType>
class ScopeGuardImpl : public ScopeGuardImplBase {
 public:
  explicit ScopeGuardImpl(FunctionType& fn) noexcept(
      std::is_nothrow_copy_constructible<FunctionType>::value)
      : ScopeGuardImpl(
	    asConst(fn),
	    makeFailsafe(std::is_nothrow_copy_constructible<FunctionType>{},
			 &fn)) {}

  explicit ScopeGuardImpl(const FunctionType& fn) noexcept(
      std::is_nothrow_copy_constructible<FunctionType>::value)
      : ScopeGuardImpl(
	    fn,
	    makeFailsafe(std::is_nothrow_copy_constructible<FunctionType>{},
			 &fn)) {}

  explicit ScopeGuardImpl(FunctionType&& fn) noexcept(
      std::is_nothrow_move_constructible<FunctionType>::value)
      : ScopeGuardImpl(
	    std::move_if_noexcept(fn),
	    makeFailsafe(std::is_nothrow_move_constructible<FunctionType>{},
			 &fn)) {}

  ScopeGuardImpl(ScopeGuardImpl&& other) noexcept(
      std::is_nothrow_move_constructible<FunctionType>::value)
      : function_(std::move_if_noexcept(other.function_)) {
    dismissed_ = other.dismissed_;
    other.dismissed_ = true;
  }

  ~ScopeGuardImpl() noexcept {
    if (!dismissed_) {
      execute();
    }
  }

 private:
  static ScopeGuardImplBase makeFailsafe(std::true_type, const void*) noexcept {
    return makeEmptyScopeGuard();
  }

  template <typename Fn>
  static auto makeFailsafe(std::false_type, Fn* fn) noexcept
      -> ScopeGuardImpl<decltype(std::ref(*fn))> {
    return ScopeGuardImpl<decltype(std::ref(*fn))>{std::ref(*fn)};
  }

  template <typename Fn>
  explicit ScopeGuardImpl(Fn&& fn, ScopeGuardImplBase&& failsafe)
      : ScopeGuardImplBase{}, function_(std::forward<Fn>(fn)) {
    failsafe.dismiss();
  }

  void* operator new(std::size_t) = delete;

  void execute() noexcept {
    runAndWarnAboutToCrashOnException(function_);
  }

  FunctionType function_;
};

template <typename FunctionType>
ScopeGuardImpl<typename std::decay<FunctionType>::type>
makeGuard(FunctionType&& fn) noexcept(
    std::is_nothrow_constructible<typename std::decay<FunctionType>::type,
				  FunctionType>::value) {
  return ScopeGuardImpl<typename std::decay<FunctionType>::type>(
      std::forward<FunctionType>(fn));
}


class UncaughtExceptionCounter {
 public:
  UncaughtExceptionCounter() noexcept
      : _ec(std::uncaught_exceptions()) {}

  UncaughtExceptionCounter(const UncaughtExceptionCounter& other) noexcept
      : _ec(other._ec) {}

  bool isNewUncaughtException() noexcept {
    return std::uncaught_exceptions() > _ec;
  }

 private:
  int _ec;
};



template <typename FunctionType, bool executeOnException>
class ScopeGuardForNewException {
 public:
  explicit ScopeGuardForNewException(const FunctionType& fn)
      : function_(fn) {
  }

  explicit ScopeGuardForNewException(FunctionType&& fn)
      : function_(std::move(fn)) {
  }

  ScopeGuardForNewException(ScopeGuardForNewException&& other)
      : function_(std::move(other.function_))
      , exceptionCounter_(std::move(other.exceptionCounter_)) {
  }

  ~ScopeGuardForNewException() noexcept(executeOnException) {
    if (executeOnException == exceptionCounter_.isNewUncaughtException()) {
      if (executeOnException) {
	ScopeGuardImplBase::runAndWarnAboutToCrashOnException(function_);
      } else {
	function_();
      }
    }
  }

 private:
  ScopeGuardForNewException(const ScopeGuardForNewException& other) = delete;

  void* operator new(std::size_t) = delete;

  FunctionType function_;
  UncaughtExceptionCounter exceptionCounter_;
};

enum class ScopeGuardOnExit {};

template <typename FunctionType>
ScopeGuardImpl<typename std::decay<FunctionType>::type>
operator+(ScopeGuardOnExit, FunctionType&& fn) {
  return ScopeGuardImpl<typename std::decay<FunctionType>::type>(
      std::forward<FunctionType>(fn));
}

enum class ScopeGuardOnFail {};

template <typename FunctionType>
ScopeGuardForNewException<typename std::decay<FunctionType>::type, true>
operator+(ScopeGuardOnFail, FunctionType&& fn) {
  return
      ScopeGuardForNewException<typename std::decay<FunctionType>::type, true>(
      std::forward<FunctionType>(fn));
}

enum class ScopeGuardOnSuccess {};


template <typename FunctionType>
ScopeGuardForNewException<typename std::decay<FunctionType>::type, false>
operator+(ScopeGuardOnSuccess, FunctionType&& fn) {
  return
      ScopeGuardForNewException<typename std::decay<FunctionType>::type, false>(
      std::forward<FunctionType>(fn));
}


#define SCOPE_EXIT \
  auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) \
  = ScopeGuardOnExit() + [&]() noexcept

#define SCOPE_FAIL \
  auto ANONYMOUS_VARIABLE(SCOPE_FAIL_STATE) \
  = ScopeGuardOnFail() + [&]() noexcept

#define SCOPE_SUCCESS \
  auto ANONYMOUS_VARIABLE(SCOPE_SUCCESS_STATE) \
  = ScopeGuardOnSuccess() + [&]()

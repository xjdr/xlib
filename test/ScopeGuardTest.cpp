/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#include <gtest/gtest.h>

#include "../src/ScopeGuard.hpp"

#include <stdexcept>

enum class ErrorBehavior {
  SUCCESS,
  HANDLED_ERROR,
  UNHANDLED_ERROR,
};

TEST(ScopeGuard, TEST_SCOPE_EXIT) {
  int x = 0;
  {
    SCOPE_EXIT { ++x; };
    EXPECT_EQ(0, x);
  }
  EXPECT_EQ(1, x);
}

class Foo {
public:
  Foo() {}
  ~Foo() {
    try {
      auto e = std::current_exception();
      int test = 0;
      {
	SCOPE_EXIT { ++test; };
	EXPECT_EQ(0, test);
      }
      EXPECT_EQ(1, test);
    } catch (const std::exception& ex) {
      //LOG(FATAL) << "Unexpected exception: " << ex.what();
    }
  }
};

TEST(ScopeGuard, TEST_SCOPE_FAILURE2) {
  try {
    Foo f;
    throw std::runtime_error("test");
  } catch (...) {
  }
}

void testScopeFailAndScopeSuccess(ErrorBehavior error, bool expectFail) {
  bool scopeFailExecuted = false;
  bool scopeSuccessExecuted = false;

  try {
    SCOPE_FAIL { scopeFailExecuted = true; };
    SCOPE_SUCCESS { scopeSuccessExecuted = true; };

    try {
      if (error == ErrorBehavior::HANDLED_ERROR) {
	throw std::runtime_error("throwing an expected error");
      } else if (error == ErrorBehavior::UNHANDLED_ERROR) {
	throw "never throw raw strings";
      }
    } catch (const std::runtime_error&) {
    }
  } catch (...) {
    // Outer catch to swallow the error for the UNHANDLED_ERROR behavior
  }

  EXPECT_EQ(expectFail, scopeFailExecuted);
  EXPECT_EQ(!expectFail, scopeSuccessExecuted);
}

TEST(ScopeGuard, TEST_SCOPE_FAIL_AND_SCOPE_SUCCESS) {
  testScopeFailAndScopeSuccess(ErrorBehavior::SUCCESS, false);
  testScopeFailAndScopeSuccess(ErrorBehavior::HANDLED_ERROR, false);
  testScopeFailAndScopeSuccess(ErrorBehavior::UNHANDLED_ERROR, true);
}

TEST(ScopeGuard, TEST_SCOPE_SUCCESS_THROW) {
  auto lambda = []() {
    SCOPE_SUCCESS { throw std::runtime_error("ehm"); };
  };
  EXPECT_THROW(lambda(), std::runtime_error);
}

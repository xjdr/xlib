/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose


#include <gtest/gtest.h>

#include "../src/Log.hpp"

TEST(Log, TestBasicMacro) {

  LOG(INFO) << "Testing Basic";

}

TEST(Log, TestINFO) {

  LOG(INFO) << "Testing INFO";

}

TEST(Log, TestWARN) {

  LOG(WARN) << "Testing WARN";

}

TEST(Log, TestERROR) {

  LOG(ERROR) << "Testing ERROR";

}

TEST(Log, TestFATAL) {

  LOG(FATAL) << "Testing FATAL";

}

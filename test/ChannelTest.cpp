/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose


#include <gtest/gtest.h>

#include "../src/Channel.hpp"

#include <stdio.h>

TEST(Log, ChannelTest) {
  auto c = Channel::create(30);

  char * out1 = new char[5];
  char * out2 = new char[5];
  char * out3 = new char[5];
  char * out4 = new char[5];
  char * out5 = new char[5];
  char * out6 = new char[5];

  char *in1 = (char *)"Test1";
  char *in2 = (char *)"Test2";
  char *in3 = (char *)"Test3";
  char *in4 = (char *)"Test4";
  char *in5 = (char *)"Test5";
  char *in6 = (char *)"Test6";

  c << in1;
  c << in2;
  c << in3;
  c << in4;

  c >> out1;
  c >> out2;

  c << in5;
  c << in6;

  c >> out3;
  c >> out4;
  c >> out5;
  c >> out6;

 ASSERT_STREQ(in1, out1);
 ASSERT_STREQ(in2, out2);
 ASSERT_STREQ(in3, out3);
 ASSERT_STREQ(in4, out4);
 ASSERT_STREQ(in5, out5);
 ASSERT_STREQ(in6, out6);
}
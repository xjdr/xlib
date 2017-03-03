/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose


#include <gtest/gtest.h>

#include "../src/BufferedChannel.hpp"

#include <future>

TEST(BufferedChannel, BufferedEmptyChannelTest) {
  auto c = BufferedChannel::mk(30);

  char * out = new char[5];

  int i = c >> out;

  EXPECT_EQ(CHANNEL_UNDERFLOW, i);
}

TEST(BufferedChannel, BufferedFullChannelTest) {
  auto c = BufferedChannel::mk(4);

  char *in = (char *)"Test1";

  int i = c << in;

  EXPECT_EQ(CHANNEL_OVERFLOW, i);
}

TEST(BufferedChannel, BufferedSimpleChannelTest) {
  auto c = BufferedChannel::mk(30);

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

TEST(BufferedChannel, BufferedChannelSimpleAsyncTest) {
  auto c = BufferedChannel::mk(30);

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

  std::async(std::launch::async, [&]{ c << in1; });

  std::async(std::launch::async, [&]{ c << in2; });
  std::async(std::launch::async, [&]{ c << in3; });
  std::async(std::launch::async, [&]{ c << in4; });

  std::async(std::launch::async, [&]{ c >> out1; });
  std::async(std::launch::async, [&]{ c >> out2; });

  std::async(std::launch::async, [&]{ c << in5; });
  std::async(std::launch::async, [&]{ c << in6; });

  std::async(std::launch::async, [&]{ c >> out3; });
  std::async(std::launch::async, [&]{ c >> out4; });
  std::async(std::launch::async, [&]{ c >> out5; });
  std::async(std::launch::async, [&]{ c >> out6; });

 ASSERT_STREQ(in1, out1);
 ASSERT_STREQ(in2, out2);
 ASSERT_STREQ(in3, out3);
 ASSERT_STREQ(in4, out4);
 ASSERT_STREQ(in5, out5);
 ASSERT_STREQ(in6, out6);
}

TEST(BufferedChannel, BufferedChannelComplexAsyncTest) {
  auto c = BufferedChannel::mk(30);

  char * out1 = new char[5];
  char * out2 = new char[5];
  char * out3 = new char[5];
  char * out4 = new char[5];
  char * out5 = new char[5];
  char * out6 = new char[5];
  char * out7 = new char[5];

  char *in1 = (char *)"Test1";
  char *in2 = (char *)"Test2";
  char *in3 = (char *)"Test3";
  char *in4 = (char *)"Test4";
  char *in5 = (char *)"Test5";
  char *in6 = (char *)"Test6";

  std::async(std::launch::async, [&]{
      c << in1;
      c << in2;
      c << in3;
      c << in4; });

      c >> out1;
      c >> out2;

      std::async(std::launch::async, [&]{ c << in5; });
      std::async(std::launch::async, [&]{ c << in6; });
      c >> out3;
      c >> out4;
      c >> out5;
      c >> out6;

      int x = c >> out7;

      // std::async(std::launch::async, [&]{ c >> out3; });
      // std::async(std::launch::async, [&]{ c >> out4; });
      // std::async(std::launch::async, [&]{ c >> out5; });
      // std::async(std::launch::async, [&]{ c >> out6; });

 ASSERT_STREQ(in1, out1);
 ASSERT_STREQ(in2, out2);
 ASSERT_STREQ(in3, out3);
 ASSERT_STREQ(in4, out4);
 ASSERT_STREQ(in5, out5);
 ASSERT_STREQ(in6, out6);
 EXPECT_EQ(CHANNEL_UNDERFLOW, x);
}

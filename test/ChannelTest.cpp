/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose


#include <gtest/gtest.h>

#include "../src/Channel.hpp"

#include <future>
#include <string>


TEST(Channel, ChannelBoolTest) {
  auto c = Channel<bool>::mk();
  bool in = true;
  bool out = false;

  c << in;
  c >> out;

  EXPECT_EQ(in, out);

  out = false;

  int i = c >> out;

  EXPECT_EQ(CHANNEL_EMPTY, i);

}

TEST(Channel, ChannelStringTest) {
  auto c = Channel<std::string>::mk();
  std::string in = "Testing";
  std::string out;

  c << in;
  c >> out;

  EXPECT_EQ(in, out);

}

TEST(Channel, ChannelEmptyTest) {
  auto c = Channel<bool>::mk();
  bool in = true;
  bool out = false;

  c << in;
  c >> out;

  EXPECT_EQ(in, out);

  out = false;

  int i = c >> out;

  EXPECT_EQ(CHANNEL_EMPTY, i);
}


TEST(Channel, ChannelFullTest) {
  auto c = Channel<bool>::mk();
  bool in = true;
  bool out = false;

  c << in;
  c >> out;

  EXPECT_EQ(in, out);

  out = false;

  c << in;
  int i = c << in;

  EXPECT_EQ(CHANNEL_FULL, i);
}


TEST(Channel, ChannelChannelTest) {
  //  auto c = Channel<int>::mk();

}

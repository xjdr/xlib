/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#include <gtest/gtest.h>

#include <memory>

#include "../src/Buf.hpp"


TEST(BufTest, TestReadEmpty) {
  int size = 1024;
  std::unique_ptr<Buf> b = Buf::create(size);
  char *out = new char[11];
  int i = b->get(out, 11);
  EXPECT_EQ(BUF_UNDERFLOW, i);
}

TEST(BufTest, TestPutTooMuch) {
  int size = 10;
  std::unique_ptr<Buf> b = Buf::create(size);
  char *in = (char *)"Hello world\0";
  int i = b->put(in, 12);
  EXPECT_EQ(BUF_OVERFLOW, i);
}

TEST(BufTest, TestSingleGet) {
  int size = 12;
  std::unique_ptr<Buf> b = Buf::create(size);
  char *in = (char *)"Hello world\0";
  b->put(in, 12);

  EXPECT_EQ('H', b->get());
  EXPECT_EQ('e', b->get());
  EXPECT_EQ('l', b->get());
  EXPECT_EQ('l', b->get());
  EXPECT_EQ('o', b->get());
  EXPECT_EQ(' ', b->get());
  EXPECT_EQ('w', b->get());
  EXPECT_EQ('o', b->get());
  EXPECT_EQ('r', b->get());
  EXPECT_EQ('l', b->get());
  EXPECT_EQ('d', b->get());
}

TEST(BufTest, TestSimplePutAndGet) {
  int size = 1024;
  std::unique_ptr<Buf> b = Buf::create(size);
  char *in = (char *)"Hello world\0";
  char *out = new char[12];
  b->put(in, 12);
  b->get(out, 12);
  ASSERT_STREQ(in, out);
}

TEST(BufTest, TestMultiplePutAndGet) {
  int size = 1024;
  std::unique_ptr<Buf> b = Buf::create(size);
  char *in = (char *)"Hello world\0";
  char *out = new char[12];
  b->put(in, 12);
  b->get(out, 12);
  ASSERT_STREQ(in, out);

  char *_in = (char *)"Again Hello Again\0";
  char *_out = new char[18];
  b->put(_in, 18);
  b->get(_out, 18);
  ASSERT_STREQ(_in, _out);
  EXPECT_EQ(false, b->isHeap());
}

TEST(BufTest, TestMaxStackPutAndGet) {
  int count = (1024 * 1024) / 12 ;
  int size = 1024 * 1024;
  std::unique_ptr<Buf> b = Buf::create(size);

  for (int i = 0 ; i < count ; i++) {
    char *in = (char *)"Hello world\0";
    char *out = new char[12];
    b->put(in, 12);
    b->get(out, 12);
    ASSERT_STREQ(in, out);
  }

  EXPECT_EQ(false, b->isHeap());
}

TEST(BufTest, TestHeapPutAndGet) {
  int count =  (1024 * 1024 + 1) / 12;
  int size = 1024 * 1024 + 1;
  std::unique_ptr<Buf> b = Buf::create(size);

  for (int i = 0 ; i < count ; i++) {
    char *in = (char *)"Hello world\0";
    char *out = new char[12];
    b->put(in, 12);
    b->get(out, 12);
    ASSERT_STREQ(in, out);
  }

  EXPECT_EQ(true, b->isHeap());
}

/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#include <gtest/gtest.h>

#include "../src/Buf.hpp"

#include <memory>
#include <future>

// These members are now private
// this test is no longer valid but
// is saved for legacy
//
// TEST(BufTest, TestConstructor) {
//   int size = 1024;
//   std::unique_ptr<Buf> b = Buf::create(size);
//   EXPECT_EQ(size, b->capacity);
//   EXPECT_EQ(0, b->cursor);
//   EXPECT_EQ(0, b->w_index);
// }

TEST(BufTest, TestReadEmpty) {
  int size = 1024;
  std::unique_ptr<Buf> b = Buf::create(size);
  char *out = new char[11];
  int i = b->get(out, 11);
  EXPECT_EQ(-1, i);
}

TEST(BufTest, TestPutTooMuch) {
  int size = 10;
  std::unique_ptr<Buf> b = Buf::create(size);
  char *in = (char *)"Hello world\0";
  int i = b->put(in, 12);
  EXPECT_EQ(-2, i);
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

// Works about 50% of the time due to race conditions
//Lets leave fixing races on async for another day
// TEST(BufTest, TestAsyncPutAndGet) {
//   int count = 11 * (1024 * 1024);
//   int size = 1024 * 1024;
//   std::unique_ptr<Buf> b = Buf::create(size);

//   for (int i = 0 ; i < count ; i++) {
//     char *in = (char *)"abcdefghij\0";
//     char *out = new char[11];

//     auto f1 = std::async(std::launch::async, [&b, &in](){
//	int r = b->put(in, 11);
//	ASSERT_TRUE(-1 != r);
//       });
//     //f1.wait();

//     auto f2 = std::async(std::launch::async, [&b, &out](){
//	if (int r = b->get(out, 11) == -1) {
//	  r = b->get(out, 11);
//	  ASSERT_TRUE(-1 != r);
//	}
//       });

//     f2.wait();
//     ASSERT_STREQ(in, out);
//   }
// }

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

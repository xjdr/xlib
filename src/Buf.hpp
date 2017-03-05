/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

/*
 * The Buf class is for building buffers primairly for network
 * applications. The buffer allows you to read and write and the
 * same time. This is specifically useful for things like chunked
 * http messages. Calling the Buf constructor directly is depricated
 * and you should build buffers from the static Buf::create(_size_)
 * method.
 *
 *
 * +-------------------+------------------+------------------+
 * | discardable bytes |  readable bytes  |  writable bytes  |
 * |                   |     (CONTENT)    |                  |
 * +-------------------+------------------+------------------+
 * |                   |                  |                  |
 * 0        <=      r_index     <=     w_index     <=     capacity
 *
 */

#pragma once

#include <stdlib.h> // memcpy, size_t

#include <memory>   // uniuqe_ptr

#include "Preprocessor.hpp"

#define MAX_STACK 1024 * 1024

#define BUF_UNDERFLOW -1
#define BUF_OVERFLOW -2

class Buf {
 public:
  static std::unique_ptr<Buf> create(int cap) {
    std::unique_ptr<Buf> b = std::make_unique<Buf>(cap);

    if (cap > MAX_STACK) {
      b->memory = malloc(cap);
      b->mal = true;
    } else {
      b->memory = new char[cap];
    }

    b->data = (char *)b->memory;

    return b;
  }

  Buf(int cap) {
    capacity = cap;
  }

  void clear() {
    r_index = 0;
    r_index = 0;
  }

  char* array() {
    return data;
  }

  char get() {
    if (UNLIKELY(r_index >= w_index)) return BUF_UNDERFLOW;
    if (UNLIKELY((r_index + 1) > capacity)) return BUF_OVERFLOW;

    return data[r_index++];
  }

  char get(size_t index) {
    if (UNLIKELY(index >= w_index)) return BUF_UNDERFLOW;
    if (UNLIKELY((index + 1) > capacity)) return BUF_OVERFLOW;

    return data[index];
  }


  int get(char *buf, size_t len) {
    if (UNLIKELY(r_index >= w_index)) return BUF_UNDERFLOW;
    if (UNLIKELY((r_index + len) > capacity)) return BUF_OVERFLOW;

    memcpy(buf, &data[r_index], len);
    r_index = r_index + (int) len;

    return 0;
  }

  int get(char *buf, size_t from, size_t len) {
    if (UNLIKELY(from >= w_index)) return BUF_UNDERFLOW;
    if (UNLIKELY((from + len) > capacity)) return BUF_OVERFLOW;

    memcpy(buf, &data[from], len);

    // This operation should not incriment the r_index as this
    // is direct access which assumes the caller knows the index
    // of the desired data. This may be a terrible idea, but we
    // will see how it goes and make changes as necessary.
    //
    //    r_index = r_index + (int) len;

    return 0;
  }

  int put(char *buf, size_t len) {
    if (UNLIKELY(r_index > w_index)) return BUF_UNDERFLOW;
    if (UNLIKELY(w_index + len > capacity)) return BUF_OVERFLOW;

    memcpy(&data[w_index], buf, len);
    w_index = w_index + (int) len;

    return 0;
  }

  bool isHeap() {
    return mal;
  }

  int get_capacity() {
    return capacity;
  }

  int remaining_capacity() {
    return capacity - w_index;
  }

  ~Buf() {
    if (mal) free(memory);
    if (!mal) delete[] (char *)memory;
  }


 private:
  void *memory = nullptr;
  char *data;

  bool mal = false;
  int capacity;
  int r_index = 0;
  int w_index = 0;

};

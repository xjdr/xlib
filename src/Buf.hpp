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

#include <stdlib.h> //size_t

#include <memory>

#define MAX_STACK 1024 * 1024

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

  int get(char *buf, size_t len) {
    if (r_index >= w_index) return -1;
    if ((r_index + len) > capacity) return -2;

    memcpy(buf, &data[r_index], len);
    r_index = r_index + (int) len;

    return 0;
  }

  int put(char *buf, size_t len) {
    if (r_index > w_index) return -1;
    if (w_index + len > capacity) return -2;

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
    if (!mal) delete [] (char *)memory;
  }


 private:
  void *memory = nullptr;
  char *data;

  bool mal = false;
  int capacity;
  int r_index = 0;
  int w_index = 0;

};

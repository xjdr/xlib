/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#pragma once

#include <stdlib.h> // size_t

#include <atomic>

#include "Buf.hpp"

#define CHANNEL_UNDERFLOW -1
#define CHANNEL_OVERFLOW -2

class Channel {
 public:
  Channel(int size) {
    buf = Buf::create(size);
    sizes = new int[size * sizeof(int)];
  }

  static std::shared_ptr<Channel> create(int size) {
    return std::make_shared<Channel>(size);
  }

  void put(char *buf_, size_t size) {
    size_t s = size;
    sizes[w_index.load()] = s;
    buf->put(buf_, s);

    w_index++;
  }

  int get(char *buf_) {
    int resp = buf->get(buf_, sizes[r_index.load()]);

    r_index++;

    return resp;
  }

  void clear() {
    buf->clear();
    w_index.store(0);
    r_index.store(0);
  }

  ~Channel() {
    delete[] sizes;
  }

 private:
  int *sizes;
  std::unique_ptr<Buf> buf;
  std::atomic<int> w_index = {0};
  std::atomic<int> r_index = {0};

};


template <typename T>
void operator<<(std::shared_ptr<Channel> &c, T buf) {
  c->put((char *)buf, strlen((char *)buf));
}


template <typename T>
int operator>>(std::shared_ptr<Channel> &c, T buf) {
  return c->get((char *)buf);
}


template <typename T>
void operator<<(Channel &c, T buf) {
  c.put((char *)buf, strlen((char *)buf));
}


template <typename T>
int operator>>(Channel &c, T buf) {
  return c.get((char *)buf);
}

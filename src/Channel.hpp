/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#pragma once

#include <stdlib.h> // size_t

#include "Buf.hpp"

#include <atomic>

class Channel {
 public:
  Channel(int size) {
    buf = Buf::create(size);
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

  void get(char *buf_) {
    buf->get(buf_, sizes[r_index.load()]);

    r_index++;
  }

 private:
  std::unique_ptr<Buf> buf;
  int sizes[36];
  std::atomic<int> w_index = {0};
  std::atomic<int> r_index = {0};

};


template <typename T>
void operator<<(std::shared_ptr<Channel> &c, T buf) {
  c->put((char *)buf, strlen((char *)buf));
}


template <typename T>
void operator>>(std::shared_ptr<Channel> &c, T buf) {
  c->get((char *)buf);
}


template <typename T>
void operator<<(Channel &c, T buf) {
  c.put((char *)buf, strlen((char *)buf));
}


template <typename T>
void operator>>(Channel &c, T buf) {
  c.get((char *)buf);
}

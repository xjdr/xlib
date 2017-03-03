/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#pragma once

#include <memory> //shared_ptr

#include "Preprocessor.hpp"

#define CHANNEL_EMPTY -1
#define CHANNEL_FULL -2

template <typename T>
class Channel {
 public:
  Channel() {
  }

  static std::shared_ptr<Channel<T>> mk() {
    return std::make_shared<Channel<T>>();
  }

  int put(T msg) {
    if (UNLIKELY(b == e)) return CHANNEL_FULL;

    data = std::move(msg);

    b = sizeof(data);
    assert(b == e);

    return 0;
  }

  int get(T &msg) {
    if (b != e) return CHANNEL_EMPTY;

    msg = std::move(data);
    b = 0;

    return 0;
  }

  ~Channel() {
  }

 private:
  T data;
  size_t b = 0;
  size_t e = sizeof(T);
};

template <typename T>
int operator<<(std::shared_ptr<Channel<T>> &c, T msg) {
  return c->put(msg);
}

template <typename T>
int operator>>(std::shared_ptr<Channel<T>> &c, T &msg) {
  return c->get(msg);
}

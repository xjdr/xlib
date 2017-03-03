/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

/*
 * This Library requires libevent version 2.1 or greater. Great effort
 * has been taken to eliminated external dependencies but this and TLS
 * libraries are 2 of the only occasions where it is more practical to
 * include tested libraries.
 *
 *
 */

#pragma once

#include <event2/event.h>


class EventLoop {
 public:

  EventLoop() {
    evb = event_base_new();
  }

  ~EventLoop() {
    event_base_free(evb);
  }

  void start() {
    event_base_dispatch(evb);
  }

 private:
  struct event_base *evb;

};

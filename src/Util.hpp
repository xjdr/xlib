/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#pragma once

/*
 * Tools to defeat the compiler optimizer in specific places
 * (for when it gets confused and makes bad decisions)
 *
 * Taken from CppCon 2015: Chandler Carruth "Tuning C++: Benchmarks, and CPUs, and Compilers! Oh My!"
 * https://www.youtube.com/watch?v=nXaxk27zwlk
 *
 */

static void escape(void *p) {
  asm volatile("" : : "g"(p) :"memory");
}

static void clobber() {
  asm volatile("" : : :"memory");
}

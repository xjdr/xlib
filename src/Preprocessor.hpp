/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#pragma once

// Inline or no
# define INLINE inline __attribute__((__always_inline__))
# define NOINLINE __attribute__((__noinline__))

/*
 * Likely macro tells the compiler that something is likely or not which
 * allows for the compiler to make good decisions
 *
 * Taken from CppCon 2015: Chandler Carruth "Tuning C++: Benchmarks, and CPUs, and Compilers! Oh My!"
 * https://www.youtube.com/watch?v=nXaxk27zwlk
 *
 */

#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))

/*
 * ANONYMOUS_VARIABLE introduces a unique identifier by incrementing
 * a compiler provided counter every time it is called.
 *
 * Taken from: CppCon 2015: Andrei Alexandrescu “Declarative Control Flow"
 * https://www.youtube.com/watch?v=WjTrfoiB0MQ&t=2734s
 *
 */

#ifndef ANONYMOUS_VARIABLE
#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)
#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)
#endif
#endif

CC = clang++
CFLAGS = -O3 -std=c++14 -stdlib=libc++ \
	-lc++abi                       \
	-fno-rtti		       \
	-Wall -pedantic -Werror        \
	-isystem $(HOME)/include       \
	-Wl, -rpath @$(HOME)/lib64

# CHANGE THIS FOR YOUR GTEST LOCATION IF REQ
GTEST = $(HOME)/lib64/libgtest.a
LIBEVENT = $(HOME)/lib64/libevent.a

all: emacs

test:
	$(CC) $(CFLAGS) TestMain.cpp $(GTEST) $(LIBEVENT) -o test ; ./test --gtest_color=yes ; rm test

emacs:
	$(CC) $(CFLAGS) TestMain.cpp $(GTEST) $(LIBEVENT) -o test ; ./test ; rm test

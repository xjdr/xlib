CC = clang++
CFLAGS = -O3 -std=c++14 -stdlib=libc++ \
	-lc++abi                       \
	-fno-rtti		       \
	-Wall -pedantic -Werror        \
	-isystem $(HOME)/include       \
	-Wl, -rpath @$(HOME)/lib64

all: test

test:
	$(CC) $(CFLAGS) TestMain.cpp $(HOME)/lib64/libgtest.a -o test ; ./test --gtest_color=yes ; rm test

#include <gtest/gtest.h>

#include "EventLoopTest.cpp"
#include "ChannelTest.cpp"
#include "BufferedChannelTest.cpp"
#include "HttpTest.cpp"
#include "LogTest.cpp"
#include "BufTest.cpp"
#include "ScopeGuardTest.cpp"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}

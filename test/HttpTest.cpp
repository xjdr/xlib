/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#include <gtest/gtest.h>

#include "../src/Http.hpp"

#include <string>


TEST(HttpTest, TestCurlGET) {

  char * req =(char *) "GET /test HTTP/1.1\r\n"
    "User-Agent: curl/7.18.0 (i486-pc-linux-gnu) libcurl/7.18.0 OpenSSL/0.9.8g zlib/1.2.3.3 libidn/1.1\r\n"
    "Host: 0.0.0.0=5000\r\n"
    "Accept: */*\r\n"
    "\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());

  msg.uri[5] = '\0';
  ASSERT_STREQ("/test", msg.uri);

  ASSERT_STREQ("User-Agent", msg.headers[0].key);
  ASSERT_STREQ("Host", msg.headers[1].key);
  ASSERT_STREQ("0.0.0.0=5000", msg.headers[1].val);
  ASSERT_STREQ("Accept", msg.headers[2].key);

}


TEST(HttpTest, TestFirefoxGet) {
  char * req = (char *)"GET /favicon.ico HTTP/1.1\r\n"
    "Host: 0.0.0.0=5000\r\n"
    "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) Gecko/2008061015 Firefox/3.0\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
    "Accept-Language: en-us,en;q=0.5\r\n"
    "Accept-Encoding: gzip,deflate\r\n"
    "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
    "Keep-Alive: 300\r\n"
    "Connection: keep-alive\r\n"
    "\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());

}

TEST(HttpTest, TestDumbfuck2) {
  char * req = (char *)"GET /dumbfuck HTTP/1.1\r\n"
    "aaaaaaaaaaaaa:++++++++++\r\n"
    "\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
}

TEST(HttpTest, TestFragmentInUri) {
  char * req = (char *)"GET /forums/1/topics/2375?page=1#posts-17408 HTTP/1.1\r\n"
    "\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
}

TEST(HttpTest, TestNoHeadersNoBody) {
  char * req = (char *)"GET /get_no_headers_no_body/world HTTP/1.1\r\n"
    "\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
}

TEST(HttpTest, TestOneHeadersNoBody) {
  char * req = (char *)"GET /get_one_header_no_body HTTP/1.1\r\n"
    "Accept: */*\r\n"
    "\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
}

TEST(HttpTest, TestFunkyContentLength) {
  const char *req ="POST /get_funky_content_length_body_hello HTTP/1.0\r\nConTENT-Length: 5\r\n\r\nHELLO";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::POST, msg.getMethod());
  EXPECT_EQ(Versions::ZERO, msg.getVersion());

  /////////////////////////////////////////////

  EXPECT_EQ(5, msg.content_length);

  msg.body[5] = '\0';
  ASSERT_STREQ("HELLO", msg.body);

  ASSERT_STREQ("ConTENT-Length", msg.headers[0].key);
}

TEST(HttpTest, TestPostIdentityBodyWorld) {
  char * req = (char *)"POST /post_identity_body_world?q=search#hey HTTP/1.1\r\n"
    "Accept: */*\r\n"
    "Transfer-Encoding: identity\r\n"
    "Content-Length: 5\r\n"
    "\r\n"
    "World";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::POST, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
  EXPECT_EQ(0, msg.chunked);
}

TEST(HttpTest, TestPostChunkedBody) {
  char * req = (char *)"POST /post_chunked_all_your_base HTTP/1.1\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "1e\r\nall your base are belong to us\r\n"
    "0\r\n"
    "\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::POST, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
  EXPECT_EQ(1, msg.chunked);

}

TEST(HttpTest, Test2ChunkMultiZero) {
  char * req = (char *)"POST /two_chunks_mult_zero_end HTTP/1.1\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\nhello\r\n"
    "6\r\n world\r\n"
    "000\r\n"
    "\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::POST, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
}

TEST(HttpTest, TestChunkedWithTrailingHeaders) {
  char * req = (char *)"POST /chunked_w_trailing_headers HTTP/1.1\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\nhello\r\n"
    "6\r\n world\r\n"
    "0\r\n"
    "Vary: *\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::POST, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
}

TEST(HttpTest, TestChunkedWithBullshitAfter11) {
  char * req = (char *)"POST /chunked_w_bullshit_after_length HTTP/1.1\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5; ihatew3;whatthefuck=aretheseparametersfor\r\nhello\r\n"
    "6; blahblah; blah\r\n world\r\n"
    "0\r\n"
    "\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::POST, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
}

TEST(HttpTest, TestWithQuotes) {
  char * req = (char *)"GET /with_\"stupid\"_quotes?foo=\"bar\" HTTP/1.1\r\n\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
}

TEST(HttpTest, TestApacheBench) {
  char * req = (char *)"GET /test HTTP/1.0\r\n"
    "Host: 0.0.0.0:5000\r\n"
    "User-Agent: ApacheBench/2.3\r\n"
    "Accept: */*\r\n\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ZERO, msg.getVersion());
}

TEST(HttpTest, TestQueryWithQuestionMark) {
  char * req = (char *)"GET /test.cgi?foo=bar?baz HTTP/1.1\r\n\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
}

TEST(HttpTest, TestNewLineGet) {
  char * req = (char *)"\r\nGET /test HTTP/1.1\r\n\r\n";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());
}

TEST(HttpTest, TestUpgradeRequest) {
  char * req = (char *)"GET /demo HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "Connection: Upgrade\r\n"
    "Sec-WebSocket-Key2: 12998 5 Y3 1  .P00\r\n"
    "Sec-WebSocket-Protocol: sample\r\n"
    "Upgrade: WebSocket\r\n"
    "Sec-WebSocket-Key1: 4 @1  46546xW%0l 1 5\r\n"
    "Origin: http://example.com\r\n"
    "\r\n"
    "Hot diggity dogg";

  http_msg msg;
  parse_msg(req, &msg);

  EXPECT_EQ(Methods::GET, msg.getMethod());
  EXPECT_EQ(Versions::ONE, msg.getVersion());

}

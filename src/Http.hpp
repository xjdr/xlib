/*
 * Copyright 2017 Jeff Rose
 *
 */

// @author: Jeff Rose

#pragma once

#include <stdlib.h> // uint8_t
#include <string.h> // memcpy

# include <stdio.h>

// Cheap high performance ordere map
struct Map {
  char key[36];
  char val[36];
};

// Need to add this to the parser
enum class Versions { ZERO, ONE, TWO };
enum class Methods  { GET, POST, PUT, DELETE };
//enum class Headers  { ContentLength, Connection, KeepAlive, TransferEncoding };

struct http_msg {

  Methods getMethod() {
    if (method[0] == 'G') return Methods::GET;
    if (method[1] == 'O') return Methods::POST;
    if (method[1] == 'U') return Methods::PUT;
    if (method[0] == 'D') return Methods::DELETE;

    return Methods::GET; // SHOULD NEVER GET HERE
  }

  Versions getVersion() {
    if (version[5] == '2') return Versions::TWO;
    if (version[7] == '1') return Versions::ONE;
    if (version[7] == '0') return Versions::ZERO;

    return Versions::ONE; // SHOULD NEVER GET HERE
  }

  // private:
  uint8_t state = 0;  // 0 method, 1 uri, 2 version, 3 header key, 4 header value, 5 body start
  int cursor = 0;     // keeps track of current location in the req buffer
  char method[10];
  char uri[2000];
  char version[10];
  Map headers[36];
  uint8_t header_count = 0;
  uint8_t content_length = 0;
  uint8_t keep_alive = 0;
  uint8_t chunked = 0;
  char *body;

};

void parse_uri(char *uri) {
  int i = 0;
  for(auto c : uri) {
    switch(c) {

    case '/':
      break;

    case '?':
      break;

    case '#':
      break;
    }
  }
}

void parse_headers(http_msg *msg) {
  int i = 0;

  do {
    //    printf("------ Char %c\n", msg->headers[i].key[0]);
    switch(msg->headers[i].key[0]) {
    case 'C':
      switch(msg->headers[i].key[3]) {
      case 'T':
	msg->content_length = atoi(msg->headers[i].val);
	break;

      case 'N':
	if (msg->headers[i].val[0] == 'k') msg->keep_alive = 1;
	break;
      }

      break;

    case 'T':
      if (msg->headers[i].val[0] == 'c') msg->chunked = 1;
      break;

    case 'K':
      break;


      break;

    }

    i++;
  } while (i < msg->header_count);
}

void parse_msg(const char *req, http_msg *msg) {
  int i = 0;

  do {
    char c = req[i];

    switch(c) {

    case ' ':
      switch(msg->state) {
      case 0:
	// HTTP Method
	memcpy(&msg->method, &req[msg->cursor], i - msg->cursor);
	msg->state = 1;
	msg->cursor = i;
	break;

      case 1:
	// URI
	memcpy(&msg->uri, &req[msg->cursor + 1],  i - msg->cursor - 1);
	msg->state = 2;
	msg->cursor = i;
	parse_uri(msg->uri);
	break;

      case 3:
	// Headers Start
	msg->headers[msg->header_count] = {};
	memcpy(&msg->headers[msg->header_count].key, &req[msg->cursor + 1],  i - msg->cursor - 2);
	msg->state = 4;
	msg->cursor = i;
	break;
      }

       break;

    case '\n':
      if (req[i-2] == '\n') msg->state = 5;
      switch(msg->state) {
      case 2:
	// HTTP Version
	if (i - msg->cursor > 0) {
	  memcpy(&msg->version, &req[msg->cursor + 1], i - msg->cursor - 2);
	  // @Temporary I think this is only required for priting
	  //	  msg->version[ i - msg->cursor] = '\0';
	  msg->state = 3;
	  msg->cursor = i;
	  break;
	}

      case 4:
	// Headers end
	memcpy(&msg->headers[msg->header_count].val, &req[msg->cursor + 1],  i - msg->cursor - 2);
	msg->header_count++;
	msg->state = 3;
	msg->cursor = i;
	break;
      }

      break;
    }

    i++;
  } while(msg->state < 5);

  msg->cursor = msg->cursor + 3; // This is a constant

  parse_headers(msg);
  // GET should not have a body
  // Maybe we change this ...
  // if (msg->method[0] == 'P') //  || msg->method[0] == 'D') // Should delete be allowed a body?
  //   for (int i = 0 ; i < msg->header_count ; i++) {
  //     if(( msg->headers[i].key[0] == 'c' ||  msg->headers[i].key[0] == 'C') && (msg->headers[i].key[8] == 'l' || msg->headers[i].key[8] == 'L' )) {
  //     msg->content_length = atoi(msg->headers[i].val);
  //     break;
  //   }
  // }

  memcpy(msg->body, &req[msg->cursor], msg->content_length);
}

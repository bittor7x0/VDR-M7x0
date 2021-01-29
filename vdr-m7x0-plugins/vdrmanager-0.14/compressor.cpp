/*
 * compressor.cpp
 *
 *  Created on: 23.03.2013
 *      Author: bju
 */

#if VDRMANAGER_USE_ZLIB || VDRMANAGER_USE_GZIP

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "compressor.h"

#if VDRMANAGER_USE_ZLIB
#include <zlib.h>

#define CHUNK 16384
#endif


cCompressor::cCompressor() {
  size = 0;
  data = NULL;
}

cCompressor::~cCompressor() {

}

#if VDRMANAGER_USE_GZIP

bool cCompressor::CompressGzip(string text) {

  int in_fd[2];
  if (pipe(in_fd) < 0) {
    return false;
  }

  int out_fd[2];
  if (pipe(out_fd) < 0) {
    return false;
  }

  int pid = fork();
  if (pid < 0) {
    return false;
  }

  if (pid == 0) {
    // child
    close(in_fd[1]);
    close(out_fd[0]);
    dup2(in_fd[0], 0);
    dup2(out_fd[1], 1);

    execlp("gzip", "gzip", "-c", "-9", NULL);

    exit(-1);

  } else {
    // parent
    close(in_fd[0]);
    close(out_fd[1]);

    if (write(in_fd[1], text.c_str(), text.length()) != text.length())
    {
      close(in_fd[1]);
      close(out_fd[0]);
      return false;
    }

    close(in_fd[1]);

    char buf[32*1024];

    for(;;) {
      int count = read(out_fd[0], buf, sizeof(buf));
      if (count < 0) {
        close(out_fd[0]);
        return false;
      }
      if (count == 0)
        break;

      char * newdata = (char *)malloc(size + count);
      if (data != NULL) {
        memcpy(newdata, data, size);
      }
      memcpy(newdata + size, buf, count);
      if (data != NULL) {
        free(data);
      }
      data = newdata;
      size += count;
    }

    close(out_fd[0]);
  }

  return true;
}

#endif

#if VDRMANAGER_USE_ZLIB

bool cCompressor::CompressZlib(string text) {

  int ret, flush;
  unsigned have;
  z_stream strm;
  unsigned char in[CHUNK];
  unsigned char out[CHUNK];
  int level = 9;

  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  ret = deflateInit(&strm, level);
  if (ret != Z_OK)
      return false;

  string input = text;
  uInt len;

  do {
    len = input.length();
    if (len > CHUNK) {
      len = CHUNK;
    }
    flush = len > 0 ? Z_NO_FLUSH : Z_FINISH;

    strm.avail_in = len;
    strm.next_in = (unsigned char *)input.c_str();

    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;

      ret = deflate(&strm, flush);
      if (ret < 0) {
        if (data != NULL) {
          free(data);
        }
        return false;
      }

      have = CHUNK - strm.avail_out;

      char * newdata = (char *)malloc(size + have);
      if (data != NULL) {
        memcpy(newdata, data, size);
      }
      memcpy(newdata + size, out, have);
      if (data != NULL) {
        free(data);
      }
      data = newdata;
      size += have;

    } while (strm.avail_out == 0);

    input = input.substr(len);

  } while (flush != Z_FINISH);

  // clean up and return
  (void)deflateEnd(&strm);

  return true;
}

#endif

char * cCompressor::GetData() {
  return data;
}

size_t cCompressor::getDataSize() {
  return size;
}

#endif

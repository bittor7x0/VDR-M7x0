/*
 * compressor.h
 *
 *  Created on: 23.03.2013
 *      Author: bju
 */
#ifndef COMPRESSOR_H_
#define COMPRESSOR_H_

#include <string>

#define COMPRESSION_NONE 0

#if VDRMANAGER_USE_ZLIB || VDRMANAGER_USE_GZIP

#define COMPRESSION_ZLIB 1
#define COMPRESSION_GZIP 2

using namespace std;

class cCompressor {
private:
  char * data;
  size_t size;
public:
  cCompressor();
  virtual ~cCompressor();
#if VDRMANAGER_USE_GZIP
  bool CompressGzip(string text);
#endif
#if VDRMANAGER_USE_ZLIB
  bool CompressZlib(string text);
#endif
  char * GetData();
  size_t getDataSize();
};

#endif

#endif /* COMPRESSOR_H_ */


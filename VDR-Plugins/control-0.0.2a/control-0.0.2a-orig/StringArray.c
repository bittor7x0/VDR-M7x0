/*
 * StringArray.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE


#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "StringArray.h"


StringArray::StringArray()
: _stringArray ( 0 ),
  _count       ( 0 )
{
}



StringArray::~StringArray() {
  Clear();
}



void StringArray::Append(const char* String) {

  _stringArray = (char**)realloc(_stringArray, sizeof(char*) * ++_count);
  _stringArray[ _count ] = strdup(String);
}



long StringArray::Count() const {
  return _count;
}



char*& StringArray::operator[](long index0) {

  if (index0 >= _count) {
    _stringArray = (char**)realloc(_stringArray, sizeof(char*) * (index0 + 1));

    for (long i = _count; i <= index0; ++i)
      _stringArray[ i ] = 0;

    _count = index0 + 1;
  }

  return _stringArray[ index0 ];
}



const char* StringArray::operator[](long index0) const {

  if (index0 >= _count) {
    _stringArray = (char**)realloc(_stringArray, sizeof(char*) * (index0 + 1));

    for (long i = _count; i <= index0; ++i)
      _stringArray[ i ] = 0;

    _count = index0 + 1;
  }

  return _stringArray[ index0 ];
}



void StringArray::Clear() {

  for (long i = 0; i < _count; ++i)
    free(_stringArray[ i ]);

  free(_stringArray );

  _stringArray = 0;
  _count = 0;
}



long StringArray::Search(const char* String) const {

  for (long i = 0; i < _count; ++i) {

    char* Text = _stringArray[ i ];

    if (Text == String)
      return i;

    if (Text == 0 || String == 0)
      return -1;

    if (strcmp(_stringArray[ i ], String) == 0)
      return i;
  }

  return -1;
}



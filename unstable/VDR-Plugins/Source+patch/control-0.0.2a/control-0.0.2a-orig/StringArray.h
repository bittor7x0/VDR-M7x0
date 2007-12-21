/*
 * StringArray.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _CTRLSTRINGARRAY_H_
#define _CTRLSTRINGARRAY_H_



class StringArray  {

private:
  StringArray(const StringArray&);

public:
  StringArray();
  ~StringArray();

  void Append(const char* String);
  long Count() const;
  char*& operator[](long index0);
  const char* operator[](long index0) const;
  void Clear();

  long Search(const char* String) const;

private:
  mutable char** _stringArray;
  mutable long _count;
};



#endif // _CTRLSTRINGARRAY_H_



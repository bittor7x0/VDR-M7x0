/***************************************************************************
 *       Copyright (c) 2003 by Marcel Wiesweg, Rolf Hakenes                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <string.h>
#include "util.h"

namespace SI {

/*---------------------------- CharArray ----------------------------*/

CharArray::CharArray() : data_(0), off(0) {
}

CharArray::~CharArray() {
   if (!data_)
      return;
   if (--data_->count_ == 0)
      delete data_;
}

CharArray::CharArray(const CharArray &f) : data_(f.data_), off(f.off) {
   if (data_)
      ++ data_->count_;
}

CharArray& CharArray::operator=(const CharArray &f) {
    // DO NOT CHANGE THE ORDER OF THESE STATEMENTS!
    // (This order properly handles self-assignment)
    if (f.data_) {
      ++ f.data_->count_;
    }
    if (data_) {
      if (--data_->count_ == 0)
         delete data_;
    }
    data_ = f.data_;
    off = f.off;
    return *this;
}

void CharArray::assign(const unsigned char*data, int size, bool doCopy) {
    //immutable
    if (!data_)
      data_= doCopy ? (Data*)new DataOwnData() : (Data*)new DataForeignData();
    // This method might need to change things in *data_
    // Thus it first checks if this is the only pointer to *data_
    if (data_->count_ > 1) {
      Data* d = doCopy ? (Data*)new DataOwnData() : (Data*)new DataForeignData();
      -- data_->count_;
      data_ = d;
    }
    data_->assign(data, size);
}

bool CharArray::operator==(const char *string) const {
   //here we can use strcmp, string is null-terminated.
   if (!data_)
      return false;
   return data_->size ? (!strcmp((const char*)data_->data, string)) : string[0]==0;
}

bool CharArray::operator==(const CharArray &other) const {
   if (!data_ || !other.data_)
      return !(data_ || other.data_); //true if both empty

   if (data_->size != other.data_->size)
      return false;

   //do _not_ use strcmp! Data is not necessarily null-terminated.
   for (int i=0;i<data_->size;i++)
      if (data_->data[i] != other.data_->data[i])
         return false;
   return true;
}

CharArray CharArray::operator+(const int offset) const {
   CharArray f(*this);
   f.off+=offset;
   return f;
}

CharArray::Data::Data() : data(0), size(0), count_(1), valid(true) {
   /*
   lockingPid = 0;
   locked = 0;
   pthread_mutex_init(&mutex, NULL);
   */
}

CharArray::Data::~Data() {
   /*
   if (locked)
      pthread_mutex_unlock(&mutex);
   pthread_mutex_destroy(&mutex);
   */
}

/*CharArray::Data::Data(const Data& d) : count_(1) {
   size=0;
   data=0;

   lockingPid = 0;
   locked = 0;
   pthread_mutex_init(&mutex, NULL);
}*/

CharArray::DataOwnData::~DataOwnData() {
   Delete();
}

void CharArray::DataOwnData::assign(const unsigned char*d, int s) {
   Delete();
   if (!d || s > 100000 || s <= 0) // ultimate plausibility check
      return;
   size=s;
   unsigned char *newdata=new unsigned char[size];
   memcpy(newdata, d, size);
   data=newdata;
}

void CharArray::DataOwnData::Delete() {
   delete[] data;
   size=0;
   data=0;
}

CharArray::DataForeignData::~DataForeignData() {
   Delete();
}

void CharArray::DataForeignData::assign(const unsigned char*d, int s) {
   size=s;
   data=d;
}

void CharArray::DataForeignData::Delete() {
   //do not delete!
}

/*
void CharArray::Data::assign(int s) {
   if (data)
      delete[] data;
   size=s;
   if (size) { //new assignment may be zero length
      data=new unsigned char[size];
      memset(data, 0, size);
   }
}

void CharArray::Data::Lock(void)
{
  if ( !pthread_equal(pthread_self(), lockingPid) || !locked) {
     pthread_mutex_lock(&mutex);
     lockingPid = pthread_self();
     }
  locked++;
}

void CharArray::Data::Unlock(void)
{
   if (!--locked) {
     lockingPid = 0;
     pthread_mutex_unlock(&mutex);
   }
}
*/

Parsable::Parsable() {
   parsed=false;
}

void Parsable::CheckParse() {
   if (!parsed) {
      parsed=true;
      Parse();
   }
}

//taken and adapted from libdtv, (c) Rolf Hakenes and VDR, (c) Klaus Schmidinger
time_t DVBTime::getTime(unsigned char date_hi, unsigned char date_lo, unsigned char time_hour, unsigned char time_minute, unsigned char time_second) {
//M7X0 BEGIN AK
   u_int32_t mjd = date_hi << 8 | date_lo;
   /* undefined time: all bits are set to 1 */
   if ((mjd == 0xffff) & (time_hour == 0xff) & (time_minute == 0xff) &
        (time_second == 0xff))
      return -1;
/* Thanks to LutherBlissett who pointed this out
 * modified julian date can be converted like this.
 * It measures days from 1858-11-17 00:00 UTC. (40587 days to epoch)
 * No leapseconds or timezones have to be considered.
 */
   return time_t((mjd - 40587) * 24 * 60 * 60 +
                 60 * 60 * bcdToDec(time_hour) +
                 60 * bcdToDec(time_minute) +
                 bcdToDec(time_second));
//M7X0 END AK
}

time_t DVBTime::getDuration(unsigned char time_hour, unsigned char time_minute, unsigned char time_second) {
   return
     bcdToDec(time_second)
   + bcdToDec(time_minute) * 60
   + bcdToDec(time_hour) *3600;
}

//taken and adapted from libdtv, (c) Rolf Hakenes
// CRC32 lookup table for polynomial 0x04c11db7
// swapped bytes to avoid one shift operation in CRC loop (c) Reinhard Nissl
u_int32_t CRC32::crc_table[256] = {
   0x00000000, 0xb71dc104, 0x6e3b8209, 0xd926430d, 0xdc760413, 0x6b6bc517,
   0xb24d861a, 0x0550471e, 0xb8ed0826, 0x0ff0c922, 0xd6d68a2f, 0x61cb4b2b,
   0x649b0c35, 0xd386cd31, 0x0aa08e3c, 0xbdbd4f38, 0x70db114c, 0xc7c6d048,
   0x1ee09345, 0xa9fd5241, 0xacad155f, 0x1bb0d45b, 0xc2969756, 0x758b5652,
   0xc836196a, 0x7f2bd86e, 0xa60d9b63, 0x11105a67, 0x14401d79, 0xa35ddc7d,
   0x7a7b9f70, 0xcd665e74, 0xe0b62398, 0x57abe29c, 0x8e8da191, 0x39906095,
   0x3cc0278b, 0x8bdde68f, 0x52fba582, 0xe5e66486, 0x585b2bbe, 0xef46eaba,
   0x3660a9b7, 0x817d68b3, 0x842d2fad, 0x3330eea9, 0xea16ada4, 0x5d0b6ca0,
   0x906d32d4, 0x2770f3d0, 0xfe56b0dd, 0x494b71d9, 0x4c1b36c7, 0xfb06f7c3,
   0x2220b4ce, 0x953d75ca, 0x28803af2, 0x9f9dfbf6, 0x46bbb8fb, 0xf1a679ff,
   0xf4f63ee1, 0x43ebffe5, 0x9acdbce8, 0x2dd07dec, 0x77708634, 0xc06d4730,
   0x194b043d, 0xae56c539, 0xab068227, 0x1c1b4323, 0xc53d002e, 0x7220c12a,
   0xcf9d8e12, 0x78804f16, 0xa1a60c1b, 0x16bbcd1f, 0x13eb8a01, 0xa4f64b05,
   0x7dd00808, 0xcacdc90c, 0x07ab9778, 0xb0b6567c, 0x69901571, 0xde8dd475,
   0xdbdd936b, 0x6cc0526f, 0xb5e61162, 0x02fbd066, 0xbf469f5e, 0x085b5e5a,
   0xd17d1d57, 0x6660dc53, 0x63309b4d, 0xd42d5a49, 0x0d0b1944, 0xba16d840,
   0x97c6a5ac, 0x20db64a8, 0xf9fd27a5, 0x4ee0e6a1, 0x4bb0a1bf, 0xfcad60bb,
   0x258b23b6, 0x9296e2b2, 0x2f2bad8a, 0x98366c8e, 0x41102f83, 0xf60dee87,
   0xf35da999, 0x4440689d, 0x9d662b90, 0x2a7bea94, 0xe71db4e0, 0x500075e4,
   0x892636e9, 0x3e3bf7ed, 0x3b6bb0f3, 0x8c7671f7, 0x555032fa, 0xe24df3fe,
   0x5ff0bcc6, 0xe8ed7dc2, 0x31cb3ecf, 0x86d6ffcb, 0x8386b8d5, 0x349b79d1,
   0xedbd3adc, 0x5aa0fbd8, 0xeee00c69, 0x59fdcd6d, 0x80db8e60, 0x37c64f64,
   0x3296087a, 0x858bc97e, 0x5cad8a73, 0xebb04b77, 0x560d044f, 0xe110c54b,
   0x38368646, 0x8f2b4742, 0x8a7b005c, 0x3d66c158, 0xe4408255, 0x535d4351,
   0x9e3b1d25, 0x2926dc21, 0xf0009f2c, 0x471d5e28, 0x424d1936, 0xf550d832,
   0x2c769b3f, 0x9b6b5a3b, 0x26d61503, 0x91cbd407, 0x48ed970a, 0xfff0560e,
   0xfaa01110, 0x4dbdd014, 0x949b9319, 0x2386521d, 0x0e562ff1, 0xb94beef5,
   0x606dadf8, 0xd7706cfc, 0xd2202be2, 0x653deae6, 0xbc1ba9eb, 0x0b0668ef,
   0xb6bb27d7, 0x01a6e6d3, 0xd880a5de, 0x6f9d64da, 0x6acd23c4, 0xddd0e2c0,
   0x04f6a1cd, 0xb3eb60c9, 0x7e8d3ebd, 0xc990ffb9, 0x10b6bcb4, 0xa7ab7db0,
   0xa2fb3aae, 0x15e6fbaa, 0xccc0b8a7, 0x7bdd79a3, 0xc660369b, 0x717df79f,
   0xa85bb492, 0x1f467596, 0x1a163288, 0xad0bf38c, 0x742db081, 0xc3307185,
   0x99908a5d, 0x2e8d4b59, 0xf7ab0854, 0x40b6c950, 0x45e68e4e, 0xf2fb4f4a,
   0x2bdd0c47, 0x9cc0cd43, 0x217d827b, 0x9660437f, 0x4f460072, 0xf85bc176,
   0xfd0b8668, 0x4a16476c, 0x93300461, 0x242dc565, 0xe94b9b11, 0x5e565a15,
   0x87701918, 0x306dd81c, 0x353d9f02, 0x82205e06, 0x5b061d0b, 0xec1bdc0f,
   0x51a69337, 0xe6bb5233, 0x3f9d113e, 0x8880d03a, 0x8dd09724, 0x3acd5620,
   0xe3eb152d, 0x54f6d429, 0x7926a9c5, 0xce3b68c1, 0x171d2bcc, 0xa000eac8,
   0xa550add6, 0x124d6cd2, 0xcb6b2fdf, 0x7c76eedb, 0xc1cba1e3, 0x76d660e7,
   0xaff023ea, 0x18ede2ee, 0x1dbda5f0, 0xaaa064f4, 0x738627f9, 0xc49be6fd,
   0x09fdb889, 0xbee0798d, 0x67c63a80, 0xd0dbfb84, 0xd58bbc9a, 0x62967d9e,
   0xbbb03e93, 0x0cadff97, 0xb110b0af, 0x060d71ab, 0xdf2b32a6, 0x6836f3a2,
   0x6d66b4bc, 0xda7b75b8, 0x035d36b5, 0xb440f7b1};

inline void swap_bytes(u_int32_t &crc)
{
  unsigned char a = crc >> 24;
  unsigned char b = crc >> 16;
  unsigned char c = crc >> 8;
  unsigned char d = crc;

  crc = ((d << 8 | c) << 8 | b) << 8 | a;
}

u_int32_t CRC32::crc32 (const char *d, int len, u_int32_t crc)
{
   register int i;
   const unsigned char *u=(unsigned char*)d; // Saves '& 0xff'

   swap_bytes(crc);

   for (i=0; i<len; i++)
      crc = (crc >> 8) ^ crc_table[(unsigned char)crc ^ *u++];

   swap_bytes(crc);

   return crc;
}

CRC32::CRC32(const char *d, int len, u_int32_t CRCvalue) {
   data=d;
   length=len;
   value=CRCvalue;
}

} //end of namespace

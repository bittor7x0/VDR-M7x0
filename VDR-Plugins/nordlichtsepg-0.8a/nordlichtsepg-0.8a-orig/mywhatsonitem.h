/*
 * See the README file for copyright information and how to reach the author.
 */

#include "nordlichtsepg.h"

extern int enigmatheme;
extern int showchannelnumbers;
extern time_t t;

class myWhatsOnItem:public cOsdItem
{
 private:
  bool next;
 public:
  const cEvent *event;
  const cChannel *channel;
  cTimer *timer;
  virtual void Set();
  myWhatsOnItem(const cEvent *Event,cChannel *Channel,bool Next);
};

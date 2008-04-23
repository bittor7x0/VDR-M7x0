/*
 * See the README file for copyright information and how to reach the author.
 */

#include "nordlichtsepg.h"
#include "mywhatsonitem.h"

class myMenuEvent:public cOsdMenu
{
private:
 eOSState Record(void);
 eOSState Switch(void);
 eOSState Delete(void);
 const cEvent *event;
 const cChannel *channel;
 cTimer *timer;
 myWhatsOnItem *item;
public:
 myMenuEvent(myWhatsOnItem *Item);
 virtual void Display(void);
 virtual eOSState ProcessKey(eKeys Key);
};

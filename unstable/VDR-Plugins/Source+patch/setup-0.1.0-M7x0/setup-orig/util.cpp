/****************************************************************************
 * DESCRIPTION: 
 *             Utitility Methods
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de
 *
 * Copyright (C) 2004 by Ralf Dotzert 
 ****************************************************************************/


#include <string.h>
#include <ctype.h>
#include "util.h"
#include "debug.h"

#define NUL '\0'

Util::Util()
{
}


Util::~Util()
{
}

static const char *undefStr="undefined";
static const char *boolStr="bool";
static const char *textStr="text";
static const char *numTextStr="numtext";
static const char *hexStr="hex";
static const char *ipStr="ip";
static const char *numberStr="number";
static const char *selectionStr="selection";


static const char *trueStr="true";
static const char *falseStr="false";
static const char *yesStr="yes";
static const char *noStr="no";


char * Util::Strdupnew( const char  *str )
{
  return ( str ? strcpy(new char[strlen(str)+1],str) : 0);
}

char * Util::Strdupnew( const char  *str, int size )
{
  char * result = NULL;
  if( str != NULL)
  {
    result = strncpy(new char[size+1],str, size);
    result[size] = '\0';
  }

  return (result);
}



char * Util::Strdupnew( const char  *prefix, const char  *str )
{
  char *newStr = NULL;

  if( str != NULL && prefix != NULL)
  {
    int len = strlen(prefix) +strlen(str);
    newStr = new char[(len+1)];
    sprintf(newStr, "%s%s", prefix, str);
  }
  return (newStr);
}


/**
 * check id the given string represenst a bool value
 * @param string string to compare for bool representation
 * @param flag corresponding bool value
 * @return false if error was detected
 */
bool Util::isBool( const char * string, bool & flag )
{
  bool ok=true;

  if( string != NULL)
  {
    if(strcmp(string , trueStr) == 0 ||
       strcmp(string , yesStr)  == 0)
      flag = true;
    else
    if(strcmp(string , falseStr) == 0 ||
       strcmp(string , noStr)  == 0)
      flag = false;
    else
      ok=false;
  }
  else
    ok = false;

  if(! ok)
    DEBUG3("%s: Illegal Bool value %s found\n", DBG_PREFIX, string);
    
  return(ok);
}

bool Util::isBool( const char * string, int & flag )
{
  bool ok=true;
  bool boolVal;

  if(isBool(string, boolVal))
  {
    if( boolVal)
      flag=true;
    else
      flag=false;
  }
   
  return(ok);
}


/**
 * check id the given string represenst a Type value
 *
 * known types are "text", "bool", "number", "numtext" "ip"
 * @param string string to compare for type representation
 * @param typ corresponding type value
 * @return false if error was detected
 */
bool Util::isType( const char * string, Util::Type & type )
{
  bool ok=true;

  if( string != NULL)
  {
    if(strcmp(string , boolStr) == 0)   
      type = Util::BOOL;
    else
    if(strcmp(string , textStr) == 0 )
       type = Util::TEXT;
    else
    if(strcmp(string , numTextStr) == 0 )
       type = Util::NUMBER_TEXT;
    else
    if(strcmp(string , hexStr) == 0 )
       type = Util::HEX;
    else
    if(strcmp(string , numberStr) == 0 )
       type = Util::NUMBER;
    else
    if(strcmp(string , ipStr) == 0 )
        type = Util::IP;
    else
    if(strcmp(string , selectionStr) == 0 )
       type = Util::SELECTION;
    else
      ok=false;
  }
  else
    ok = false;

  if(! ok)
    DEBUG3("%s: Illegal Type value %s found\n", DBG_PREFIX, string);
    
  return(ok);
}


/**
 * 
 * @param string 
 * @param number 
 * @return 
 */
bool Util::isNumber( const char * string, int & number )
{
  bool ok = true;
  number = 0;

  if( string != NULL)
  {
    int len = strlen(string);
    for(int i=0; i<len && ok==true; i++)
        if(string[i] < '0' || string[i]>'9')
           ok=false;
        else
          number = number * 10 + (string[i] - '0');
  }
  else
    ok = false;
  
  return(ok);
  
}


/**
 * Returns the strung representation of given type
 * @param type 
 * @return 
 */
const char * Util::boolToStr( bool val )
{
  const char * result;

  if( val == true)
    result = yesStr;
  else
    result = noStr;

  
  return(result);
}



/**
 * Returns the strung representation of given type
 * @param type 
 * @return 
 */
const char * Util::typeToStr( Type type )
{
  const char * result = undefStr;
  switch(type)
  {
    case BOOL:      result =boolStr;
                    break;
    case TEXT:      result =textStr;
                    break;
    case NUMBER_TEXT:
                    result=numTextStr;
                    break;
    case HEX:       result=hexStr;
                    break;
    case NUMBER:    result =numberStr;
                    break;
    case IP:        result =ipStr;
                    break;
    case SELECTION:
                    result =selectionStr;
                    break;
    case UNDEFINED:
                    result=undefStr;
                    break;
  }
  return(result);
}

void Util::SwitchChannelList( const char * selectedChannelList )
{
  char * channelDir  =NULL;
  char * channelFile =NULL;
  char   resolvedPath[PATH_MAX];
  char  *newChannelFile = NULL;

  if( selectedChannelList == NULL)
    return;

  asprintf(&channelDir,"%s", cPlugin::ConfigDirectory());
  char *tmp = strrchr(channelDir,'/');
  *tmp = '\0';

  asprintf(&channelFile,    "%s/channels.conf",channelDir);
  asprintf(&channelDir,     "%s/channels",channelDir);
  asprintf(&newChannelFile, "%s/%s.conf", channelDir, selectedChannelList);

  if( realpath(channelFile, resolvedPath) == NULL)
  {
    DEBUG4("%s Can not resolve realpath of %s  errno=%d\n", DBG_PREFIX, channelFile, errno );
  }
  else
  {
    if( strcmp(resolvedPath, newChannelFile)!= 0) // channels changed !!
    {
      Channels.SwitchTo(1);
      if( unlink(channelFile) == 0 )
      {
        if( symlink(newChannelFile, channelFile)!= 0){
          DEBUG5("%s Can not link File %s  to %s errno=%d\n",DBG_PREFIX,  tmp, channelFile, errno);
        }
        TimerList timers;
        timers.SaveTimer();
        Channels.Load(channelFile, true, false); // Read Channel-List
        Channels.SwitchTo(1);
        timers.RestoreTimer();
      }
      else{
        DEBUG4("%s Can not unlink File %s  errno=%d\n",DBG_PREFIX, channelFile, errno);
      }
    }
  }

  free(channelDir);
  free(channelFile);
  free(newChannelFile);
}



// Timer Save and Restore Methods
void TimerList::SaveTimer( )
{
  myTimers.Clear();
  for (cTimer *t = Timers.First(); t; t= Timers.Next(t)){
    myTimers.Add(new TimerString(t->ToText(true)));
  }
  Timers.cList<cTimer>::Clear();
}

// restore Timers, delete Timers with unknown channel
void TimerList::RestoreTimer( )
{
  Timers.cList<cTimer>::Clear();

  for (TimerString *timer = myTimers.First(); timer; timer= myTimers.Next(timer)){
    cString t = timer->GetTimerString();
    cTimer *tim = new cTimer();
    tim->Parse(t);
    if( tim->Channel() != NULL)
      Timers.Add(tim);
  }
  myTimers.Clear();
}




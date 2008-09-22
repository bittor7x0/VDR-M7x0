/****************************************************************************
 * DESCRIPTION: 
 *             Handles sysconfig File
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de
 *
 * Copyright (C) 2004 by Ralf Dotzert 
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "vdr/plugin.h"
#include "vdr/tools.h"
#include "sysconfig.h"
#include "util.h"
#include "common.h"


Variable::Variable( )
{
  _name  = NULL;
  _value = NULL;
}

Variable::~ Variable( )
{
  delete [] _name;
  delete [] _value;
}




void Variable::SetName( const char * nam )
{
  delete [] _name;
 _name = Util::Strdupnew(nam);
}

const char * Variable::GetName( )
{
  return(_name);
}

void Variable::SetValue( const char * val )
{
  delete [] _value;
  _value =  Util::Strdupnew(val);
}

const char * Variable::GetValue( )
{
  return(_value);
}
/**
 * print Variable
 */
void Variable::Print( )
{
  printf("Name=");
  if( _name == NULL)
    printf("NULL");
  else
    printf(_name);

  printf(" Value=");
  if( _value == NULL)
    printf("NULL");
  else
    printf(_value);
  printf("\n"); 
}




//------------------------------------------------------

Variable * Sysconfig::_variables=NULL;
int        Sysconfig::_nr =0;
char      *Sysconfig::_filename=NULL;

Sysconfig::Sysconfig()
{
  _variables = NULL;
  _filename  = NULL;
  _nr        = 0;
}


Sysconfig::~Sysconfig()
{
  destroy();
}

void Sysconfig::destroy( )
{

  delete [] _variables;
  delete [] _filename;

  _variables=NULL;
  _filename=NULL;
  _nr=0;
}



bool Sysconfig::LoadFile( const char*fname)
{
   bool result = true;
   const char * line;
   destroy();
   _filename = Util::Strdupnew(fname);
   FILE * fp = fopen(fname, "r");
	debug("Load sysconf:[%s]",fname);
	
   if( fp != NULL)
   {     
     while( (line = readLine(fp)) != NULL)
     {
       addLine(line);
       delete [] line;
     }
    
     fclose(fp);
   }
   else
   {
     error("Could not read sysconfigFile: [%s], errno=%d", fname, errno);
   }

   return(result);
}

/**
 * Save Sysconfig file
 * @return true on success
 */
bool Sysconfig::SaveFile( )
{
  bool result=true;
  debug("Saving sysconf: [%s]",_filename);
   FILE * fp = fopen(_filename, "w");
   if(fp != NULL)
   {
     for(int i=0; i< _nr; i++)
     {
       		fprintf(fp, "%s=%s\n", _variables[i].GetName(), _variables[i].GetValue());
     }
     isyslog("Saved file [%s]",_filename);
     fclose(fp);
   }
   else
    {
     debug("Could not write file: [%s], errno=%d", _filename, errno);
     result=false;
    }
  return(result);
}

/**
 * read one line from opened file
 * @param fp opened filepointer
 * @return null if EOF or allocated character String holding one line
 */
const char * Sysconfig::readLine( FILE * fp )
{
  char myChar;
  char buf[1024];
  int  i=0;
  int  maxLen = (int) sizeof(buf)-1;
  char *line=NULL;
  while((myChar=fgetc(fp)) != EOF )
  {
    if(myChar=='\n' || i==maxLen)
    {
       line = new char[i+1];
       strncpy(line, buf, i);
       line[i]='\0';
       return(line);
    }
    else
      buf[i++]=myChar;
  }
  return((const char*)line);
}

/**
 * Add the given line from sysconfig file an split it in Name an Variable
 * @param line allocated buffer holding one line
 */
void Sysconfig::addLine(const char * line )
{
  char *nam=NULL;
  char *val=NULL;
  debug ("sysconfig add line:%s",line);
  if(strlen(line)>0 && line[0] != '#' )
  {//Line with variable
    if( (nam =strtok((char*)line, "=")) != NULL &&
    	(val=strtok(NULL, "\0")) != NULL )
    {
//      addVariable(Util::Strtrim(nam), Util::Strtrim(val));
      addVariable(compactspace(nam), compactspace(val));
    }
  }
  
 
}

/**
 * add new Variable to internal Variable list
 * @param name specifiesd the name of the variables
 * @param value specifies the value of the variable
 */
void Sysconfig::addVariable( const char * name, const char * value )
{
    Variable *v = new Variable[_nr+1];
	debug ("sysconfig add variable:%s=\"%s\"",name,value);
 
    //copy existing Array elements
    for(int i=0; i<_nr; i++)
    {
    //v[i] = _variables[i];
    v[i].SetName(_variables[i].GetName());
    v[i].SetValue(_variables[i].GetValue());
    }

    delete []_variables;

    v[_nr].SetName(name);
    v[_nr].SetValue(value);
    _variables=v;
    _nr++;  
}



/**
 * 
 */
void Sysconfig::Print( )
{
  for(int i=0; i<_nr; i++)
    _variables[i].Print();
}

/**
 * Set or overwrite the specified variable
 * @param name name of variable
 * @param value value of variable
 */
void Sysconfig::SetVariable(const char * name, const char * value )
{
  bool found =false;
  int  index=0;

  debug ("sysconfig set variable:%s=\"%s\"",name,value);

  for(int i=0; i<_nr && found==false; i++)
  {
    if( strcmp(name, _variables[i].GetName())==0 )
    {
      found=true;
      index=i;
    }
  }
  if( found)
    _variables[index].SetValue(value);
  else
    addVariable(name, value);
}

/**
 * returns the current filename of the sysconfig
 * @return return filename
 */
char * Sysconfig::GetFileName( )
{
  return(_filename);
}

/**
 * Get the value of the specified variable
 * @param name name of variable
 */
const char * Sysconfig::GetVariable( const char * name )
{
  bool found =false;
  int  index=0;

  for(int i=0; i<_nr && found==false; i++)
  {
    if( strcmp(name, _variables[i].GetName())==0 )
    {
      found=true;
      index=i;
    }
  }
  if( found)
    return(_variables[index].GetValue());
  else
    return(NULL);
}





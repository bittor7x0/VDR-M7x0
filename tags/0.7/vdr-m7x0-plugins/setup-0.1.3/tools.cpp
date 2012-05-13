/***************************************************************************
                          tools.c  -  description
                             -------------------
    begin                : Sa Jan 15 2005
    copyright            : (C) 2005 by Holger Brunn
    email                : holger.brunn@stud.uni-karlsruhe.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <malloc.h>
#include <ctype.h>
#include "tools.h"

/*
  Implementation cStringList
*/

bool cStringList::Contains(const char* String)
{
  for(cStringContainer *i = First(); i; i=Next(i))
  {
    if(strcmp(i->GetObject(), String)==0)
    {
      return true;
    }
  }
  return false;
}

void cStringList::Remove(const char* String)
{
  for(cStringContainer *i = First(); i!=NULL; i=Next(i))
  {
      if(strcmp(i->GetObject(), String)==0)
      {
        Del(i);
        break;
      }
  }
}

/*
  Implementation of cConfigReader
*/

cConfigReader::cConfigReader(cFilebrowserStatebag* Statebag, char* Filename)
{
  char* Name=(char*)malloc(strlen(Statebag->ConfigDirectory) + strlen(Filename) + 2);
  sprintf(Name, "%s/%s", *(Statebag->ConfigDirectory), Filename);
  File=fopen(Name, "r");
  D(fprintf(stderr, "open file %s %s, handle is %X\n", Name, File ? "succeeded" : "failed", File));
  free(Name);
  Reader=new cReadLine();
}

cConfigReader::~cConfigReader()
{
  D(fprintf(stderr, "cConfigReader destructor called [Handle: %X]\n", File));
  if(File) fclose(File);
  if(Reader) delete(Reader);
  D(fprintf(stderr, "cConfigReader destructor finished\n"));
}

char* cConfigReader::Read()
{
  if(!File)
  {
    return NULL;
  }

  char *line;
  do
  {
    line = Reader->Read(File);
    StripComment(line);
  } while ((line != NULL) && IsBlank(line));

  return line;
}

void cConfigReader::StripComment(char* string)
{
  if (string)
  {
      char *commentPosition = strchr(string, '#');
      if (commentPosition)
      {
        *commentPosition = 0;
      }
  }
}

bool cConfigReader::IsBlank(const char* string)
{
  bool blank = true;
         
  if (string != NULL)
  {
    int len = strlen(string);
    for (int i = 0; i < len; i++)
    {
      if (!isspace(string[i]))
      {
        blank = false;
        break;
      }
    }
  }

  return blank;
}

/*
  Implementation of cConfigParser
*/

cConfigParser::cConfigParser(char* Line, bool TakePointer)
{
  this->Line=TakePointer ? Line : strdup(Line);
  Buffer=(char*)malloc(strlen(Line) + 1);
  Pos=this->Line;
  FieldNumber=-1;
  CurrentLength=-1;
}

cConfigParser::~cConfigParser()
{
  if(Line) free(Line);
  if(Buffer) free(Buffer);
}

char* cConfigParser::Next()
{
  if(Pos!=Line && *(Pos-1)=='\0')
  {
    FieldNumber=-1;
    CurrentLength=-1;
    return NULL;
  }
  char* BufferPos=Buffer;
  for(; Pos==Line || *(Pos-1)!='\0'; Pos++)
  {
    switch(*Pos)
    {
      case '\0':
      case Delimiter:
        Pos++;
        *(BufferPos)='\0';
        FieldNumber++;
        CurrentLength=BufferPos - Buffer;
        return Buffer;
      case Escape:
        if(*(Pos+1)==Delimiter)
        {
          *(BufferPos++)=Delimiter;
          Pos++;
        }
        else
        {
          *(BufferPos++)=*Pos;
        }
        break;
      default:
        *(BufferPos++)=*Pos;
        break;
    }
  }
  return NULL;
}

char* cConfigParser::First()
{
  Pos=Line;
  FieldNumber=-1;
  CurrentLength=-1;
  return Next();
}

/*
  Implementation of cCommandParser
*/

cCommandParser::cCommandParser(const cString &Command, bool DoReplacements, bool ShellEscape)
{
  this->DoReplacements=DoReplacements;
  this->Command=Command;
  this->ShellEscape=ShellEscape;
  Handlers=new cHandlerList();
}

cCommandParser::~cCommandParser()
{
  if(Handlers) delete Handlers;
}

void cCommandParser::AddHandler(char Key, ParseProc Proc, void* Data)
{
  cHandler* NewHandler=new cHandler();
  NewHandler->Proc=Proc;
  NewHandler->Params.Key=Key;
  NewHandler->Params.Data=Data;
  NewHandler->Params.ShellEscape=ShellEscape;
  NewHandler->Params.DoReplacements=DoReplacements;
  Handlers->Add(NewHandler);
}

void cCommandParser::AddReplacement(char Key, const char* String)
{
  AddHandler(Key, &cCommandParser::ReplacementHandler, (void*)String);
}



cString cCommandParser::Parse()
{
  if(!*Command)
  {
    return cString();
  }
  #define BUFLEN 50
  char* Text=(char*)malloc(BUFLEN);
  Text[0]='\0';
  char* TextPos=Text;
  int TextLength=BUFLEN;
  D(fprintf(stderr, "parsing %s\n", *Command));
  for(char* i=(char*)*Command; i==*Command || *(i-1)!='\0'; i++)
  {
    if(TextPos-Text >= TextLength)
    {
      int Pos=TextPos-Text;
      TextLength+=BUFLEN;
      D(fprintf(stderr, "reallocating %d bytes, Text is \"%s\"\n", TextLength, Text));
      Text=(char*)realloc(Text, TextLength);
      TextPos=Text+Pos;
    }
    if(*i=='%')
    {
      char* InsText=NULL;
      int InsTextLength=0;
      cHandler* FoundHandler=NULL;
      for(cHandler* h=Handlers->First();
          h; h=(cHandler*)h->Next())
      {
        if(h->Params.Key==*(i+1))
        {
          FoundHandler=h;
        }
      }
      if(FoundHandler)
      {
        InsText=FoundHandler->Proc(*Command, i, &FoundHandler->Params);
        InsTextLength=(InsText ? strlen(InsText) + 1 : 0);
      }
      else
      {
        InsText=(char*)malloc(2);
        InsTextLength=2;
        sprintf(InsText, "%c", *(i+1));
      }
      if(TextPos+InsTextLength-Text >= TextLength)
      {
        int Pos=TextPos-Text;
        TextLength+=InsTextLength+BUFLEN;
        Text=(char*)realloc(Text, TextLength);
        TextPos=Text+Pos;
      }
      if(InsText)
      {
        strcpy(TextPos, InsText);
        TextPos+=InsTextLength-1;
        free(InsText);
      }
      i++;
      continue;
    }
    *TextPos++=*i;
  }
  D(fprintf(stderr, "Finished parsing - returning %s\n", Text));
  return cString(Text, true);
}

char* cCommandParser::ReplacementHandler(const char* OrgString, const char* CurrentPos, const cHandlerParameters* Params)
{
  if(!Params->DoReplacements)
  {
    return NULL;
  }
  if(Params->ShellEscape)
  {
    return EscapeShellArgument((char*)Params->Data, NULL);
  }
  return strdup((char*)Params->Data);
}

char* cCommandParser::EscapeShellArgument(char* Argument, int* EscapedLength)
{
#define ESCAPE_SEQUENCE "'\\''"
#define ESCAPE_SEQUENCE_LENGTH strlen(ESCAPE_SEQUENCE)
  D(fprintf(stderr, "escaping shell argument %s\n", Argument));
  int Length=strlen(Argument);
  char* result=(char*)malloc(Length*ESCAPE_SEQUENCE_LENGTH + 3);
  char* result_pos=result;
  *result_pos++='\'';
  Length++;
  for(char* i=Argument; *i!='\0'; i++)
  {
    if(*i!='\'')
    {
      *result_pos++=*i;
    }
    else
    {
      strcpy(result_pos, ESCAPE_SEQUENCE);
      result_pos+=ESCAPE_SEQUENCE_LENGTH;
      Length+=ESCAPE_SEQUENCE_LENGTH-1;
    }
  }
  *result_pos++='\'';
  *result_pos++='\0';
  Length++;
  result=(char*)realloc(result, Length+1);
  D(fprintf(stderr, "finished escaping - returning %s[%d]\n", result, Length));
  if(EscapedLength)
  {
    *EscapedLength=Length;
  }
  return result;
}


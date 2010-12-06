/***************************************************************************
                          threads.c  -  description
                             -------------------
    begin                : So Jan 16 2005
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "threads.h"
#include "commands.h"

/*
Implementation cCommandThread
*/

cCommandThread::cCommandThread(cFilebrowserStatebag* Statebag, char* DestinationFile, char* CurrentFile) : cThread()
{
  Description=NULL;
  this->DestinationFile=DestinationFile ? strdup(DestinationFile) : NULL;
  this->CurrentFile=CurrentFile ? strdup(CurrentFile) : NULL;
  this->Statebag=Statebag;
  //deep copy of Selected files
  SelectedFiles=new cStringList();
  for(int i=0; i<Statebag->GetSelectedFiles()->Count(); i++)
  {
    SelectedFiles->Add(new cStringContainer(strdup((char*)Statebag->GetSelectedFiles()->Get(i)->GetObject())));
  }
  State=tsCreated;
  RemoveRequested=false;
}
cCommandThread::~cCommandThread()
{
  if(Description) free (Description);
  if(DestinationFile) free (DestinationFile);
  if(CurrentFile) free (CurrentFile);
  if(SelectedFiles) free(SelectedFiles);
}

cOsdMenu* cCommandThread::GetMenu()
{
  return NULL;
}

void cCommandThread::Cancel(int WaitSeconds)
{
  cThread::Cancel(WaitSeconds);
  State=tsFinished;
}

void cCommandThread::Pause()
{
  if(Running())
  {
    State=tsPaused;
  }
}

void cCommandThread::Resume()
{
  if(Running())
  {
    State=tsRunning;
  }
}

/*
Implementation cCommandThreadTextOutput
*/

cCommandThreadTextOutput::cCommandThreadTextOutput(cFilebrowserStatebag* Statebag, char* DestinationFile, char* CurrentFile) : cCommandThread(Statebag, DestinationFile, CurrentFile)
{
  OutputBufferLength=128;
  OutputBuffer=(char*)malloc(OutputBufferLength);
  memset(OutputBuffer, 0, OutputBufferLength);
  ChildPid=-1;
}

void cCommandThreadTextOutput::Action()
{
  State=tsRunning;

  D(fprintf(stderr, "starting thread\n"));
  int child_output[2];
  pipe(child_output);

  pid_t pid=ChildPid=fork();
  if(pid == 0) //child
  {
    setpgid(ChildPid, 0);

    int MaxPossibleFileDescriptors = getdtablesize();
    for (int i = STDERR_FILENO + 1; i < MaxPossibleFileDescriptors; i++)
    {
      if(/*i!=child_output[0] &&*/ i!=child_output[1])
      {
        close(i); //close all dup'ed filedescriptors
      }	
    }
    
//    close(child_output[0]);
    
    D(fprintf(stderr, "fork succeeded, now redirecting output\n"));
    
    dup2(child_output[1], STDOUT_FILENO);
    dup2(child_output[1], STDERR_FILENO);

    int dev_null=open("/dev/null", O_RDWR, 0);
    dup2(dev_null, STDIN_FILENO);
    close(dev_null);

    Execute();

//    close(child_output[1]);

    return;
  }
  else if (pid < 0)
  {
    D(fprintf(stderr, "fork failed\n"));
    return;
  }
  else // parent
  {
    D(fprintf(stderr, "reading output of child process\n"));
    FILE* out;
    out=fdopen(child_output[0], "r");

    D(fprintf(stderr, "opening output succeeded\n"));
    close(child_output[1]);

    if(out)
    {
      char* caret=OutputBuffer;
      char c;
      while((c=fgetc(out)) != EOF && caret - OutputBuffer < OutputBufferLength)
      {
        switch(c)
        {
          case '\b':
            if(caret > OutputBuffer)
            {
              caret--;
            }
          break;
          case '\r':
          case '\f':
          case '\v':
            for(char* i=caret; i>=OutputBuffer; i--)
            {
              if(i==OutputBuffer)
              {
                caret=i;
                break;
              }
              if(*i=='\n')
              {
                caret=i+1;
                break;
              }
            }
          break;
          default:
            *(caret++)=c;
          break;
        }
        if(caret - OutputBuffer >= OutputBufferLength - 2)
        {
          int Offset=caret - OutputBuffer;
          OutputBufferLength*=2;
          Lock();
          char* TmpBuffer=(char*)realloc(OutputBuffer, OutputBufferLength);
          if(TmpBuffer && OutputBufferLength < 1024*1024)
          {
            OutputBuffer=TmpBuffer;
            caret=OutputBuffer + Offset;
            memset(caret, 0, OutputBufferLength - Offset);
            D(fprintf(stderr, "resized OutputBuffer to %d bytes\nCaret at Position %d is \"%s\"\n", (int)OutputBufferLength, Offset, caret));
          }
          else
          {
            D(fprintf(stderr, "couldn't resize output buffer\n"));
            OutputBuffer=TmpBuffer ? TmpBuffer : OutputBuffer;
            caret=OutputBuffer;
            OutputBufferLength/=2;
            memset(caret, 0, OutputBufferLength);
            //TODO: Error message
          }
          Unlock();
        }
      }

      int ChildStatus;
      D(fprintf(stderr, "Waiting for child %d\n", ChildPid));
      waitpid(ChildPid, &ChildStatus, 0);

      RemoveRequested&=!ChildStatus;

      ChildPid=-1;

      if((long)strlen(OutputBuffer) == OutputBufferLength)
      {
        D(fprintf(stderr, "buffer ran full (%d)\n%s\n", strlen(OutputBuffer), OutputBuffer));
        while((c=fgetc(out)) != EOF) {};
      }
      
      D(fprintf(stderr, "forked thread closed, output was:\n%s\n", OutputBuffer));
      fclose(out);

      Statebag->UpdateRequested=true;
    }
    else
    {
      D(fprintf(stderr, "failed to read output of Execute\n"));
    }
    D(fprintf(stderr, "Closing pipe\n"));
    close(child_output[0]);
    State=tsFinished;
  }
}

cCommandThreadTextOutput::~cCommandThreadTextOutput()
{
  D(fprintf(stderr, "CommandThreadTextOutput: Destructor called\n"));
  Cancel();
  free(OutputBuffer);
}

void cCommandThreadTextOutput::Cancel(int WaitSeconds)
{
  D(fprintf(stderr, "CommandThreadTextOutput: Cancel called"));
  if(ChildPid > 1)
  {
    D(fprintf(stderr, " - killing %d", ChildPid));
    kill(-ChildPid, SIGTERM);
    ChildPid=-1;
  }
  D(fprintf(stderr, "\n"));
  cCommandThread::Cancel(WaitSeconds);
}

cOsdMenu* cCommandThreadTextOutput::GetMenu()
{
  cOsdMenuTextOutput* OutputMenu=new cOsdMenuTextOutput(this);
  OutputMenu->SetText(OutputBuffer);
  return OutputMenu;
}

void cCommandThreadTextOutput::Pause()
{
  if(Running() && ChildPid > 1)
  {
    D(fprintf(stderr, "Sending sigstop to %d\n", ChildPid));
    kill(-ChildPid, SIGSTOP);
  }
  cCommandThread::Pause();
}

void cCommandThreadTextOutput::Resume()
{
  if(Running() && ChildPid > 1)
  {
    D(fprintf(stderr, "Sending sigcont to %d\n", ChildPid));
    kill(-ChildPid, SIGCONT);
  }
  cCommandThread::Resume();
}

/*
Implementation cConfigCommandThread
*/

cConfigCommandThread::cConfigCommandThread(cFilebrowserStatebag* Statebag, char* DestinationFile, char* CurrentFile, cFilebrowserConfigCommand* Command) : cCommandThreadTextOutput(Statebag, DestinationFile, CurrentFile)
{
  this->Command=Command;
  if(Command->UsesCurrentFile())
  {
    Description=(char*)malloc(strlen(Command->GetName()) + strlen(CurrentFile) + 2);
    sprintf(Description, "%s%s%s", Command->GetName(), strcmp("", CurrentFile) == 0 ? "" : " ", strcmp("", CurrentFile) == 0 ? "" : CurrentFile);
  }
  else if(Command->UsesDestination())
  {
    Description=(char*)malloc(strlen(Command->GetName()) + strlen(DestinationFile) + 2 + strlen(tr(" to ")));
    sprintf(Description, "%s%s%s", Command->GetName(), strcmp("", DestinationFile) == 0 ? "" : tr(" to "), strcmp("", DestinationFile) == 0 ? "" : DestinationFile);
  }
  else
  {
    Description=(char*)malloc(strlen(Command->GetName()) + 1);
    sprintf(Description, "%s", Command->GetName());
  }
  SetDescription(Description);
  RemoveRequested=Command->RemoveWhenFinished();
//  D(fprintf(stderr, "got %d marked files\n", Statebag->GetSelectedFiles()->Count()));
  D(fprintf(stderr, "created thread %s\n", Description));
}

cOsdMenu* cConfigCommandThread::GetMenu()
{
  cOsdMenuTextOutput* OutputMenu=(cOsdMenuTextOutput*)cCommandThreadTextOutput::GetMenu();
  OutputMenu->SetTitle(Description);
  return OutputMenu;
}

char* cConfigCommandThread::SelectedFilesHandler(const char* OrgString, const char* CurrentPos, const cCommandParser::cHandlerParameters* Params)
{
  cStringList* SelectedFiles=(cStringList*)Params->Data;
  char* Text=NULL;
  int TextLength=0;
  
  D(fprintf(stderr, "adding marked files - have %d\n", SelectedFiles->Count()));
  //expand filenames
  char* ifs=getenv("IFS");
  D(fprintf(stderr, "using IFS=\"%s\"\n", ifs));
  for(int i=0; i<SelectedFiles->Count(); i++)
  {
    int FileNameLength;
    char* File=cCommandParser::EscapeShellArgument((char*)SelectedFiles->Get(i)->GetObject(), &FileNameLength);
    D(fprintf(stderr, "adding file %s\n", File));

    FileNameLength++; /*\0 or IFS[0] */
    Text=(char*)realloc(Text, TextLength + FileNameLength);
    sprintf(Text + TextLength - (TextLength > 0 ? 1 : 0), "%.1s%s", i > 0 ? (ifs ? ifs : " ") : "", File);
    TextLength+=FileNameLength;
    free(File);
  }
  D(fprintf(stderr, "marked files: %s (length: %d)\n", Text, TextLength));

  return Text;
}

int cConfigCommandThread::Execute()
{
  D(fprintf(stderr, "parsing %s\n", Command->GetCommand()));

  cCommandParser CommandParser(Command->GetCommand());
  CommandParser.AddReplacement('f', CurrentFile);
  CommandParser.AddReplacement('d', DestinationFile);
  CommandParser.AddReplacement('D', DestinationFile);
  CommandParser.AddHandler('m', &cConfigCommandThread::SelectedFilesHandler, SelectedFiles);
  CommandParser.AddHandler('M', &cConfigCommandThread::SelectedFilesHandler, SelectedFiles);

  cString Text=CommandParser.Parse();

  D(fprintf(stderr, "executing %s\n", *Text));
  return execl("/bin/sh", "sh", "-c", *Text, NULL);
}

/*
 * getlength - a small tool to get the length of a VDR recording by the size
 * of the file index.vdr
 *
 * (c) by Martin Prochnow
 * Distributed under the terms of the GPL v2.0 (see COPYING at the root dir of
 * this archive).
 *
 * Compile with: gcc getlength.c -o getlength
 *
 * Usage: 'getlength' if index.vdr is in the current working directory or
          'getlength /PATH/TO/RECDIR/' else
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc,char **argv)
{
 char *filename;
 int hasindex,length;
 struct stat status;
 FILE *out;

 switch(argc)
 {
  case 1: asprintf(&filename,"./index.vdr");break;
  case 2: asprintf(&filename,"%s/index.vdr",argv[1]);break;
  default: fprintf(stderr,"Usage:\ngetlength [PATH/TO/index.vdr]\n");exit(-1);
 }

 hasindex=!access(filename,R_OK);
 if(hasindex)
 {
  stat(filename,&status);
  length=status.st_size/12000; // calculate length if the recording by the size of index.vdr
  
  free(filename);
  switch(argc)
  {
   case 1: asprintf(&filename,"./length.vdr");break;
   case 2: asprintf(&filename,"%s/length.vdr",argv[1]);break;
  }
  if((out=fopen(filename,"w"))!=NULL)
  {
   fprintf(out,"%d\n",length);
   fclose(out);
  }
  else
  {
   perror("Error while open length.vdr");
   free(filename);
   exit(-1);
  }
 }
 else
 {
  perror("Error while accessing index.vdr");
  free(filename);
  exit(-1);
 }

 free(filename);
 return 0;
}

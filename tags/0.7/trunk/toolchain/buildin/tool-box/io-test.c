/***************************************************************************
 *                                                                         *
 *	Copyright (c) 2007 by Dirk Clemens <develop@cle-mens.de>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#define _GNU_SOURCE 1

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

///////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG
  #define DEBUG 0
#elif DEBUG
  #undef DEBUG
  #define DEBUG 1
#else
  #undef DEBUG
  #define DEBUG 0
#endif

#if DEBUG
  #define TRACE(a) fprintf a
  #define ASSERT(a) if (!(a)) fprintf(stderr,"!!! ASSERTION FAILED IN LINE %u !!!\n",__LINE__), exit(EXIT_FAILURE)
#else
  #define TRACE(a)
  #define ASSERT(a)
#endif

///////////////////////////////////////////////////////////////////////////////

#define NAME "io-test"
#define TITLE NAME " V0.01 - Dirk Clemens - 2007-02-15"

#define MIN_SIZE_MIB    1
#define DEF_SIZE_MIB  100
#define MAX_SIZE_MIB 2047

///////////////////////////////////////////////////////////////////////////////

typedef const char * ccp;

int opt_verbose	= 0;
int opt_direct	= 0;
int opt_force	= 0;
int filesize	= DEF_SIZE_MIB;

///////////////////////////////////////////////////////////////////////////////

static char help_text[] =
	"\n"
	TITLE "\n"
	"\n"
	" Usage: " NAME " [options] file_or_directory\n"
	"\n"
	" Options:\n"
	"   -h --help      : print this help and exit\n"
	"   -v --verbose   : print more statistic infos to stderr\n"
	"   -s --size  mib : test file size in MiB (1024*1024 bytes)\n"
	"                    default=%d MiB, range=%d..%d MiB\n"
	"   -d --direct    : use opt_direct io\n"
	"   -f --force     : force overwrite an existing file\n"
	"\n";

void help_exit()
{
    fprintf(stderr,help_text,DEF_SIZE_MIB,MIN_SIZE_MIB,MAX_SIZE_MIB);
    exit(EXIT_FAILURE);
}

///////////////////////////////////////////////////////////////////////////////

void hint_exit ( ccp argv0 )
{
    fprintf(stderr,"%s: use option --help for more help\n",argv0);
    exit(EXIT_FAILURE);
}

///////////////////////////////////////////////////////////////////////////////

unsigned gettimer_usec()
{
    struct timeval tval;
    gettimeofday(&tval,NULL);

    static time_t timebase = 0;
    if (!timebase)
	timebase = tval.tv_sec;

    return ( tval.tv_sec - timebase ) * 1000000 + tval.tv_usec;
}

///////////////////////////////////////////////////////////////////////////////

void calc_stat ( char * dest, int mib, unsigned msec )
{
    double rate = ( (double)mib * 1000000.0 ) /(double)msec;
    if ( rate >= 100.0 )
	sprintf(dest,"%4.2f GiB/s",rate/1024.0);
    else if ( rate >= 0.1 )
	sprintf(dest,"%4.2f MiB/s",rate);
    else
	sprintf(dest,"%4.2f KiB/s",rate*1024.0);
}

///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char ** argv )
{
    static char short_opt[] = "hvs:df";
    static struct option long_opt[] =
    {
	{ "help",	0, 0, 'h' },
	{ "verbose",	0, 0, 'v' },
	{ "size",	1, 0, 's' },
	{ "direct",	0, 0, 'd' },
	{ "force",	0, 0, 'f' },
	{0,0,0,0}
    };

    if ( argc < 2 )
	help_exit();

    for(;;)
    {
	const int opt_stat = getopt_long(argc,argv,short_opt,long_opt,0);
	if ( opt_stat == -1 )
	    break;
	switch (opt_stat)
	{
	  case '?':
	    hint_exit(argv[0]);
	    
	  case 'h':
	    help_exit();

	  case 'v':
	    opt_verbose++;
	    break;

	  case 's':
	    {
		char * end;
		filesize = strtoul(optarg,&end,10);
		if ( optarg == end
		    || *end
		    || filesize < MIN_SIZE_MIB
		    || filesize > MAX_SIZE_MIB )
		{
		    fprintf(stderr,"%s: illegal number for option --size: '%s'\n",argv[0],optarg);
		    hint_exit(argv[0]);
		}
	    }
	    break;
	    
	  case 'd':
	    opt_direct = 1;
	    break;

	  case 'f':
	    opt_force = 1;
	    break;

	  default:
	    fprintf(stderr,"%s: internal error: unhandled option: '%c'\n",argv[0],opt_stat);
	    exit(1);
	    break;
	}
    }

    if ( optind == argc )
    {
	fprintf(stderr,"%s: missing filename or directory.\n",argv[0]);
	hint_exit(argv[0]);
    }

    ccp filename = argv[optind];
    if ( optind + 1 != argc || !*filename )
    {
	fprintf(stderr,"%s: exact 1 parameter (filename or directory) expected.\n",argv[0]);
	hint_exit(argv[0]);
    }

    char filename_buf[2048];
    struct stat st;
    if (!stat(filename,&st))
    {
	if (S_ISDIR(st.st_mode))
	{
	    snprintf(filename_buf,sizeof(filename_buf),"%s/tmp.io-test.%d.XXXXXX",filename,getpid());
	    filename = mktemp(filename_buf);
	}
	else if (!S_ISREG(st.st_mode))
	{
	    fprintf(stderr,"%s: not a directory or a regular file: %s\n",argv[0],filename);
	    exit(EXIT_FAILURE);
	}
	else if (!opt_force)
	{
	    fprintf(stderr,"%s: file exists, use --force to overwrite: %s\n",argv[0],filename);
	    exit(EXIT_FAILURE);
	}
    }

    int o_direct = opt_direct ? O_DIRECT : 0;

    if (opt_verbose)
	fprintf(stderr,
	    "Test File: %s\n"
	    "Test Size: %d MiB\n"
	    "O_DIRECT:  %d\n"
	    ,filename, filesize, opt_direct );


    //---------- setup buf

    if (opt_verbose)
	fprintf(stderr," - setup write buf...\n");

    const int align   =  64*1024;
    const int bufsize = 512*1024;
    const int cycles_per_mib = 1024*1024/bufsize;

    void * buf;
    if (posix_memalign(&buf,align,bufsize))
    {
	perror(argv[0]);
	fprintf(stderr,"%s: can't allocate aligned memory\n",argv[0]);
	exit(EXIT_FAILURE);
    }

    memset(buf,0,bufsize);
    strncpy(buf,help_text,bufsize);
    {
	char * src  = buf;
	char * dest = buf + 101;
	char * end  = buf + bufsize;
	int i = 0;
	for ( i=0; dest < end; i++ )
	    *dest++ ^= *src++ ^ i;
    }

    //---------- write test

    int fd = open(filename,O_WRONLY|O_CREAT|o_direct,0600);
    if ( fd < 0 )
    {
	perror(argv[0]);
	fprintf(stderr,"%s: can't create file: %s\n",argv[0],filename);
	exit(EXIT_FAILURE);
    }

    int i;
    if (opt_verbose)
	fprintf(stderr," - start writing...\n");
    unsigned start_time = gettimer_usec();
    for ( i = 0; i < filesize*cycles_per_mib; i++ )
    {
	const int stat = write(fd,buf,bufsize);
	if ( stat != bufsize )
	{
	    perror(argv[0]);
	    fprintf(stderr,"%s: error (stat=%d) while writing to file: %s\n",argv[0],stat,filename);
	    close(fd);
	    unlink(filename);
	    exit(EXIT_FAILURE);
	}
    }
    close(fd);
    unsigned write_time = gettimer_usec() - start_time;

    char wstat[100];
    calc_stat(wstat,filesize,write_time);
    if (opt_verbose)
	fprintf(stderr," -> %d MiB written in %4.2f seconds => %s\n",filesize,write_time/1000000.0,wstat);

    //---------- read test

    fd = open(filename,O_RDONLY|o_direct);
    if ( fd < 0 )
    {
	perror(argv[0]);
	fprintf(stderr,"%s: can't open file: %s\n",argv[0],filename);
	exit(EXIT_FAILURE);
    }

    if (opt_verbose)
	fprintf(stderr," - start reading...\n");
    start_time = gettimer_usec();
    for ( i = 0; i < filesize*cycles_per_mib; i++ )
    {
	const int stat = read(fd,buf,bufsize);
	if ( stat != bufsize )
	{
	    perror(argv[0]);
	    fprintf(stderr,"%s: error (stat=%d) while reading from file: %s\n",argv[0],stat,filename);
	    close(fd);
	    unlink(filename);
	    exit(EXIT_FAILURE);
	}
    }
    close(fd);
    unsigned read_time = gettimer_usec() - start_time;

    char rstat[100];
    calc_stat(rstat,filesize,read_time);
    if (opt_verbose)
	fprintf(stderr," -> %d MiB read    in %4.2f seconds => %s\n",filesize,read_time/1000000.0,rstat);
    
    //---------- stat & term

    unlink(filename);
    printf("STAT_WRITE=\"%s\"\nSTAT_READ=\"%s\"\n",wstat,rstat);

    return EXIT_SUCCESS;
}

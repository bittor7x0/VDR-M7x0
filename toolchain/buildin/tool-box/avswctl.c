/*
 * avswctl.c
 * M740AV A/V-Switch control utility
 *
 * Sendet ioctl Kommandos an /dev/avswitch.
 * Dirk Clemens, m740av (at) cle-mens (dot) de, GPL2
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#define NAME	"avswctl"
#define VERSION	"2.01"
#define DATE	"2006-09-11"
#define AUTHOR	"Dirk Clemens"

#define DEVICE	"/dev/avswitch"

//-----------------------------------------------------------------------------

typedef const char * ccp;

struct io_codes_t
{
	int code;
	ccp name;
	ccp info;
};

struct io_options_t
{
	ccp name;
	const struct io_codes_t * io_tab;
};

//-----------------------------------------------------------------------------

const struct io_codes_t io_codes[] =
{
	{ 0x80, "tv-fbas",	"set tv to mode 'fbas'" },
	{ 0x81, "tv-svideo",	"set tv to mode 'svideo'" },
	{ 0x82, "tv-vcr",	"enable vcr-tv passthrough (channel 0)" },
	{ 0x84, "tv-off",	"together with 'tv-vcr': SCART signal 'tv off'" },

	{ 0x85, "vcr-fbas",	"set vcr to mode 'fbas'" },
	{ 0x86, "vcr-svideo",	"set vcr to mode 'svideo'" },

	{ 0x90, "4:3",		"set tv aspect ratio to 4:3" },
	{ 0x89, "16:9",		"set tv aspect ratio to 16:9" },
	{ 0x88, "off",		"set tv off" },

	{0,0,0}
};

const struct io_codes_t tv_io_codes[] =
{
	{ 0x80, "fbas",		0 },
	{ 0x81, "svideo",	0 },
	{ 0x82, "vcr",		0 },
	{0,0,0}
};

const struct io_codes_t vcr_io_codes[] =
{
	{ 0x85, "fbas",		0 },
	{ 0x86, "svideo",	0 },
	{0,0,0}
};

const struct io_codes_t mode_io_codes[] =
{
	{ 0x90, "4:3",		0 },
	{ 0x89, "16:9",		0 },
	{0,0,0}
};

const struct io_options_t io_options[] =
{
	{ "tv",	  tv_io_codes },
	{ "vcr",  vcr_io_codes },
	{ "mode", mode_io_codes },
	{0,0}
};

#define MAX_CODES 20
int codes[MAX_CODES];
int n_codes = 0;

int test_mode = 0;
int debug_mode = 0;

const char pre_msg[]  = NAME ": ";
const char post_err[] = "  ==> try " NAME " -help\n";

//-----------------------------------------------------------------------------
//

void help_exit()
{
    const struct io_options_t * opt;
    const struct io_codes_t * io;

    printf(
	"\n"
	NAME " V" VERSION " - " AUTHOR " - " DATE "\n"
	"\n"
	"usage: " NAME " [options]... [mode]...\n"
	"\n"
	"   Options:\n");

    for ( opt = io_options; opt->name; opt++ )
    {
	printf("\t-%-4s",opt->name);
	char sep = ' ';
	for ( io = opt->io_tab; io->name; io++ )
	{
	    printf("%c%s",sep,io->name);
	    sep = '|';
	}
	putchar('\n');
    }

    printf(
	"\n"
	"\t-t -test    test mode, don't ioctl anything\n"
	"\t-d -debug   print debug infos on stderr\n"
	"\t-h -help    print this help info\n"
	"\n"
	"   Modes:\n");
    for ( io = io_codes; io->name; io++ )
	printf("\t%-12s0x%02x  %s\n",io->name,io->code,io->info);
    printf("\t%-18sany mode (c like numbers)\n\n","<number>");
	
    exit(1);
}

//-----------------------------------------------------------------------------

void add_mode ( const struct io_codes_t * io, ccp mode, int allow_num )
{
    if (!io)
	exit(1);

    if ( n_codes >= MAX_CODES )
    {
	fprintf(stderr,"%sto many codes, max=%d%s",pre_msg,MAX_CODES,post_err);
	exit(1);
    }

    if ( allow_num && *mode >= '0' && *mode <= '9' )
    {
	char * end;
	const int code = strtol(mode,&end,0);
	if ( !*end )
	{
	    codes[n_codes++] = code;
	    return;
	}
    }
	
    for (; io->name; io++ )
	if (!strcasecmp(mode,io->name))
	{
	    codes[n_codes++] = io->code;
	    return;
	}

    fprintf(stderr,"%sunknown mode: %s%s",pre_msg,mode,post_err);
    exit(1);
}

//-----------------------------------------------------------------------------
//

int main( int argc, char** argv )
{
    int a;
    for ( a = 1; a < argc; a++ )
    {
	ccp arg = argv[a];
	if ( *arg == '-' )
	{
	    arg++;
	    if ( *arg == '-' )
		arg++;
	    if ( !strcasecmp(arg,"t") || !strcasecmp(arg,"test") )
		test_mode = 1;
	    else if ( !strcasecmp(arg,"d") || !strcasecmp(arg,"debug") )
		debug_mode = 1;
	    else if ( !strcasecmp(arg,"h") || !strcasecmp(arg,"help") )
		help_exit();
	    else
	    {
		const struct io_options_t * opt;
		for ( opt = io_options; opt->name; opt++ )
		    if (!strcasecmp(arg,opt->name))
			break;
		if (!opt->name)
		{
		    fprintf(stderr,"%sunknown option: -%s%s",pre_msg,arg,post_err);
		    exit(1);
		}
		a++;
		if ( a >= argc )
		{
		    fprintf(stderr,"%smising parameter for option -%s%s",pre_msg,arg,post_err);
		    exit(1);
		}
		add_mode(opt->io_tab,argv[a],0);
	    }
	}
	else
	    add_mode(io_codes,arg,1);
    }

    if (!n_codes)
	help_exit();

    if (test_mode)
    {
	printf("%swould send %d codes:\n\t",pre_msg,n_codes);
	int c;
	for ( c = 0; c < n_codes; c++ )
	    printf(" 0x%02x",codes[c]);
	putchar('\n');
    }
    else
    {
	const int dev = open(DEVICE,O_RDWR);
	if ( dev == -1 )
	{
	    perror(DEVICE);
	    return 1;
	}

	int c;
	for ( c = 0; c < n_codes; c++ )
	{
	    const int status = ioctl(dev,codes[c],0);
	    if (debug_mode)
		fprintf(stderr,"ioctl(%d,0x%02x,0) -> %d\n",dev,codes[c],status);
	}
	close(dev);
    }

    return 0;
}

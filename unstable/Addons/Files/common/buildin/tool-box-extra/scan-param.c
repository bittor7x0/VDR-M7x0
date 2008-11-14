/***************************************************************************
 *                                                                         *
 *      Copyright (c) 2005-2007 by Dirk Clemens <develop@cle-mens.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define NAME  "scan-param"
#define VERSION "1.09"
#define TITLE NAME " V" VERSION ", Dirk Clemens, 2007-04-01"

#define LINEBUFSIZE 2000

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

#define noTRACE(a)
#define noASSERT(a)

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

enum bool { false, true };
enum { TYPE_NONE, TYPE_KEY, TYPE_LIST, TYPE_NUM, TYPE_STR };

typedef struct t_keyword
{
    struct t_keyword * next;
    char * name;
    struct t_keyword * real_key;
    int all;
    int flag;
} t_keyword;

typedef struct t_param
{
    struct t_param * next;
    char * name;
    char * str;
    int type, done;
    ulong min, max, def;
    t_keyword * key;
} t_param;

t_param * param = 0;
t_param ** last_param = &param;

const char * prefix = "";

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void *good_malloc ( size_t size )
{
    void * ptr = malloc(size);
    if (!ptr)
    {
	fprintf(stderr,"\nFATAL ERROR: out of memory: malloc(%u)\n",size);
	exit(EXIT_FAILURE);
    }
    ASSERT(ptr);
    return ptr;
}

//////////////////////////////////////////////////////////////////////////////

char * good_strdup ( const char * str )
{
    char * ptr = strdup(str);
    if (!ptr)
    {
	fprintf(stderr,"\nFATAL ERROR: out of memory; strdup()\n");
	exit(EXIT_FAILURE);
    }
    ASSERT(ptr);
    return ptr;
}
    
//////////////////////////////////////////////////////////////////////////////

t_param * new_param ( const char * name )
{
    t_param * par = good_malloc(sizeof(t_param));
    ASSERT(par);
    memset(par,0,sizeof(t_param));
    par->name = good_strdup(name);
    ASSERT(par->name);
    ASSERT(last_param);
    *last_param = par;
    last_param = &par->next;
    return par;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

char scan_str_sep ( char ** p_src, enum bool eat_spaces )
{
    char * src = *p_src;
    while ( *src == ' ' || *src == '\t' )
	src++;
    char result = 0;
    if ( *src == '\'' || *src == '"' )
    {
	result = *src++;
	if (eat_spaces)
	    while ( *src == ' ' || *src == '\t' )
		src++;
    }
    *p_src = src;
    return result;
}

//////////////////////////////////////////////////////////////////////////////

char * scan_word ( char ** p_src, char * sep, enum bool allow_minus )
{
    char * src = *p_src;
    noTRACE((stderr,"scan_word(%s)\n",src));
    while ( *src == ' ' || *src == '\t' )
	src++;

    char * start = src;
    while (   *src >= '0' && *src <= '9'
	   || *src >= 'a' && *src <= 'z'
	   || *src >= 'A' && *src <= 'Z'
	   || *src == ':'
	   || *src == '_'
	   || *src == '-' && allow_minus )
		src++;

    if ( start == src )
	return 0;

    if (sep)
	*sep = src[0] == '=' && src[1] == '"' ? 'B' : *src;
	
    if (*src)
	*src++ = 0;

    while ( *src == ' ' || *src == '\t' )
	src++;

    *p_src = src;
    return start;
}

//////////////////////////////////////////////////////////////////////////////

int scan_num ( ulong * num, char ** p_src, char sep )
{
    ASSERT(num);

    char *src = *p_src;
    if ( !src || !*src )
	return 1;

    *num = strtoul(src,&src,10);
    if ( src == *p_src )
	return 1;

    if ( *src == 'K' || *src == 'k' )
	*num *= 1024;
    else if ( *src == 'M' || *src == 'm' )
	*num *= 1024*1024;
    else if ( *src == 'G' || *src == 'g' )
	*num *= 1024*1024*1024;
    else src--;
    src++;

    if ( *(unsigned char*)src > ' ' && *src != sep )
	return 1;

    *p_src = src;
    return 0;
}

//////////////////////////////////////////////////////////////////////////////

char * scan_str ( char * src, int bash_like )
{
    ASSERT(src);

    if ( bash_like && *src == '"' )
	src++;
    else
    {
	bash_like = 0;
	while ( *src == ' ' || *src == '\t' )
	    src++;
    }

    char *end, *dest;
    for ( end = dest = src; *end; end++ )
    {
	if ( *(unsigned char*)end <= ' '
		|| *end == '|'
		|| *end == '&'
		|| *end == ';'
		|| *end == '>'
		|| *end == '<'
		|| *end == '('
		|| *end == '`'
	   )
	{
	    *dest++ = ' ';
	}
	else if ( bash_like && *end == '\\' && end[1] )
	    *dest++ = *++end;
	else
	    *dest++ = *end;
    }

    while ( dest > src && dest[-1] == ' ' )
	dest--;
    if ( bash_like && dest > src && dest[-1] == '"' )
	dest--;

    *dest = 0;
    return src;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void scan_config_key ( char * name, char * src, int type )
{
    TRACE((stderr,"scan_config_key(\"%s\",,%d)\n",name,type));

    t_param * par = new_param(name);
    par->type = type;

    t_keyword *  real_key = 0;
    t_keyword ** last_key = &par->key;
    for(;;)
    {
	while ( *src == ' ' || *src == '\t' )
	    src++;

	int all = 0;
	const int equal_found = *src == '=';
	if (equal_found)
	    src++;
	else if ( type == TYPE_LIST )
	    while ( *src == '*' )
		all++, src++;

	char * key_name = scan_word(&src,0,true);
	if (!key_name)
	    break;

	t_keyword * key = good_malloc(sizeof(t_keyword));
	ASSERT(key);
	memset(key,0,sizeof(t_keyword));

	key->name = good_strdup(key_name);
	ASSERT(key->name);
	if ( !real_key || !equal_found )
	    real_key = key;
	key->real_key = real_key;
	key->all  = all;
	key->flag = 0;

	*last_key = key;
	last_key = &key->next;
    }
}

//////////////////////////////////////////////////////////////////////////////

void scan_config_num ( char * name, char * src )
{
    TRACE((stderr,"scan_config_num(\"%s\",)\n",name));
    ulong def, min, max;
    if ( scan_num(&def,&src,0) || scan_num(&min,&src,0) || scan_num(&max,&src,0) )
	return;
    TRACE((stderr," - min=%lu, max=%lu, def=%lu\n",min,max,def));

    t_param * par = new_param(name);
    par->type = TYPE_NUM;
    par->min = min;
    par->max = max;
    par->def = def;
}

//////////////////////////////////////////////////////////////////////////////

void scan_config_str ( char * name, char * src )
{
    TRACE((stderr,"scan_config_str(\"%s\",)\n",name));
    t_param * par = new_param(name);
    par->type = TYPE_STR;
    par->str = good_strdup(scan_str(src,0));
}

//////////////////////////////////////////////////////////////////////////////

int scan_config ( char * filename )
{
    if ( !filename || !*filename )
	return 1;

    FILE * fd = fopen(filename,"r");
    if (!fd)
	return 1;

    char buf[200];
    while (fgets(buf,sizeof(buf),fd))
    {
	char * src = buf;
	char * name = scan_word(&src,0,false);
	if (!name)
	    continue;

	char * type = scan_word(&src,0,false);
	if (!type)
	    continue;

	if (!strcasecmp(type,"key"))
	    scan_config_key(name,src,TYPE_KEY);
	else if (!strcasecmp(type,"list"))
	    scan_config_key(name,src,TYPE_LIST);
	else if (!strcasecmp(type,"num"))
	    scan_config_num(name,src);
	else if (!strcasecmp(type,"str"))
	    scan_config_str(name,src);
    }
    
    fclose(fd);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void check_key ( t_param * par, char * src )
{
    noTRACE((stderr,"check_key(%s)\n",par->name));
    scan_str_sep(&src,1);
    char * key_name = scan_word(&src,0,true);
    if (!key_name)
	return;

    t_keyword * key;
    for ( key = par->key; key; key = key->next )
	if (!strcasecmp(key_name,key->name))
	{
	    printf("%s%s=%s\n",prefix,par->name,key->real_key->name);
	    par->done++;
	    return;
	}
}

//////////////////////////////////////////////////////////////////////////////

void check_list ( t_param * par, char * src )
{
    int done = 0;
    TRACE((stderr,"check_list(%s)\n",par->name));
    scan_str_sep(&src,1);
    for (;;)
    {
	while ( *src == ' ' || *src == '\t' || *src == ',' )
	    src++;

	char * key_name = scan_word(&src,0,true);
	if (!key_name)
	    break;

	t_keyword * key;
	for ( key = par->key; key; key = key->next )
	    if (!strcasecmp(key_name,key->name))
	    {
		TRACE((stderr,"-> %s -> %s\n",key->name,key->real_key->name));
		key->real_key->flag = 3;
		done = 1;
		if ( key->all )
		{
		    int realy_all = key->all > 1;
		    while ( key->next )
		    {
			key = key->next;
			if ( !realy_all && key->all )
			    break;
			key->real_key->flag |= 1;
		    }
		}
		break;
	    }
    }

    if (!done)
	return;
    par->done++;
    
    printf("%s%s=",prefix,par->name);
    char * sep = "";
    t_keyword * key;
    for ( key = par->key; key; key = key->next )
	if ( key->flag & 2 )
	{
	    printf("%s%s",sep,key->name);
	    sep = ",";
	}
    putchar('\n');    

    for ( key = par->key; key; key = key->next )
    {
	if ( !key->all && key->real_key == key )
	    printf("%s%s_%s=%d\n",prefix,par->name,key->name,key->flag&1);
	key->flag = 0;
    }
}

//////////////////////////////////////////////////////////////////////////////

void check_num ( t_param * par, char * src )
{
    TRACE((stderr,"check_num(%s) -> '%s'\n",par->name,src));
    const char sep = scan_str_sep(&src,1);
    TRACE((stderr,"check_num(%s) -> '%s'\n",par->name,src));
    ulong num;
    if (!scan_num(&num,&src,sep))
    {
	if ( num < par->min )
	    num = par->min;
	else if ( num > par->max )
	    num = par->max;
	printf("%s%s=%lu\n",prefix,par->name,num);
	par->done++;
    }
}

//////////////////////////////////////////////////////////////////////////////

void check_str ( t_param * par, char * src, int bash_like )
{
    noTRACE((stderr,"check_str(%s)\n",par->name));

    src = scan_str(src,bash_like);

    char buf[2*LINEBUFSIZE];
    char *dest = buf, *dest_end = buf + sizeof(buf) - 2;
    while ( *src && dest < dest_end )
    {
	if ( *src == '\\' || *src == '"' || *src == '$' || *src == '`' )
	    *dest++ = '\\';
	*dest++ = *src++;
    }
    ASSERT( dest < buf + sizeof(buf)-1 );
    *dest = 0;

    printf("%s%s=\"%s\"\n",prefix,par->name,buf);
    par->done++;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char ** argv )
{
    char * filename = 0;
    int print_dump = 0, print_def = 0, export_prefix = 0, print_comment = 0;
    int err = 0, n_param = 0;
    int a;
    for ( a = 1; a < argc; a++ )
    {
	if ( *argv[a] == '-' )
	{
	    if (!strcmp(argv[a],"-d"))
		print_dump = 1;
	    else if (!strcmp(argv[a],"-e"))
		export_prefix = 1;
	    else if (!strcmp(argv[a],"-u"))
		print_def = 1;
	    else if (!strcmp(argv[a],"-c"))
		print_comment = 1;
	    else
		err++;
	}
	else
	{
	    filename = argv[a];
	    n_param++;
	}
    }

    if ( err || n_param != 1 || scan_config(filename) )
    {
	fprintf(stderr,
		"\n"
		TITLE "\n"
		"\n"
		"usage: %s [options]... config_file\n"
		"\n"
		"   Options:\n"
		"\t-e	use 'export ' as prefix.\n"
		"\t-u	print unsetted vars with default values.\n"
		"\t-c	comment output.\n"
		"\t-d	print config dump stderr.\n"
		"\n"
		,argv[0]);
	
	return EXIT_FAILURE;
    }

    if (export_prefix)
	prefix = "export ";

    if (print_dump)
    {
	fprintf(stderr,"---------- DUMP param ----------\n");
	t_param * par;
	for ( par = param; par; par = par->next )
	{
	    switch(par->type)
	    {
	      case TYPE_KEY:
	      case TYPE_LIST:
		{
		    fprintf(stderr,"%s: %s\n",
				par->type == TYPE_KEY ? "KEY"  : "LIST",
				par->name);
		    t_keyword * key;
		    for ( key = par->key; key; key = key->next )
			if ( key->all )
			    fprintf(stderr,"  key: %d* %s\n",key->all,key->name);
			else if ( key->name == key->real_key->name )
			    fprintf(stderr,"  key: %s\n",key->name);
			else
			    fprintf(stderr,"  key: %s -> %s\n",key->name,key->real_key->name);
		}
		break;

	      case TYPE_NUM:
		fprintf(stderr,"NUM: %s\n",par->name);
		fprintf(stderr,"  min=%lu, max=%lu, def=%lu\n",par->min,par->max,par->def);
		break;

	      case TYPE_STR:
		fprintf(stderr,"STR: %s\n",par->name);
		break;

	      default:
		fprintf(stderr,"type=%d ???: %s\n",par->type,par->name);
		break;
	    }
	}
	fprintf(stderr,"--------------------------------\n");
    }

    if (print_comment)
	printf("# " TITLE "\n");

    char buf[LINEBUFSIZE];
    while (fgets(buf,sizeof(buf),stdin))
    {
	char sep, *src = buf;
	char *name = scan_word(&src,&sep,false);
	int bash_like = sep == 'B';
	if ( sep == ' ' || sep == '\t' )
	    sep = *src++;
	if ( !name || sep != '=' && sep != 'B' )
	    continue;
	t_param * par = param;
	while ( par && strcmp(par->name,name) )
	    par = par->next;

	if (!par)
	    continue;

	switch(par->type)
	{
	    case TYPE_KEY:
		check_key(par,src);
		break;

	    case TYPE_LIST:
		check_list(par,src);
		break;

	    case TYPE_NUM:
		check_num(par,src);
		break;

	    case TYPE_STR:
		check_str(par,src,bash_like);
		break;
	}
    }

    if (print_def)
    {
    	if (print_comment)
	    printf("# defaults\n");
	t_keyword * key;
	t_param * par;
	for ( par = param; par; par = par->next )
	{
	  if (!par->done)
	    switch(par->type)
	    {
	      case TYPE_KEY:
		if (par->key)
		    printf("%s%s=%s\n",prefix,par->name,par->key->name);
		else
		    printf("%s%s=\n",prefix,par->name);
		break;

	      case TYPE_LIST:
		printf("%s%s=\n",prefix,par->name);
		for ( key = par->key; key; key = key->next )
		    if ( !key->all && key->real_key == key )
			printf("%s%s_%s=0\n",prefix,par->name,key->name);
		break;

	      case TYPE_NUM:
		printf("%s%s=%lu\n",prefix,par->name,par->def);
		break;

	      case TYPE_STR:
		check_str(par,par->str,0);
		break;
	    }
	}
    }
    return EXIT_SUCCESS;
}


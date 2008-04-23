/*
 * news.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: news.h 0.0.1 2003/11/22 ew $
 */
 
#ifndef __NEWS_H
#define __NEWS_H

#include <stdlib.h>
#include <string.h>

#include <vdr/plugin.h>

// Socket Headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream.h>
#include <fstream.h>

#define INT_SIZEOFBUF 300 * 1024 // Buffer 200Kb 

class cNews{
private:
	char *buffer;
	char *scrolltext;
	char *descriptiontext;
	
	char linebuffer[256];
	
	int buffersize;
	
	int spacewidth, charwidth, ascender, descender, fontwidth, fontheight;
	
	int descriptionfound;
public:
  cNews(void);
  ~cNews();
  bool downloadRDF(char* url, const char *plugin_Name, char *option_savePath);
  char* getDescriptiontext();
  char* getScrolltext();
  char* getBuffer();
  };

#endif

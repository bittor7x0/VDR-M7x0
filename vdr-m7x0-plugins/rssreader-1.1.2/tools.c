/*
 * tools.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "compatibility.h"

#include "tools.h"
#include "common.h"
#include <stdlib.h>
#include <string>

using namespace std;

// --- Static -----------------------------------------------------------

#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))



struct conv_table {
  const char *from;
  const char *to;
};


static struct conv_table post_conv_table[] =
{
   // 'to' field must be smaller than 'from'
  {"&amp;",    "&"},
  {"&#039;",   "'"},
  {"&#160;",   " "},
  {"&#171;",   "\xc2\xab"},
  {"&#176;",   "\xc2\xb0"},
  {"&#180;",   "'"},
  {"&#187;",   "\xc2\xbb"},
  {"&#191;",   "\xc2\xbf"},  
  {"&#193;",   "\xc3\x81"},
  {"&#196;",   "\xc3\x84"},
  {"&#197;",   "\xc3\x85"},
  {"&#201;",   "\xc3\x89"},
  {"&#205;",   "\xc3\x8d"},
  {"&#209;",   "\xc3\x91"},  
  {"&#211;",   "\xc3\x93"},
  {"&#214;",   "\xc3\x96"},
  {"&#218;",   "\xc3\x9a"},
  {"&#220;",   "\xc3\x9c"},
  {"&#223;",   "\xc3\x9f"},  
  {"&#225;",   "\xc3\xa1"},
  {"&#226;",   "\xc3\xa2"},
  {"&#228;",   "\xc3\xa4"},
  {"&#229;",   "\xc3\xa5"},
  {"&#231;",   "\xc3\xa7"},  
  {"&#233;",   "\xc3\xa9"},
  {"&#234;",   "\xc3\xaa"},
  {"&#237;",   "\xc3\xad"},
  {"&#241;",   "\xc3\xb1"},
  {"&#243;",   "\xc3\xb3"},
  {"&#246;",   "\xc3\xb6"},
  {"&#250;",   "\xc3\xba"},
  {"&#252;",   "\xc3\xbc"},
  {"&#38;",    "&"},
  {"&#40;",    "'"},
  {"&#41;",    "'"},
  {"&#58;",    ":"},
  {"&#60;",    "<"},
  {"&#62;",    ">"},
  {"&#8211;",  "-"},
  {"&#8216;",  "'"},
  {"&#8217;",  "'"},
  {"&#8220;",  "\""},
  {"&#8221;",  "\""},
  {"&#8222;",  "\""},
  {"&#8364;",  "\xe2\x82\xac"},
  {"&#91;",    "["},
  {"&#93;",    "]"},  
  {"&aacute;", "\xc3\xa1"},
  {"&Aacute;", "\xc3\xa1"},
  {"&acirc;",  "\xc3\xa2"},
  {"&acute;",  "'"},  
  {"&Auml;",   "\xc3\x84"},
  {"&auml;",   "\xc3\xa4"},
  {"<br />",   "\n"},
  {"<br/>",    "\n"},
  {"&ccedil;", "\xc3\xa7"},  
  {"&Ccedil;", "\xc3\xa7"},  
  {"&eacute;", "\xc3\xa9"},
  {"&Eacute;", "\xc3\x89"},
  {"&euro;",   "\xe2\x82\xac"},
  {"&gt;",     ">"},
  {"&iacute;", "\xc3\xad"},
  {"&Iacute;", "\xc3\x8d"},
  {"&lt;",     "<"},
  {"&nbsp;",   " "},
  {"&ndash;",  "-"},
  {"\n\n",     "\n"}, // let's also strip multiple linefeeds
  {"&ntilde;", "\xc3\xb1"},
  {"&Ntilde;", "\xc3\x91"},     
  {"&oacute;", "\xc3\xb3"},
  {"&Oacute;", "\xc3\x93"},  
  {"&Ouml;",   "\xc3\x96"},
  {"&ouml;",   "\xc3\xb6"},
  {"&quot;",   "\""},
  {"&szlig;",  "\xc3\x9f"},
  {"&uacute;", "\xc3\xba"},
  {"&Uacute;", "\xc3\x9a"},
  {"&Uuml;",   "\xc3\x9c"},
  {"&uuml;",   "\xc3\xbc"},
  {NULL,NULL}
};



// --- General functions ------------------------------------------------
 
string& replace_all(string& context, const string& from, const string& to) {
  size_t lookHere = 0;
  size_t foundHere;
  while ((foundHere = context.find(from, lookHere)) != string::npos) {
    context.replace(foundHere, from.size(), to);
    lookHere = foundHere + to.size();
  }
  return context;
}

void striphtml(string &str){  
  int i=0;
  bool inTag = false;
  bool done = false;
  //Convert encoding
  cCharSetConv conv("UTF-8",DEFAULT_ENCONDING);
  while(1) {
    if ( post_conv_table[i].from == NULL && post_conv_table[i].to == NULL ) break;
    replace_all(str,post_conv_table[i].from,conv.Convert(post_conv_table[i].to));
    i++;
  }
  //strip html
  while(!done) {
	if(inTag) {
		// The previous line started an HTML tag
		// but didn't finish. Must search for '>'.
		size_t rightPos = str.find('>');
		if(rightPos != string::npos) {
			inTag = false;
			str.erase(0, rightPos + 1);
		}
		else {
			done = true;
			str.erase();
		}
	}
	else {
		// Look for start of tag:
		size_t leftPos = str.find('<');
		if(leftPos != string::npos) {
			// See if tag close is in this line:
			size_t rightPos = str.find('>');
			if(rightPos == string::npos) {
				inTag = done = true;
				str.erase(leftPos);
			}
			else
				str.erase(leftPos, rightPos - leftPos + 1);
			}		
		else
			done = true;
	}
  }	
  
}


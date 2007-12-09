/*
 * tools.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "tools.h"
#include "common.h"

// --- Static -----------------------------------------------------------

#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

struct conv_table {
  char *from;
  char *to;
};

static struct conv_table pre_conv_table[] =
{
  // 'to' field must be smaller than 'from'
  {"<br />",   "\n"}
};

static struct conv_table post_conv_table[] =
{
  // 'to' field must be smaller than 'from'
  {"&#228;",   "\xc3\xa4"},
  {"&auml;",   "\xc3\xa4"},
  {"&#196;",   "\xc3\x84"},
  {"&Auml;",   "\xc3\x84"},
  {"&#246;",   "\xc3\xb6"},
  {"&ouml;",   "\xc3\xb6"},
  {"&#214;",   "\xc3\x96"},
  {"&Ouml;",   "\xc3\x96"},
  {"&#229;",   "\xc3\xa5"},
  {"&#197;",   "\xc3\x85"},
  {"&#220;",   "\xc3\x9c"},
  {"&Uuml;",   "\xc3\x9c"},
  {"&#252;",   "\xc3\xbc"},
  {"&uuml;",   "\xc3\xbc"},
  {"&#223;",   "\xc3\x9f"},
  {"&szlig;",  "\xc3\x9f"},
  {"&#234;",   "\xc3\xaa"},
  {"&#176;",   "\xc2\xb0"},
  {"&#8211;",  "-"},
  {"&ndash;",  "-"},
  {"&#38;",    "&"},
  {"&amp;",    "&"},
  {"&#58;",    ":"},
  {"&#91;",    "["},
  {"&#93;",    "]"},
  {"&#40;",    "'"},
  {"&#41;",    "'"},
  {"&#039;",   "'"},
  {"&#180;",   "'"},
  {"&acute;",  "'"},
  {"&#8216;",  "'"},
  {"&#8217;",  "'"},
  {"&#231;",   "\xc3\xa7"},
  {"&ccedil;", "\xc3\xa7"},
  {"&#233;",   "\xc3\xa9"},
  {"&eacute;", "\xc3\xa9"},
  {"&#226;",   "\xc3\xa2"},
  {"&acirc;",  "\xc3\xa2"},
  {"&#8364;",  "\xe2\x82\xac"},
  {"&euro;",   "\xe2\x82\xac"},
  {"&quot;",   "\""},
  {"&#8220;",  "\""},
  {"&#8221;",  "\""},
  {"&#8222;",  "\""},
  {"&#160;",   " "},
  {"&nbsp;",   " "},
  {"&lt;",     "<"},
  {"&gt;",     ">"},
  {"\n\n",     "\n"}, // let's also strip multiple linefeeds
};

static char *htmlcharconv(char *str, struct conv_table *conv, unsigned int elem)
{
  if (str && conv) {
     for (unsigned int i = 0; i < elem; ++i) {
        char *ptr = strstr(str, conv[i].from);
        while (ptr) {
           int of = ptr - str;
           int l  = strlen(str);
           int l1 = strlen(conv[i].from);
           int l2 = strlen(conv[i].to);
           if (l2 > l1) {
              error("htmlcharconv(): cannot reallocate string");
              return str;
              }
           if (l2 != l1)
              memmove(str + of + l2, str + of + l1, l - of - l1 + 1);
           strncpy(str + of, conv[i].to, l2);
           ptr = strstr(str, conv[i].from);
           }
        }
     return str;
     }
  return NULL;
}

// --- General functions ------------------------------------------------

char *striphtml(char *str)
{
  if (str) {
     char *c, t = 0, *r;
     str = htmlcharconv(str, pre_conv_table, ELEMENTS(pre_conv_table));
     c = str;
     r = str;
     while (*str != '\0') {
       if (*str == '<')
          t++;
       else if (*str == '>')
          t--;
       else if (t < 1)
          *(c++) = *str;
       str++;
       }
     *c = '\0';
     return htmlcharconv(r, post_conv_table, ELEMENTS(post_conv_table));
     }
  return NULL;
}

void *myrealloc(void *ptr, size_t size)
{
  /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
  if (ptr)
     return realloc(ptr, size);
  else
     return malloc(size);
}

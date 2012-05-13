/*
 * tools.c: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "tools.h"

char *strcatrealloc(char *dest, const char *src)
{
  if (src) {
     int l = strlen(dest) + strlen(src) + 1;
     dest = (char *)realloc(dest, l);
     if (dest)
        strcat(dest, src);
     else
        esyslog("ERROR: out of memory!");
  }
  return dest;
}

char *strncatrealloc(char *dest, const char *src, size_t len)
{
  if (src) {
     size_t l = strlen(dest) + min(strlen(src), len) + 1;
     dest = (char *)realloc(dest, l);
     if (dest)
        strncat(dest, src, min(strlen(src), len));
     else
        esyslog("ERROR: out of memory!");
  }
  return dest;
}

static char *striptags(char *str)
{
  char *r, *c;
  int t = 0, d = 0;
  bool s = false;
  r = str;
  c = str;
  while (*str != '\0') {
    if (*str == '<') {
       t++;
       s = true;
       }
    else if (*str == '>') {
       t--;
       }
    else if (t < 1) {
       *(c++) = *str;
       }
    else if (s) {
       if (*str == '/') {
          d--;
          }
       else {
          d++;
          }
       if (d < 1) {
          *(c++) = '\n';
          }
       else if (d > 1) {
          *(c++) = ' ';
          }
       s = false;
       }
    str++;
    }
  *c = '\0';
  return r;
}

#define AUX_HEADER_EPGSEARCH           "EPGSearch: "
#define AUX_TAGS_EPGSEARCH_START       "<epgsearch>"
#define AUX_TAGS_EPGSEARCH_ITEM1_START "<channel>"
#define AUX_TAGS_EPGSEARCH_ITEM1_END   "</channel>"
#define AUX_TAGS_EPGSEARCH_ITEM2_START "<searchtimer>"
#define AUX_TAGS_EPGSEARCH_ITEM2_END   "</searchtimer>"
#define AUX_TAGS_EPGSEARCH_ITEM3_START "<search timer>"
#define AUX_TAGS_EPGSEARCH_ITEM3_END   "</search timer>"
#define AUX_TAGS_EPGSEARCH_END         "</epgsearch>"

#define AUX_HEADER_VDRADMIN            "VDRAdmin: "
#define AUX_TAGS_VDRADMIN_START        "<vdradmin-am>"
#define AUX_TAGS_VDRADMIN_ITEM1_START  "<pattern>"
#define AUX_TAGS_VDRADMIN_ITEM1_END    "</pattern>"
#define AUX_TAGS_VDRADMIN_END          "</vdradmin-am>"

#define AUX_HEADER_PIN			"Protected: "
#define AUX_TAGS_PIN_START		"<pin-plugin>"
#define AUX_TAGS_PIN_ITEM1_START	"<protected>"
#define AUX_TAGS_PIN_ITEM1_END		"</protected>"
#define AUX_TAGS_PIN_END		"</pin-plugin>"

char *parseaux(char *aux)
{
  char *start, *end;
  // check if egpsearch
  start = strcasestr(aux, AUX_TAGS_EPGSEARCH_START);
  end = strcasestr(aux, AUX_TAGS_EPGSEARCH_END);
  if (start && end) {
     char *tmp;
     // duplicate string
     char *s = strdup(aux);
     char *r = s;
     size_t len = strlen(AUX_HEADER_EPGSEARCH);
     bool founditem = false;
     // add header
     strncpy(r, AUX_HEADER_EPGSEARCH, len);
     r += len;
     // parse first item
     len = strlen(AUX_TAGS_EPGSEARCH_ITEM1_START);
     if ((tmp = strcasestr(start, AUX_TAGS_EPGSEARCH_ITEM1_START)) != NULL) {
        if (tmp < end) {
            char *tmp2;
            if ((tmp2 = strcasestr(tmp, AUX_TAGS_EPGSEARCH_ITEM1_END)) != NULL) {
               // add channel
               strncpy(r, tmp + len, tmp2 - (tmp + len));
               r += (tmp2 - (tmp + len));
               founditem = true;
               }
            else {
               founditem = false;
               }
            }
        }  
     // parse second item
     len = strlen(AUX_TAGS_EPGSEARCH_ITEM2_START);
     if ((tmp = strcasestr(start, AUX_TAGS_EPGSEARCH_ITEM2_START)) != NULL) {
        if (tmp < end) {
            char *tmp2;
            if ((tmp2 = strcasestr(tmp, AUX_TAGS_EPGSEARCH_ITEM2_END)) != NULL) {
               // add separator
               if (founditem) {
                  *(r++) = ',';
                  *(r++) = ' ';
                  }
               // add search item
               strncpy(r, tmp + len, tmp2 - (tmp + len));
               r += (tmp2 - (tmp + len));
               founditem = true;
               }
            else {
               founditem = false;
               }
            } 
        }
     else {
        // parse third item
        len = strlen(AUX_TAGS_EPGSEARCH_ITEM3_START);
        if ((tmp = strcasestr(start, AUX_TAGS_EPGSEARCH_ITEM3_START)) != NULL) {
           if (tmp < end) {
               char *tmp2;
               if ((tmp2 = strcasestr(tmp, AUX_TAGS_EPGSEARCH_ITEM3_END)) != NULL) {
                  // add separator
                  if (founditem) {
                     *(r++) = ',';
                     *(r++) = ' ';
                     }
                  // add search item
                  strncpy(r, tmp + len, tmp2 - (tmp + len));
                  r += (tmp2 - (tmp + len));
                  founditem = true;
                  }
               else {
                  founditem = false;
                  }
               }
           }
        }
     // use old syntax
     if (!founditem) {
        len = strlen(AUX_HEADER_EPGSEARCH);
        strncpy(r, start + len, end - (start + len));
        r += (end - (start + len));
        }
     // copy duplicate string to aux
     *r = 0;
     strn0cpy(aux, s, strlen(aux));
     // free duplicated string
     free(s);
     // .. and return
     return aux;
     }
  // check if vdradmin
  start = strcasestr(aux, AUX_TAGS_VDRADMIN_START);
  end = strcasestr(aux, AUX_TAGS_VDRADMIN_END);
  if (start && end) {
     char *tmp;
     // duplicate string
     char *s = strdup(aux);
     char *r = s;
     size_t len = strlen(AUX_HEADER_VDRADMIN);
     // add header
     strncpy(r, AUX_HEADER_VDRADMIN, len);
     r += len;
     // parse first item
     len = strlen(AUX_TAGS_VDRADMIN_ITEM1_START);
     if ((tmp = strcasestr(start, AUX_TAGS_VDRADMIN_ITEM1_START)) != NULL) {
        if (tmp < end) {
            char *tmp2;
            if ((tmp2 = strcasestr(tmp, AUX_TAGS_VDRADMIN_ITEM1_END)) != NULL) {
               // add search item
               strncpy(r, tmp + len, tmp2 - (tmp + len));
               r += (tmp2 - (tmp + len));
               }
            }
        }
     // copy duplicate string to aux
     *r = 0;
     strn0cpy(aux, s, strlen(aux));
     // free duplicated string
     free(s);
     // .. and return
     return aux;
     }
  // check if pin
  start = strcasestr(aux, AUX_TAGS_PIN_START);
  end = strcasestr(aux, AUX_TAGS_PIN_END);
  if (start && end) {
     char *tmp;
     // duplicate string
     char *s = strdup(aux);
     char *r = s;
     size_t len = strlen(AUX_HEADER_PIN);
     // add header
     strncpy(r, AUX_HEADER_PIN, len);
     r += len;
     // parse first item
     len = strlen(AUX_TAGS_PIN_ITEM1_START);
     if ((tmp = strcasestr(start, AUX_TAGS_PIN_ITEM1_START)) != NULL) {
        if (tmp < end) {
            char *tmp2;
            if ((tmp2 = strcasestr(tmp, AUX_TAGS_PIN_ITEM1_END)) != NULL) {
               // add search item
               strncpy(r, tmp + len, tmp2 - (tmp + len));
               r += (tmp2 - (tmp + len));
               }
            }
        }
     // copy duplicate string to aux
     *r = 0;
     strn0cpy(aux, s, strlen(aux));
     // free duplicated string 
     free(s);
     // .. and return
     return aux;
     }
  // just strip tags
  return striptags(aux);
}

bool ischaracters(const char *str, const char *mask)
{
  bool match = true;
  const char *p = str;
  for (; *p; ++p) {
      const char *m = mask;
      bool tmp = false;
      for (; *m; ++m) {
          if (*p == *m)
             tmp = true;
          }
      match = match && tmp;
      }
  return match;
}

bool ischaracter(const char ch, const char *mask)
{
  bool match = false;
  const char *m = mask;
  for (; *m; ++m) {
      if (ch == *m)
         match = true;
      }
  return match;
}


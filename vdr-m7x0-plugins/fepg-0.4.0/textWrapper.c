#include <string.h>
#include <ctype.h>

#include <vdr/font.h>
#include <vdr/tools.h>
#include "textWrapper.h"


// A slight modification of VDR's cTextWrapper
// Prevents an infinite loop when Width is less than the width of a character
// Adds parameter MaxLines, so that processing stops when we reach this number of lines


cExtTextWrapper::cExtTextWrapper(void)
{
  text = eol = NULL;
  lines = 0;
  lastLine = -1;
}

cExtTextWrapper::cExtTextWrapper(const char *Text, const cFont *Font, int Width, int MaxLines)
{
  text = NULL;
  Set(Text, Font, Width, MaxLines);
}

cExtTextWrapper::~cExtTextWrapper()
{
  free(text);
}

void cExtTextWrapper::Set(const char *Text, const cFont *Font, int Width, int MaxLines)
{
  free(text);
  text = Text ? strdup(Text) : NULL;
  eol = NULL;
  lines = 0;
  lastLine = -1;
  if (!text)
     return;
  lines = 1;
  if (Width <= 0)
     return;

  char *Blank = NULL;
  char *Delim = NULL;
  int w = 0;

  stripspace(text); // strips trailing newlines

  for (char *p = text; *p && (MaxLines != 0 || lines < MaxLines); ) {
      if (*p == '\n') {
         lines++;
         w = 0;
         Blank = Delim = NULL;
         p++;
         continue;
         }
      else if (isspace(*p))
         Blank = p;
      int cw = Font->Width(*p);
      if (w + cw > Width) {
         if (Blank) {
            *Blank = '\n';
            p = Blank;
            continue;
            }
         else {
            // Here's the ugly part, where we don't have any whitespace to
            // punch in a newline, so we need to make room for it:
            if (Delim)
               p = Delim + 1; // let's fall back to the most recent delimiter
            int l = p - text;
            if (l>0 && p[-1] == '\n') // Make sure we didn't already try to break here
               break;
            char *s = MALLOC(char, strlen(text) + 2); // The additional '\n' plus the terminating '\0'
            strncpy(s, text, l);
            s[l] = '\n';
            strcpy(s + l + 1, p);
            free(text);
            text = s;
            p = text + l;
            continue;
            }
         }
      else
         w += cw;
      if (strchr("-.,:;!?_", *p)) {
         Delim = p;
         Blank = NULL;
         }
      p++;
      }  
}

const char *cExtTextWrapper::Text(void)
{
  if (eol) {
     *eol = '\n';
     eol = NULL;
     }
  return text;
}

const char *cExtTextWrapper::GetLine(int Line)
{
  char *s = NULL;
  if (Line < lines) {
     if (eol) {
        *eol = '\n';
        if (Line == lastLine + 1)
           s = eol + 1;
        eol = NULL;
        }
     if (!s) {
        s = text;
        for (int i = 0; i < Line; i++) {
            s = strchr(s, '\n');
            if (s)
               s++;
            else
               break;
            }
        }
     if (s) {
        if ((eol = strchr(s, '\n')) != NULL)
           *eol = 0;
        }
     lastLine = Line;
     }
  return s;
}

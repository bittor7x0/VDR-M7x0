#include <vdr/tools.h>
#include <vdr/osd.h>
#include <stdlib.h>
#include <ctype.h>
#include "osd.h"

#if VDRVERSNUM < 10307
void Frame(cOsdBase *Osd, int x1, int y1, int x2, int y2, eDvbColor Color) {
	Osd->Fill(x1, y1, x2, y1 + 1, Color);
	Osd->Fill(x1, y1, x1 + 1, y2, Color);
	Osd->Fill(x1, y2 - 1, x2, y2, Color);
	Osd->Fill(x2 - 1, y1, x2, y2, Color);
#else /* VDRVERSNUM < 10307 */
void Frame(cOsd *Osd, int x1, int y1, int x2, int y2, tColor Color) {
	Osd->DrawRectangle(x1, y1, x2, y1 + 1, Color);
	Osd->DrawRectangle(x1, y1, x1 + 1, y2, Color);
	Osd->DrawRectangle(x1, y2 - 1, x2, y2, Color);
	Osd->DrawRectangle(x2 - 1, y1, x2, y2, Color);
#endif /* VDRVERSNUM < 10307 */
}

#if VDRVERSNUM < 10307
void Frame(cBitmap *Bitmap, int x1, int y1, int x2, int y2, eDvbColor Color) {
	Bitmap->Fill(x1, y1, x2, y1 + 1, Color);
	Bitmap->Fill(x1, y1, x1 + 1, y2, Color);
	Bitmap->Fill(x1, y2 - 1, x2, y2, Color);
	Bitmap->Fill(x2 - 1, y1, x2, y2, Color);
#else /* VDRVERSNUM < 10307 */
void Frame(cBitmap *Bitmap, int x1, int y1, int x2, int y2, tColor Color) {
	Bitmap->DrawRectangle(x1, y1, x2, y1 + 1, Color);
	Bitmap->DrawRectangle(x1, y1, x1 + 1, y2, Color);
	Bitmap->DrawRectangle(x1, y2 - 1, x2, y2, Color);
	Bitmap->DrawRectangle(x2 - 1, y1, x2, y2, Color);
#endif /* VDRVERSNUM < 10307 */
}

#if VDRVERSNUM < 10307
bool LoadXpm(cBitmap *Bitmap, const char *FileName, eDvbColor NoneColor) {
#else /* VDRVERSNUM < 10307 */
bool LoadXpm(cBitmap *Bitmap, const char *FileName, tColor NoneColor) {
#endif /* VDRVERSNUM < 10307 */
	// Note: Copied from ElchiAIO4d (not stolen, since i created original code)
  bool bRet = false;
  FILE *infile;

	printf("bitmap: %s\n", FileName);

  infile = fopen(FileName, "r");
  if (infile) {
     bool ok = true;
     char buf[512];
     char *ptr;
     int state = 0;
     int width, height, colors, colwidth, cnt = 0;
     int temp;
     uint pal[65536];

     while (ok && fgets(buf, sizeof(buf), infile) != NULL) {
       int len = strlen(buf);
       ptr = buf;
       if (ptr[len - 1] == '\n')
          ptr[--len] = '\0';

       if (state > 0 && strncmp(ptr, "/*", 2) == 0) {
          continue;
          }

       switch (state) {
         case 0:
              if (strcmp(ptr, "/* XPM */") != 0) {
                 esyslog("ERROR: loading xpm %s failed: invalid header", FileName);
                 ok = false;
                 break;
                 }
              ++state;
              break;
         case 1:
              ++state;
              break;
         case 2:
              sscanf(ptr, "\"%d %d %d %d\",", &width, &height, &colors, &colwidth);
              if (colwidth > 2) {
                 esyslog("ERROR: wrong colorwidth in xpm %s", FileName);
                 ok = false;
                 break;
                 }
              cnt = 0;
              ++state;
              break;
         case 3:
              ++ptr;
              temp = 0;
              for (int i = 0; i < colwidth; ++i) {
                  temp <<= 8;
                  temp += (int)*ptr;
                  ++ptr;
                  }
              ++ptr;
              if (strncmp(ptr, "c ", 2) != 0) {
                 esyslog("ERROR: wrong character in xpm %s", FileName);
                 ok = false;
                 break;
                 }
              ptr += 2;
              if (*ptr == '#') {
                 int col = strtoul(++ptr, NULL, 16);
#if VDRVERSNUM < 10307
                 pal[temp] = 0xff000000 | ((col & 0xff) << 16) | (col & 0xff00) | ((col & 0xff0000) >> 16);
#else /* VDRVERSNUM < 10307 */
                 pal[temp] = 0xff000000 | (col & 0xff) | (col & 0xff00) | (col & 0xff0000);
#endif /* VDRVERSNUM < 10307 */
                 }
              else {
                 pal[temp] = NoneColor;
                 }
              if (++cnt == colors) {
                 cnt = 0;
                 ++state;
                 }
              break;
         case 4:
              ++ptr;
              for (int p = 0; p < width; ++p) {
                  temp = 0;
                  for (int i = 0; i < colwidth; ++i) {
                      temp <<= 8;
                      temp += (int)*ptr;
                      ++ptr;
                      }
#if VDRVERSNUM < 10307
                  Bitmap->SetPixel(p, cnt, (eDvbColor)pal[temp]);
#else /* VDRVERSNUM < 10307 */
                  Bitmap->DrawPixel(p, cnt, (tColor)pal[temp]);
#endif /* VDRVERSNUM < 10307 */
                  }
              if (++cnt == height) {
                 ++state;
                 bRet = true;
                 }
              break;
         default:
              break;
         }
       }
     fclose(infile);
     }
  return bRet;
}

#if VDRVERSNUM < 10307
char *WrapText(cBitmap *Bitmap, const char *Text, int Width, int *Height) {
#else /* VDRVERSNUM < 10307 */
char *WrapText(cBitmap *Bitmap, const char *Text, int Width, const cFont *Font, int *Height) {
#endif /* VDRVERSNUM < 10307 */
  // Wraps the Text to make it fit into the area defined by the given Width
  // (which is given in character cells).
  // The actual number of lines resulting from this operation is returned in
  // Height.
  // The returned string is newly created on the heap and the caller
  // is responsible for deleting it once it is no longer used.
  // Wrapping is done by inserting the necessary number of newline
  // characters into the string.
	// 
	// Note: Stolen from vdr 1.2.6 :-)

  int Lines = 1;
  char *t = strdup(Text);
  char *Blank = NULL;
  char *Delim = NULL;
  int w = 0;

  while (*t && t[strlen(t) - 1] == '\n')
        t[strlen(t) - 1] = 0; // skips trailing newlines

  for (char *p = t; *p; ) {
      if (*p == '|')
         *p = '\n';
      if (*p == '\n') {
         Lines++;
         w = 0;
         Blank = Delim = NULL;
         p++;
         continue;
         }
      else if (isspace(*p))
         Blank = p;
#if VDRVERSNUM < 10307
      int cw = Bitmap->Width(*p);
#else /* VDRVERSNUM < 10307 */
      int cw = Font->Width(*p);
#endif /* VDRVERSNUM < 10307 */
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
            char *s = MALLOC(char, strlen(t) + 2); // The additional '\n' plus the terminating '\0'
            int l = p - t;
            strncpy(s, t, l);
            s[l] = '\n';
            strcpy(s + l + 1, p);
            free(t);
            t = s;
            p = t + l;
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

	if (Height != NULL)
#if VDRVERSNUM < 10307
		*Height = Lines * cOsd::LineHeight();
#else /* VDRVERSNUM < 10307 */
		*Height = Lines * Font->Height();
#endif /* VDRVERSNUM < 10307 */
  return t;
}

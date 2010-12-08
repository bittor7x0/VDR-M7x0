#ifndef __FEPG_TEXTWRAPPER_H
#define __FEPG_TEXTWRAPPER_H


class cExtTextWrapper 
{
public:
  cExtTextWrapper(void);
  cExtTextWrapper(const char *Text, const cFont *Font, int Width, int MaxLines=0);
 ~cExtTextWrapper();
 
  void Set(const char *Text, const cFont *Font, int Width, int MaxLines);
  const char *Text(void);
  int Lines(void) { return lines; }
  const char *GetLine(int Line);

private:
  char *text;
  char *eol;
  int lines;
  int lastLine;      
};


#endif //__FEPG_TEXTWRAPPER_H

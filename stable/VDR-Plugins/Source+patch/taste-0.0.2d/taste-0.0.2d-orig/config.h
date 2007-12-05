/*
 * $Id: config.h,v 1.1.1.1 2004/11/19 16:45:31 lordjaxom Exp $
 */

#ifndef VDR_TASTE_CONFIG_H
#define VDR_TASTE_CONFIG_H

class cSetupTaste {
public:
  int  HideMenuEntry;
  int  MessageTimeout;

  cSetupTaste(void);
  bool Parse(const char *Name, const char *Value);
};

extern cSetupTaste SetupTaste;

#endif // VDR_TASTE_CONFIG_H

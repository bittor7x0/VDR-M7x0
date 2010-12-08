#ifndef __FEPG_H
#define __FEPG_H


//////////////////////////////////////////////////////////////////////////////


class cPluginFepg : public cPlugin 
{
public:
  cPluginFepg(void);
  virtual ~cPluginFepg();
  virtual const char* Version(void);
  virtual const char* Description(void);
  virtual const char* CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char* MainMenuEntry(void);
  virtual cOsdObject* MainMenuAction(void);
  virtual cMenuSetupPage* SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  
private:
  // Add any member variables or functions you may need here.  
};
  
  
//////////////////////////////////////////////////////////////////////////////


#endif //__FEPG_H

class myMenuCommands:public cOsdMenu
{
 private:
  cCommands *commands;
  char *parameters;
  eOSState Execute(void);
 public:
  myMenuCommands(const char *Title, cCommands *Commands, const char *Parameters = NULL);
  virtual ~myMenuCommands();
  virtual eOSState ProcessKey(eKeys Key);
};

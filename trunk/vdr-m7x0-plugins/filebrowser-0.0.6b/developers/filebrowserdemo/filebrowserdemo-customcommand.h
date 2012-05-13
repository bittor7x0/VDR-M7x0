#include "filebrowserlib/commands.h"

class cCustomCommand : public cFilebrowserCommand
{
  public:
    cCustomCommand();
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename) { return true; }; 
};

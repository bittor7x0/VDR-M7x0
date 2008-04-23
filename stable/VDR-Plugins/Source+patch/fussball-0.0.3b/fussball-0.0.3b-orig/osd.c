#include "osd.h"

cOSD::cOSD():cMenuText("", "")
{
  	loadFile("/tmp/ergebnisse.txt","Ergebnisse");
  	
	SetHelp("Tabelle","Ergebnisse","","");
}

cOSD::~cOSD()
{
}

eOSState cOSD::ProcessKey(eKeys Key)
{

eOSState state = cMenuText::ProcessKey(Key);


 {
  switch (Key)
  {
   case kRed:	
   		loadFile("/tmp/tabelle.txt","Tabelle");
		break;
   case kGreen:	
   		loadFile("/tmp/ergebnisse.txt","Ergebnisse");
		break;
   case kYellow:
   		//loadFile("","");
		break;
   case kBlue:
	    //LoadFile("","");
		break;
   default:	break;
  }
 }
 return state;
}


void cOSD::loadFile(const char* file, const char* titel){
    dsyslog("Now I should read the File %s",file);

    char *text = NULL;
    char *text2 = NULL;
    char temp[255];

    std::ifstream filestream(file);
    if (!filestream) {
        dsyslog("Error: can't open %s file",file);
        return;
    }
    dsyslog("File is open %s",file);
    //read each line
    for (int i = 0; !filestream.eof(); i++) {
        //dsyslog("read %d line", i);
        filestream.getline(temp, 255);
        if (text)
            text2 = text;
        //dsyslog("%d) %s ", i, temp);
        asprintf(&text, "%s\n%s", text ? text : "", temp);
        //dsyslog("Text: %s", text);
        if (text2)
            delete text2;
    }
    filestream.close();
    SetText(text);
	SetTitle(titel);
	Display();
}


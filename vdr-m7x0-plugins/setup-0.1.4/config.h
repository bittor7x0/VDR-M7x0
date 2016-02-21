/*********************************************************
 * DESCRIPTION: 
 *             Header File
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de
 *
 * Copyright (C) 2004 by Ralf Dotzert 
 *********************************************************/

#ifndef CONFIG_H
#define CONFIG_H
#include "tinyxml2.h"
#include "plugins.h"
#include "sysconfig.h"
#include "menus.h"

using namespace tinyxml2;

/**
@author Ralf Dotzert
*/





class Config{
public:
    Config(char *xmlFile);
    ~Config();
    bool         LoadFile();
    Plugins     *GetPlugins();
    Menus       *GetMenus();
    bool         SaveFile();
    bool         SaveFile(char* fname);
    char        *GetBootLinux();

private:
    XMLDocument         _xmlDoc;
    char               *_filename;
    char               *_bootLinux;
    Menus              _menus;
    Sysconfig          _sysconfig;
    char	           *_sysconfigPre;
    char	      	   *_sysconfigPost;
    Plugins            _activePlugins;
    Plugins            _notInSystemPlugins;
    const char         *_libDir;
    const char         *_libDirCMD;
    bool                loadPlugins(XMLNode *node);
    XMLElement         *savePlugins() ;
    void                savePlugins(Plugins *p, XMLElement *xml);
    void                dumpXMLError();
    void                dumpXMLError(const char* errStr);
    bool                readVdrLib();
    void                prepareSysConfig();
    bool                readVdrLibDir(const char* libDir);
};


#endif

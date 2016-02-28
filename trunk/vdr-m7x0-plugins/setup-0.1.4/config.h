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
#include "plugins.h"
#include "sysconfig.h"
#include "menus.h"

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
    const char   *GetBootLinux();

private:
    XMLDocument         _xmlDoc;
    char               *_filename;
    const char         *_bootLinux;
    Menus              _menus;
    Sysconfig          _sysconfig;
    const char         *_sysconfigFile;
    const char         *_sysconfigPre;
    const char         *_sysconfigPost;
    Plugins            _activePlugins;
    Plugins            _notInSystemPlugins;
    const char         *_libDir;
    const char         *_libDirCMD;
    bool                loadPlugins(XMLNode *node);
    void                dumpXMLError(const char* errStr);
    bool                readVdrLib();
    void                prepareSysConfig();
    bool                readVdrLibDir(const char* libDir);
};

#endif

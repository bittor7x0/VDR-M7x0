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

#ifndef UTIL_H
#define UTIL_H
#include <vdr/tools.h>
#include <string>
using namespace std;

/**
@author Ralf Dotzert
*/
class Util{
public:
    Util();
    ~Util();
    enum Type {UNDEFINED, TEXT, HEX, IP, BOOL, NUMBER, SELECTION, FILE, DIR};
    static char * Strdupnew(const char  *str);
    static char * Strdupnew(const char  *prefix, const char  *str);
    static char * Strdupnew( const char  *str, int size );
	static void trim(string& str,const string& drop=" \t\n\r" );
    static bool isBool(const char *string, bool &flag);
    static bool isBool(const char *string, int  &flag);
    static bool isType( const char * string, Util::Type & typ );
    static bool isNumber( const char * string, int &number );
    static const char * typeToStr(Type type);
    static const char * boolToStr( bool val );
};
#endif

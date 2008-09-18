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

#ifndef SYSCONFIG_H
#define SYSCONFIG_H



/**
@author Ralf Dotzert
*/
class Variable{
const char *_name;
const char *_value;
public:
    Variable();
    ~Variable();
    void SetName(const char *name);
    const char *GetName();
    void SetValue(const char *value);
    const char *GetValue();
    void Print();
};



class Sysconfig{
static Variable *_variables;
static  int       _nr;
static char     *_filename;
public:
    Sysconfig();
    ~Sysconfig();
     void destroy();
     bool LoadFile(const char*fname);
     bool SaveFile();
     void Print();
     void SetVariable(const char* name, const char*value);
    static const char *GetVariable(const char *name);
     char *GetFileName();
private:
     const char *readLine(FILE *fp);
     void addLine(const char *line);
     void addVariable(const char *name, const char *value);
};




#endif

/***************************************************************************
                          tools.h  -  description
                             -------------------
    begin                : Sa Jan 15 2005
    copyright            : (C) 2005 by Holger Brunn
    email                : holger.brunn@stud.uni-karlsruhe.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILEBROWSER_TOOLS_H
#define FILEBROWSER_TOOLS_H

#include <vdr/tools.h>

#ifdef DEBUG
  #define D(s) s
#else
  #define D(s)
#endif

template<class T> class cListContainer : public cListObject
{
  protected:
    T* Object;
    bool FreeOnDel;
  public:
    cListContainer(T *Object, bool FreeOnDel=true)
    {
      this->Object = Object;
      this->FreeOnDel = FreeOnDel;
    }
    ~cListContainer()
    {
      if(FreeOnDel && Object) delete Object;
    };
    T* GetObject() { return Object; };
    void Free() { if(Object) delete Object; };
    cListContainer<T>* Next() { return (cListContainer<T>*)cListObject::Next(); };
    cListContainer<T>* Prev() { return (cListContainer<T>*)cListObject::Prev(); };
};

class cStringContainer : public cListContainer<char>
{
  public:
    cStringContainer(char* Object, bool FreeOnDel=true) : cListContainer<char>(Object, FreeOnDel) {};
    ~cStringContainer() {};
};

class cStringList : public cList<cStringContainer>
{
  public:
    bool Contains(const char* String);
    void Remove(const char* String);
};

#endif

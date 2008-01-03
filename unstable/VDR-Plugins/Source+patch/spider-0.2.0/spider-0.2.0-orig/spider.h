/*
 * Spider-Arachnid: A plugin for the Video Disk Recorder
 *
 * Copyright (C) 2005-2007, Thomas Günther <tom@toms-cafe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id: spider.h 95 2007-09-21 23:01:10Z tom $
 */

#ifndef VDR_SPIDER_H
#define VDR_SPIDER_H

#define USE_TEMPLATES_FROM_STL

#ifdef USE_TEMPLATES_FROM_STL

#include <vector>

namespace Spider
{
  //--- template class Array ---------------------------------------------------

  template <class T>
  class Array : public std::vector<T>
  {
  public:
    Array(int length) : std::vector<T>(length) {}
  };

  //--- template class Vector --------------------------------------------------

  template <class T> class Vector : public std::vector<T> {};

} // namespace Spider

#else // use own templates

namespace Spider
{
  //--- template class Array ---------------------------------------------------

  template <class T>
  class Array
  {
  protected:
    T* array;
    int count;
  public:

    /** Constructor */
    Array(int length)
    {
      array = new T[length];
      count = length;
    }

    /** Destructor */
    ~Array()
    {
      delete[] array;
    }

    /** Reference to an item of the array */
    T& operator[](int p)
    {
      return array[p];
    }

    /** Reference to an item of the array */
    const T& operator[](int p) const
    {
      return array[p];
    }

    /** Length of the array */
    unsigned int size() const
    {
      return count;
    }

    /** Is the array empty? */
    bool empty() const
    {
      return count == 0;
    }
  };


  //--- template class Vector --------------------------------------------------

  template <class T>
  class Vector
  {
  protected:
    T* vector;
    int max;
    int count;
    int steps;

    /** Resize the vector to the new length */
    void resize(int m)
    {
      if (m < steps)
        m = steps;
      else
        m = ((m - 1) / steps + 1) * steps;
      if (m != max)
      {
        T* v = new T[m];
        for (int i = 0; i < count; ++i)
          v[i] = vector[i];
        delete[] vector;
        vector = v;
        max = m;
      }
    }

  public:

    /** Contructor */
    Vector(int stepCount = 10)
    {
      vector = new T[stepCount];
      max = steps = stepCount;
      count = 0;
    }

    /** Destructor */
    ~Vector()
    {
      delete[] vector;
    }

    /** Reference to an item of the vector */
    T& operator[](int p)
    {
      return vector[p];
    }

    /** Reference to an item of the vector */
    const T& operator[](int p) const
    {
      return vector[p];
    }

    /** Reference to an item of the vector */
    T& back()
    {
      return vector[count - 1];
    }

    /** Reference to an item of the vector */
    const T& back() const
    {
      return vector[count - 1];
    }

    /** Current length of the vector */
    unsigned int size() const
    {
      return count;
    }

    /** Is the vector empty? */
    bool empty() const
    {
      return count == 0;
    }

    /** Add an item to the end of the vector */
    void push_back(const T& item)
    {
      resize(count + 1);
      vector[count] = item;
      ++count;
    }

    /** Remove an item from the end of the vector */
    void pop_back()
    {
      if (count > 0)
      {
        --count;
        resize(count);
      }
    }
  };

} // namespace Spider

#endif

#endif // VDR_SPIDER_H

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
 * $Id: history.h 95 2007-09-21 23:01:10Z tom $
 */

#ifndef VDR_SPIDER_HISTORY_H
#define VDR_SPIDER_HISTORY_H

#include "spider.h"


namespace Spider
{
  class Pack;
  class Pile;
  typedef Array<Pile*> Piles;
  class FinalHeap;
  class Move;
  typedef Vector<Move*> Moves;


  //--- class Spider::History --------------------------------------------------

  class History
  {
  private:
    Moves history;
    unsigned int executed;

  public:

    /** Constructor */
    History();

    /** Destructor */
    ~History();

    /** Current move in the history */
    Move* current();

    /** Add a new move */
    void add(Move* move);

    /** Set previous move as current */
    void backward();

    /** Set next move as current */
    void forward();

    /** Are there executed moves in the history */
    bool movesExecuted();

    /** Are there moves to execute in the history */
    bool movesToExecute();
  };


  //--- class Spider::Move -----------------------------------------------------

  class Move
  {
  public:

    /** Destructor */
    virtual ~Move() {};

    /** Do the move */
    virtual void execute() = 0;

    /** Redo the move */
    virtual void takeBack() = 0;
  };


  //--- class Spider::DealMove -------------------------------------------------

  class DealMove : public Move
  {
  private:
    Pack* source;
    Piles& destination;

  public:

    /** Constructor */
    DealMove(Pack* s, Piles& d);

    /** Do the move */
    void execute();

    /** Redo the move */
    void takeBack();
  };


  //--- class Spider::NormalMove -----------------------------------------------

  class NormalMove : public Move
  {
  private:
    Pile* source;
    Pile* destination;
    int count;
    bool turn;

  public:

    /** Constructor */
    NormalMove(Pile* s, Pile* d, int c, bool t);

    /** Do the move */
    void execute();

    /** Redo the move */
    void takeBack();
  };


  //--- class Spider::FinalMove ------------------------------------------------

  class FinalMove : public Move
  {
  private:
    Pile* source;
    FinalHeap* destination;
    int count;
    bool turn;
    bool bonus;

  public:

    /** Constructor */
    FinalMove(Pile* s, FinalHeap* d, int c, bool t, bool b);

    /** Do the move */
    void execute();

    /** Redo the move */
    void takeBack();
  };

} // namespace Spider

#endif // VDR_SPIDER_HISTORY_H

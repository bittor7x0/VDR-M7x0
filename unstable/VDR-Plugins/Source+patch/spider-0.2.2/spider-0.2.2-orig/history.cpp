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
 * $Id: history.cpp 95 2007-09-21 23:01:10Z tom $
 */

#include "history.h"
#include "deck.h"
#include "heap.h"

using namespace Spider;


//--- class Spider::History ----------------------------------------------------

/** Constructor */
History::History()
{
  executed = 0;
}

/** Destructor */
History::~History()
{
  for (unsigned int pos = history.size(); pos > 0; --pos)
  {
    // Remove object created outside of History
    delete history.back();
    history.pop_back();
  }
}

/** Current move in the history */
Move* History::current()
{
  return history[executed - 1];
}

/** Add a new move */
void History::add(Move* move)
{
  for (unsigned int pos = history.size(); pos > executed; --pos)
  {
    // Remove object created outside of History
    delete history.back();
    history.pop_back();
  }
  history.push_back(move);
  ++executed;
}

/** Set previous move as current */
void History::backward()
{
  if (movesExecuted())
    --executed;
}

/** Set next move as current */
void History::forward()
{
  if (movesToExecute())
    ++executed;
}

/** Are there executed moves in the history */
bool History::movesExecuted()
{
  return executed > 0;
}

/** Are there moves to execute in the history */
bool History::movesToExecute()
{
  return executed < history.size();
}


//--- class Spider::DealMove ---------------------------------------------------

/** Constructor */
DealMove::DealMove(Pack* s, Piles& d) :
  source(s), destination(d)
{
}

/** Do the move */
void DealMove::execute()
{
  source->deal(destination);
}

/** Redo the move */
void DealMove::takeBack()
{
  source->takeBackDeal(destination);
}


//--- class Spider::NormalMove -------------------------------------------------

/** Constructor */
NormalMove::NormalMove(Pile* s, Pile* d, int c, bool t) :
  source(s), destination(d), count(c), turn(t)
{
}

/** Do the move */
void NormalMove::execute()
{
  source->moveTo(destination, count);
  if (turn)
    source->turn();
}

/** Redo the move */
void NormalMove::takeBack()
{
  if (turn)
    source->turn();
  destination->moveTo(source, count);
}


//--- class Spider::FinalMove --------------------------------------------------

/** Constructor */
FinalMove::FinalMove(Pile* s, FinalHeap* d, int c, bool t, bool b) :
  source(s), destination(d), count(c), turn(t), bonus(b)
{
}

/** Do the move */
void FinalMove::execute()
{
  source->moveTo(destination, count);
  if (turn)
    source->turn();
  destination->setBonus(bonus);
}

/** Redo the move */
void FinalMove::takeBack()
{
  destination->setBonus(false);
  if (turn)
    source->turn();
  destination->moveTo(source, count);
}

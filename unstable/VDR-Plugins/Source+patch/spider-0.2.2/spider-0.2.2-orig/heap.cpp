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
 * $Id: heap.cpp 95 2007-09-21 23:01:10Z tom $
 */

#include "heap.h"
#include "deck.h"

using namespace Spider;


//--- virtual base class Spider::Heap ------------------------------------------

/** Constructor */
Heap::Heap(unsigned int maxCards)
{
  maxCount = maxCards;
  emptyChanged = true;
}

/** Destructor */
Heap::~Heap()
{
}

/** Current count of cards */
int Heap::count() const
{
  return allCards.size();
}

/** Card in heap */
const Card& Heap::card(int position) const
{
  return allCards[position];
}

/** Top card of the heap */
const Card& Heap::top() const
{
  return allCards.back();
}

/** Add a new card */
void Heap::add(const Card& card)
{
  if (allCards.size() < maxCount)
  {
    if (allCards.empty())
      emptyChanged = true;
    allCards.push_back(card);
  }
}

/** Remove the top card */
void Heap::remove()
{
  if (!allCards.empty())
  {
    allCards.pop_back();
    if (allCards.empty())
      emptyChanged = true;
  }
}

/** Move some matching cards to an other heap */
void Heap::moveTo(Heap* other, int countToMove)
{
  for (int i = count() - countToMove; i < count(); ++i)
    other->add(card(i));
  for (int i = 0; i < countToMove; ++i)
    remove();
}

/** Is the heap empty? */
bool Heap::empty() const
{
  return allCards.empty();
}

/** Is the heap changed? */
bool Heap::changed() const
{
  return emptyChanged;
}

/** Reset changed property */
void Heap::resetChanged()
{
  emptyChanged = false;
}


//--- class Spider::Pack -------------------------------------------------------

/** Constructor */
Pack::Pack(const Deck& deck) :
  Heap(deck.count())
{
  for (int pos = 0; pos < deck.count(); ++pos)
    add(deck.card(pos));
}

/** First initial deal of a game */
void Pack::initialDeal(Piles& piles, int rows, Piles& extra)
{
  for (int r = 0; r < rows; ++r)
    deal(piles);
  deal(extra);

  // turn all open cards
  for (unsigned int p = 0; p < piles.size(); ++p)
    piles[p]->turn();
}

/** Deal one row to the piles */
void Pack::deal(Piles& piles)
{
  for (unsigned int p = 0; p < piles.size(); ++p)
  {
    piles[p]->add(top());
    remove();
  }
}

/** Cancel the deal */
void Pack::takeBackDeal(Piles& piles)
{
  for (int p = piles.size(); --p >= 0; )
  {
    add(piles[p]->top());
    piles[p]->remove();
  }
}


//--- class Spider::Pile -------------------------------------------------------

/** Constructor */
Pile::Pile(const Deck& deck) :
  Heap(deck.count())
{
  currentOpen = 0;
  currentMatching = 0;
  currentSelected = 0;
  currentChanged = 0;
}

/** Add a new card */
void Pile::add(const Card& card)
{
  Heap::add(card);
  ++currentOpen;
  if (topCardsMatches())
    ++currentMatching;
  currentSelected = 0;
  ++currentChanged;
}

/** Remove top card from pile */
void Pile::remove()
{
  if (currentSelected > 0)
    --currentSelected;
  if (topCardsMatches())
    --currentMatching;
  if (currentOpen > 0)
    --currentOpen;
  Heap::remove();
  if (currentChanged > 0)
    --currentChanged;
  if (currentChanged < 1 && !empty())
    currentChanged = 1;
}

/** Turn all open top cards or rather open the top card */
void Pile::turn()
{
  if (currentOpen < 1 && !empty())
    currentOpen = 1;
  else
    currentOpen = 0;
  currentMatching = 0;
  if (currentChanged < 1 && !empty())
    currentChanged = 1;
}

/** Current count of open cards */
int Pile::open() const
{
  return currentOpen;
}

/** Current count of matching cards */
int Pile::getMatching() const
{
  return currentMatching;
}

/** The two open top cards are matching */
bool Pile::topCardsMatches() const
{
  return (open() >= 2 && card(count() - 1).matchesTo(card(count() - 2)));
}

/** Current count of selected cards */
int Pile::selected() const
{
  return currentSelected;
}

/** Select up to max matching cards on the end of this pile */
void Pile::select(int max)
{
  currentSelected = 0;
  if (open() > 0)
  {
    currentSelected = 1;
    for (int i = count(); --i > count() - open(); )
      if (card(i).matchesTo(card(i - 1)))
        currentSelected++;
      else
        break;
  }
  if (currentSelected > max && max > 0)
    currentSelected = max;
  if (currentChanged < currentSelected)
    currentChanged = currentSelected;
}

/** Unselect this pile */
void Pile::unselect()
{
  if (currentChanged < currentSelected)
    currentChanged = currentSelected;
  currentSelected = 0;
}

/** Adapt the selection to match an other pile */
void Pile::adaptSelectionTo(const Pile* other)
{
  if (!other->empty())
  {
    if (currentChanged < currentSelected)
      currentChanged = currentSelected;
    int diff = other->top().rank - top().rank;
    if (diff > 0 && diff <= currentSelected)
      currentSelected = diff;
    else
      currentSelected = 0;
  }
}

/** Matches the selection to an other pile? */
bool Pile::selectionMatchesTo(const Pile* other, bool matchSuit) const
{
  return (!other->empty() &&
          (other->top().rank == top().rank + currentSelected) &&
          (other->top().suit == top().suit || !matchSuit));
}

/** Is the heap changed? */
bool Pile::changed() const
{
  return (Heap::changed() || currentChanged > 0);
}

/** Reset changed property */
void Pile::resetChanged()
{
  Heap::resetChanged();
  currentChanged = 0;
}

/** How many cards are changed? */
int Pile::cardsChanged() const
{
  return currentChanged;
}


//--- class Spider::FinalHeap --------------------------------------------------

/** Constructor */
FinalHeap::FinalHeap(const Deck& deck) :
  Heap(deck.cardsInSuit)
{
  bonus = false;
}

/** Set bonus of the final heap */
void FinalHeap::setBonus(bool newBonus)
{
  bonus = newBonus;
}

/** Has this final heap bonus? */
bool FinalHeap::getBonus() const
{
  return bonus;
}

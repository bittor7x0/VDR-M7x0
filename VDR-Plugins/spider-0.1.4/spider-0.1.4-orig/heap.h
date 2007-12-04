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
 * $Id: heap.h 87 2007-06-22 22:37:36Z tom $
 */

#ifndef VDR_SPIDER_HEAP_H
#define VDR_SPIDER_HEAP_H

#include "spider.h"
class Card;
typedef Vector<Card> CardStack;
class Deck;
class Pile;
typedef Array<Pile*> Piles;


/** --- base class Heap ---------------------------------------------------- **/

class Heap
{
protected:
  CardStack allCards;
  unsigned int maxCount;
  bool emptyChanged;

  /** Constructor */
  Heap(unsigned int maxCards);

  /** Destructor */
  virtual ~Heap();

public:

  /** Current count of cards */
  int count() const;

  /** Card in heap */
  const Card& card(int position) const;

  /** Top card of the heap */
  const Card& top() const;

  /** Add a new card */
  virtual void add(const Card& card);

  /** Remove the top card */
  virtual void remove();

  /** Move some matching cards to an other heap */
  void moveTo(Heap* other, int countToMove);

  /** Is the heap empty? */
  bool empty() const;

  /** Is the heap changed? */
  bool changed() const;

  /** Reset changed property */
  void resetChanged();
};


/** --- class Pack --------------------------------------------------------- **/

class Pack : public Heap
{
public:

  /** Constructor */
  Pack(const Deck& deck);

  /** First initial deal of a game */
  void initialDeal(Piles& piles, int rows, Piles& extra);

  /** Deal one row to the piles */
  void deal(Piles& piles);

  /** Cancel the deal */
  void takeBackDeal(Piles& piles);
};


/** --- class Pile --------------------------------------------------------- **/

class Pile : public Heap
{
protected:
  int currentOpen;
  int currentMatching;
  int currentSelected;
  int currentChanged;

public:

  /** Constructor */
  Pile(const Deck& deck);

  /** Add a new card */
  void add(const Card& card);

  /** Remove top card from pile */
  void remove();

  /** Turn all open top cards or rather open the top card */
  void turn();

  /** Current count of open cards */
  int open() const;

  /** Current count of matching cards */
  int getMatching() const;

  /** The two open top cards are matching */
  bool topCardsMatches() const;

  /** Current count of selected cards */
  int selected() const;

  /** Select up to max matching cards on the end of this pile */
  void select(int max = 0);

  /** Unselect this pile */
  void unselect();

  /** Adapt the selection to match an other pile */
  void adaptSelectionTo(const Pile* other);

  /** Matches the selection to an other pile? */
  bool selectionMatchesTo(const Pile* other, bool matchSuit = false) const;

  /** Is the heap changed? */
  bool changed() const;

  /** Reset changed property */
  void resetChanged();

  /** How many cards are changed? */
  int cardsChanged() const;
};


/** --- class FinalHeap ---------------------------------------------------- **/

class FinalHeap : public Heap
{
private:
  bool bonus;

public:

  /** Constructor */
  FinalHeap(const Deck& deck);

  /** Set bonus of the final heap */
  void setBonus(bool newBonus);

  /** Has this final heap bonus? */
  bool getBonus() const;
};

#endif // VDR_SPIDER_HEAP_H

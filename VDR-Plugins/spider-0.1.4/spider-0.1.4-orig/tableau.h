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
 * $Id: tableau.h 87 2007-06-22 22:37:36Z tom $
 */

#ifndef VDR_SPIDER_TABLEAU_H
#define VDR_SPIDER_TABLEAU_H

#include "spider.h"
class Deck;
class Card;
class Pack;
class Pile;
typedef Array<Pile*> Piles;
class FinalHeap;
typedef Array<FinalHeap*> Finals;
class History;


/** --- class Tableau ------------------------------------------------------ **/

class Tableau
{
private:
  int dealCount;
  int cardsToOpen;
  Deck& deck;

public:
  Pack* pack;
  Piles piles;
  Finals finals;
  Pile* selected;
  bool changed;
  History* history;

  /** Constructor */
  Tableau(Deck& deck, int pileCount, int finalCount, int deals);

  /** Destructor */
  ~Tableau();

  /** Current count of deals */
  int deals();

  /** Current count of points */
  int points();

  /** Is no pile empty? */
  bool noPileEmpty();

  /** Matches all cards in all piles? */
  bool allCardsMatches();

  /** Is the game over? */
  bool gameOver();

  /** Select p-th pile by selecting up to max matching cards on its end */
  void select(int p, int max = 0);

  /** Unselect the selected pile */
  void unselect();

  /** Move cards from selected pile to p-th pile */
  void move(int p);

  /** Search move from p-th pile to the next left pile, return destination */
  int autoMoveLeft(int p);

  /** Search move from p-th pile to the next right pile, return destination */
  int autoMoveRight(int p);

  /** Search best move from p-th pile, return destination */
  int autoMove(int p);

  /** Deal one row */
  void deal();

  /** Remove one suit of cards from selected pile to the final heaps */
  void remove();

  /** Go one move backward in the history */
  void backward();

  /** Go one move forward in the history */
  void forward();
};

#endif // VDR_SPIDER_TABLEAU_H

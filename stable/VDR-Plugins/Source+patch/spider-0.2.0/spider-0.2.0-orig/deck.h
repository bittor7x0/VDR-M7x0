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
 * $Id: deck.h 95 2007-09-21 23:01:10Z tom $
 */

#ifndef VDR_SPIDER_DECK_H
#define VDR_SPIDER_DECK_H

#include "spider.h"


namespace Spider
{
  class Card;
  typedef Array<Card> Cards;


  //--- class Spider::Deck -----------------------------------------------------

  class Deck
  {
  protected:
    Cards allCards;
  public:
    int cardsInSuit;
    int suitCount;
    int deckCount;

    /** Constructor */
    Deck(int cards, int suits, int decks);

    /** Current count of cards */
    int count() const;

    /** Card in deck */
    const Card& card(int position) const;

    /** Shuffle the deck */
    void shuffle();
  };


  //--- class Spider::Card -----------------------------------------------------

  class Card
  {
  public:
    int suit;
    int rank;

    /** Constructor */
    Card(int s = -1, int r = -1);

    /** Matches this card to an other card? */
    bool matchesTo(const Card& other) const;
  };

} // namespace Spider

#endif // VDR_SPIDER_DECK_H

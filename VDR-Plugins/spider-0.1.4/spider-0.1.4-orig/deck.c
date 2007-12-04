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
 * $Id: deck.c 87 2007-06-22 22:37:36Z tom $
 */

#include "deck.h"
#include <vdr/config.h>
#include <vdr/tools.h>
#include <stdlib.h>


/** Random number 0 .. max-1 */
unsigned int rand(unsigned int max)
{
  static unsigned int seed = cTimeMs::Now();
  return (unsigned int)((double)max * rand_r(&seed) / (RAND_MAX + 1.0));
}


/** --- class Deck --------------------------------------------------------- **/

/** Constructor */
Deck::Deck(int cards, int suits, int decks) :
  allCards(cards * suits * decks)
{
  cardsInSuit = cards;
  suitCount = suits;
  deckCount = decks;
  shuffle();
}

/** Current count of cards */
int Deck::count() const
{
  return allCards.size();
}

/** Card in deck */
const Card& Deck::card(int position) const
{
  return allCards[position];
}

/** Shuffle the deck */
void Deck::shuffle()
{
  for (unsigned int i = 0; i < allCards.size(); ++i)
  {
    int pos = -1;
    while (pos < 0)
      pos = rand(allCards.size());
    while (allCards[pos].suit >= 0)
      pos = (pos + 1) % allCards.size();
    allCards[pos] = Card((i / cardsInSuit) % suitCount, i % cardsInSuit);
  }
}


/** --- class Card --------------------------------------------------------- **/

/** Constructor */
Card::Card(int s, int r)
{
  suit = s;
  rank = r;
}

/** Matches this card to an other card? */
bool Card::matchesTo(const Card& other) const
{
  return (suit == other.suit && rank + 1 == other.rank);
}

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
 * $Id: tableau.cpp 95 2007-09-21 23:01:10Z tom $
 */

#include "tableau.h"
#include "deck.h"
#include "heap.h"
#include "history.h"

using namespace Spider;


//--- class Spider::Tableau ----------------------------------------------------

/** Constructor */
Tableau::Tableau(Deck& deck, int pileCount, int finalCount, int deals) :
  dealCount(deals), deck(deck), piles(pileCount), finals(finalCount)
{
  cardsToOpen = deck.count() - (deals + 1) * pileCount;
  for ( ; cardsToOpen < 0; cardsToOpen += pileCount)
    --deals;

  pack = new Pack(deck);
  for (unsigned int p = 0; p < piles.size(); ++p)
    piles[p] = new Pile(deck);
  for (unsigned int f = 0; f < finals.size(); ++f)
    finals[f] = new FinalHeap(deck);

  history = new History();

  // choice of piles for extra deal with remaining cards
  Piles extra(cardsToOpen % pileCount);
  if (!extra.empty())
  {
    int extraMax = extra.size() - 1;
    int pilesMax = piles.size() - 1;
    for (int e = 0; e <= extraMax / 2; ++e)
    {
      int p = (e * pilesMax) / extraMax;
      extra[e] = piles[p];
      extra[extraMax - e] = piles[pilesMax - p];
    }
  }

  // deal cards to open
  pack->initialDeal(piles, cardsToOpen / pileCount, extra);

  // deal one open row
  pack->deal(piles);

  selected = 0;
}

/** Destructor */
Tableau::~Tableau()
{
  delete pack;
  for (unsigned int p = 0; p < piles.size(); ++p)
    delete piles[p];
  for (unsigned int f = 0; f < finals.size(); ++f)
    delete finals[f];
  delete history;
}

/** Current count of deals */
int Tableau::deals()
{
  return dealCount - pack->count() / piles.size();
}

/** Current count of points */
int Tableau::points()
{
  int openCard        = 10;
  int openPile        = 15;
  int matchingCard    = 2;
  int readyFinal      = 50;
  int bonusFinal      = 2;
  int bonusfreeFinals = 3;

  int points = openCard * cardsToOpen;
  for (unsigned int p = 0; p < piles.size(); ++p)
  {
    if (piles[p]->count() > piles[p]->open())
      points -= openCard * (piles[p]->count() - piles[p]->open());
    else
      points += openPile;
    points += matchingCard * piles[p]->getMatching();
  }
  int emptyFinals = 0;
  int bonusFinals = 0;
  for (unsigned int f = 0; f < finals.size(); ++f)
    if (finals[f]->empty())
      ++emptyFinals;
    else if (finals[f]->getBonus())
      ++bonusFinals;
  points += readyFinal * (finals.size() - emptyFinals);
  if (emptyFinals == 0 && bonusFinals > bonusfreeFinals)
    points += bonusFinal * (bonusFinals - bonusfreeFinals);
  return points;
}

/** Is no pile empty? */
bool Tableau::noPileEmpty()
{
  for (unsigned int p = 0; p < piles.size(); p++)
    if (piles[p]->empty())
      return false;
  return true;
}

/** Matches all cards in all piles? */
bool Tableau::allCardsMatches()
{
  for (unsigned int p = 0; p < piles.size(); ++p)
    if (piles[p]->count() > piles[p]->open() ||
        piles[p]->count() > piles[p]->getMatching() *
                            deck.cardsInSuit / (deck.cardsInSuit - 1))
      return false;
  return true;
}

/** Is the game over? */
bool Tableau::gameOver()
{
  for (unsigned int p = 0; p < piles.size(); p++)
    if (!piles[p]->empty())
      return false;
  return true;
}

/** Select p-th pile by selecting up to max matching cards on its end */
void Tableau::select(int p, int max)
{
  if (!piles[p]->empty())
  {
    unselect();
    selected = piles[p];
    selected->select(max);
    changed = true;
  }
}

/** Unselect the selected pile */
void Tableau::unselect()
{
  if (selected)
  {
    selected->unselect();
    selected = 0;
  }
}

/** Move cards from selected pile to p-th pile */
void Tableau::move(int p)
{
  selected->adaptSelectionTo(piles[p]);
  int count = selected->selected();
  if (count > 0)
  {
    bool turn = (count == selected->open() && count < selected->count());
    history->add(new NormalMove(selected, piles[p], count, turn));
    history->current()->execute();
  }
  unselect();
  changed = true;
}

/** Search move from p-th pile to the next left pile, return destination */
int Tableau::autoMoveLeft(int p)
{
  int i = -1;
  if (!piles[p]->empty())
  {
    if (selected != piles[p])
      select(p);
    for (i = p - 1; i >= 0; --i)
      if (piles[i]->empty() || selected->selectionMatchesTo(piles[i]))
        break;
    if (i >= 0)
      move(i);
    changed = true;
  }
  return i;
}

/** Search move from p-th pile to the next right pile, return destination */
int Tableau::autoMoveRight(int p)
{
  int i = -1;
  if (!piles[p]->empty())
  {
    if (selected != piles[p])
      select(p);
    for (i = p + 1; i < (int)piles.size(); ++i)
      if (piles[i]->empty() || selected->selectionMatchesTo(piles[i]))
        break;
    if (i < (int)piles.size())
      move(i);
    else
      i = -1;
    changed = true;
  }
  return i;
}

/** Search best move from p-th pile, return destination */
int Tableau::autoMove(int p)
{
  int i = -1;
  if (!piles[p]->empty())
  {
    if (selected != piles[p])
      select(p);
    if (allCardsMatches() && selected->selected() == deck.cardsInSuit)
      remove();
    else
    {
      i = p;
      while ((i = (i + 1) % piles.size()) != p)
        if (selected->selectionMatchesTo(piles[i], true))
          break;
      if (i == p)
        while ((i = (i + 1) % piles.size()) != p)
          if (selected->selectionMatchesTo(piles[i], false))
            break;
      if (i == p)
        while ((i = (i + 1) % piles.size()) != p)
          if (piles[i]->empty())
            break;
      if (i != p)
        move(i);
      else
        i = -1;
    }
  }
  return i;
}

/** Deal one row */
void Tableau::deal()
{
  if (!pack->empty() && noPileEmpty())
  {
    history->add(new DealMove(pack, piles));
    history->current()->execute();

    unselect();
    changed = true;
  }
}

/** Remove one suit of cards from selected pile to the final heaps */
void Tableau::remove()
{
  int count = selected->selected();
  if (count == deck.cardsInSuit)
  {
    unsigned int f;
    for (f = 0; f < finals.size(); ++f)
      if (finals[f]->empty())
        break;
    if (f < finals.size())
    {
      bool turn = (count == selected->open() && count < selected->count());
      bool bonus = allCardsMatches();
      history->add(new FinalMove(selected, finals[f], count, turn, bonus));
      history->current()->execute();
    }
    unselect();
    changed = true;
  }
}

/** Go one move backward in the history */
void Tableau::backward()
{
  if (history->movesExecuted())
  {
    history->current()->takeBack();
    history->backward();

    unselect();
    changed = true;
  }
}

/** Go one move forward in the history */
void Tableau::forward()
{
  if (history->movesToExecute())
  {
    history->forward();
    history->current()->execute();

    unselect();
    changed = true;
  }
}

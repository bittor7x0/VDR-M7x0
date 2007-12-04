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
 * $Id: game.c 87 2007-06-22 22:37:36Z tom $
 */

#include "game.h"
#include "deck.h"
#include "tableau.h"
#include "heap.h"
#include "setup.h"
#include "bitmap.h"
#include "i18n.h"
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>


// Defintions for bitmaps
const int cursorWidth  = 16;
const int cursorHeight = 22;
const int cardWidth  = 71;
const int cardHeight = 96;

const char* cursorName = "cursor";
const char* coverName  = "cover";
const char* suitNames[] = { "herz", "karo", "pik", "kreuz" };
const char* rankNames[] = { "ace", "two", "three", "four", "five",
                            "six", "seven", "eight", "nine", "ten",
                            "jack", "queen", "king" };
const int suitCount = sizeof(suitNames) / sizeof(*suitNames);
const int rankCount = sizeof(rankNames) / sizeof(*rankNames);

Bitmap* cursor = NULL;
Bitmap* back = NULL;
Bitmap* frame = NULL;
Bitmap* cards[suitCount][rankCount];


/** --- class SpiderGame --------------------------------------------------- **/

/** Constructor */
SpiderGame::SpiderGame(const SpiderSetup& setup, const char* confdir) :
  cOsdObject(true), setup(setup), confdir(confdir)
{
  width = 504;
  height = 360;
  xPos = (720 - width) / 2;
  yPos = (576 - height) / 2;
  xDist = 1;
  yDist = cardHeight / 6;
  osd = NULL;
  deck = NULL;
  tableau = NULL;
  currentPile = 0;
  status = cursorOnPile;
  info = new Bitmap(width * 2 / 3, 60);
  infoText = NULL;
}

/** Destructor */
SpiderGame::~SpiderGame()
{
  delete deck;
  delete tableau;
  delete info;
  delete osd;
  for (int s = 0; s < suitCount; ++s)
    for (int r = 0; r < rankCount; ++r)
      delete cards[s][r];
  delete frame;
  delete back;
  delete cursor;
  frame = back = cursor = NULL;
}

/** Display the game on the OSD */
void SpiderGame::Show()
{
  osd = cOsdProvider::NewOsd(0, 0);
  if (osd)
  {
    tArea area = { xPos, yPos, xPos + width - 1, yPos + height - 1, 4 };
    osd->SetAreas(&area, 1);
    start();
    paint();
  }
}

/** Process user events */
eOSState SpiderGame::ProcessKey(eKeys key)
{
  eOSState state = cOsdObject::ProcessKey(key);
  if (state == osUnknown)
  {
    if (key == kBack)
      return osEnd;
    if (key == kBlue || (status == gameOver && key == kOk))
    {
      start();
      status = cursorOnPile;
      currentPile = 0;
    }
    else if (status == cursorOnPile)
    {
      switch (key)
      {
        case kLeft:
        case kLeft|k_Repeat:
          if (currentPile > 0)
            --currentPile;
          break;
        case kRight:
        case kRight|k_Repeat:
          if (currentPile < tableau->piles.size() - 1)
            ++currentPile;
          break;
        case kUp:
          status = cursorOnPack;
          break;
        case kOk:
          if (!tableau->piles[currentPile]->empty())
          {
            tableau->select(currentPile);
            status = selectedPile;
          }
          break;
        case kGreen:
          tableau->backward();
          break;
        case kYellow:
          tableau->forward();
          break;
        default:
          return osContinue;
      }
    }
    else if (status == selectedPile)
    {
      int selected = tableau->selected->selected();
      int destination;
      switch (key)
      {
        case kLeft:
          destination = tableau->autoMoveLeft(currentPile);
          if (destination >= 0)
          {
            currentPile = destination;
            tableau->select(currentPile, selected);
          }
          break;
        case kRight:
          destination = tableau->autoMoveRight(currentPile);
          if (destination >= 0)
          {
            currentPile = destination;
            tableau->select(currentPile, selected);
          }
          break;
        case kUp:
        case kUp|k_Repeat:
          tableau->selected->select(selected + 1);
          if (key == kUp)
          {
            if (selected == deck->cardsInSuit)
            {
              tableau->remove();
              status = cursorOnPile;
            }
            else if (selected == tableau->selected->selected())
              infoText = tr("Only complete suits are allowed to remove");
          }
          break;
        case kDown:
        case kDown|k_Repeat:
          if (selected > 1)
            tableau->selected->select(selected - 1);
          break;
        case kOk:
          tableau->unselect();
          status = cursorOnPile;
          break;
        default:
          return osContinue;
      }
    }
    else if (status == cursorOnPack)
    {
      switch (key)
      {
        case kDown:
          status = cursorOnPile;
          currentPile = 0;
          break;
        case kOk:
          if (tableau->pack->empty())
            infoText = tr("No cards left");
          else if (!tableau->noPileEmpty())
            infoText = tr("Deal not allowed with empty piles");
          else
            tableau->deal();
          break;
        default:
          return osContinue;
      }
    }
    if (tableau->gameOver())
    {
      status = gameOver;
      infoText = tr("Congratulations!\nPress OK to start a new game");
    }
    paint();
    state = osContinue;
  }
  return state;
}

/** Start a new game */
void SpiderGame::start()
{
  // Load bitmaps
  if (cursor == NULL)
  {
    cursor = new Bitmap(cursorWidth, cursorHeight, confdir, cursorName);
    back = new Bitmap(cardWidth, cardHeight, confdir, coverName);
    frame = new Bitmap(cardWidth, cardHeight, clrWhite, clrGray50);

    for (int s = 0; s < suitCount; ++s)
      for (int r = 0; r < rankCount; ++r)
        cards[s][r] = new Bitmap(cardWidth, cardHeight, confdir,
                                 suitNames[s], rankNames[r]);
  }

  delete deck;
  delete tableau;
  deck = NULL;
  tableau = NULL;

  int deckCount, dealCount, pileCount;

  if (setup.variation == SpiderSetup::Mini)
  {
    deckCount = 1;
    dealCount = 4;
    pileCount = 7;
  }
  else // normal variation
  {
    deckCount = 2;
    dealCount = 5;
    pileCount = 10;
    xDist = -23;
  }

  if (pileCount > rankCount * suitCount * deckCount)
    pileCount = rankCount * suitCount * deckCount;
  int finalCount = suitCount * deckCount;
  deck = new Deck(rankCount, suitCount, deckCount);
  tableau = new Tableau(*deck, pileCount, finalCount, dealCount);
}

/** Paint all pieces of the game */
void SpiderGame::paint()
{
  int x1 = xPos;
  int x2 = xPos + width - 1;
  int y1 = yPos;
  int y2 = yPos + height - 1;

  // Save and restore palette to reduce flickering
  cPalette savePalette(*osd->GetBitmap(0));
  osd->DrawRectangle(x1,     y1,     x2,     y2,     clrGray50);
  osd->SetPalette(savePalette, 0);

  // Paint red frame
  osd->DrawRectangle(x1,     y1,     x2,     y1 + 1, clrRed);
  osd->DrawRectangle(x1,     y1,     x1 + 1, y2,     clrRed);
  osd->DrawRectangle(x1,     y2 - 1, x2,     y2,     clrRed);
  osd->DrawRectangle(x2 - 1, y1,     x2,     y2,     clrRed);

  paintPack();

  unsigned int f;
  for (f = 0; f < tableau->finals.size(); ++f)
    if (tableau->finals[f]->empty())
      break;
  unsigned int count = f;
  for (f = tableau->finals.size(); f-- > count;)
    paintFinal(f);
  for (f = 0; f < count; ++f)
    paintFinal(f);

  unsigned int p;
  for (p = tableau->piles.size(); p-- > currentPile + 1;)
    paintPile(p);
  for (p = 0; p <= currentPile; ++p)
    paintPile(p);

  if (infoText)
  {
    info->text(infoText);
    osd->DrawBitmap(xPos + (width - info->Width()) / 2, yPos + 10, *info);
    infoText = NULL;
  }
  osd->Flush();
}

/** Paint the pack */
void SpiderGame::paintPack()
{
  int packX = xPos + 1;
  int packY = yPos + 1;
  if (tableau->pack->empty())
    paintFrame(packX, packY);
  else
    paintBack(packX, packY);
  if (status == cursorOnPack)
    paintCursor(packX, packY);
}

/** Paint a final heap */
void SpiderGame::paintFinal(unsigned int f)
{
  int offset = tableau->piles.size() - tableau->finals.size();
  int finalX = xPos + 1 + (f + offset) * (cardWidth + xDist);
  int finalY = yPos + 1;
  if (tableau->finals[f]->empty())
    paintFrame(finalX, finalY);
  else
    paintCard(finalX, finalY, tableau->finals[f]->top());
}

/** Paint a pile */
void SpiderGame::paintPile(unsigned int p)
{
  int pileX = xPos + 1 + p * (cardWidth + xDist);
  int pileY = yPos + 1 + cardHeight + 1;
  paintFrame(pileX, pileY);

  int count = tableau->piles[p]->count();
  int closed = count - tableau->piles[p]->open();
  int unselected = count - tableau->piles[p]->selected();
  int dist = yDist;
  if (pileY + (count + 1) * dist > yPos + height)
    dist = (yPos + height - pileY) / (count + 1);

  for (int c = 0; c < count; ++c, pileY += dist)
  {
    if (c == unselected)
      pileY += dist;
    if (c < closed)
      paintBack(pileX, pileY);
    else
      paintCard(pileX, pileY, tableau->piles[p]->card(c));
  }
  if (count > 0)
    pileY -= dist;
  if ((status == cursorOnPile || status == selectedPile) && p == currentPile)
    paintCursor(pileX, pileY);
}

/** Paint the cursor onto a card */
void SpiderGame::paintCursor(int x, int y)
{
  int x0 = x + (cardWidth - cursorWidth) / 2;
  int y0 = y + (cardHeight - cursorHeight) / 2;
  tColor color = clrBlue;
  if (status == selectedPile)
    color = clrYellow;
  for (x = 0; x < cursorWidth; ++x)
    for (y = 0; y < cursorHeight; ++y)
      if (cursor->Color(*cursor->Data(x, y)) != clrTransparent)
        osd->DrawRectangle(x0 + x, y0 + y, x0 + x, y0 + y, color);
}

/** Paint an empty card frame */
void SpiderGame::paintFrame(int x, int y)
{
  osd->DrawBitmap(x, y, *frame);
}

/** Paint a card back */
void SpiderGame::paintBack(int x, int y)
{
  osd->DrawBitmap(x, y, *back);
}

/** Paint a card */
void SpiderGame::paintCard(int x, int y, const Card& card)
{
  osd->DrawBitmap(x, y, *cards[card.suit][card.rank]);
}

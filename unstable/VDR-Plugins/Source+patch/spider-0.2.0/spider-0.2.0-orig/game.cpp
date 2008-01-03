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
 * $Id: game.cpp 97 2007-09-24 22:29:48Z tom $
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

using namespace SpiderPlugin;
using namespace Spider;


// Definitions for bitmaps
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


//--- class SpiderPlugin::Game -------------------------------------------------

const Game::Colors Game::full_colors =
{
  clrGray50,  // background      50% gray
  clrRed,     // osd_frame       red
  clrWhite,   // card_frame      white
  clrGray50,  // card_frame_bg   50% gray
  clrBlue,    // inactive_cursor blue
  clrYellow   // active_cursor   yellow
};
const Game::Colors Game::reduced_colors =
{
  0xFF0000FF, // background      blue
  0xFFFF0000, // osd_frame       red
  0xFFFFFFFF, // card_frame      white
  0xFF000000, // card_frame_bg   black
  0xFF0000FF, // inactive_cursor blue
  0xFF0000FF  // active_cursor   blue
};

/** Constructor */
Game::Game(const SetupData& setup, const char* confdir,
           Deck*& deck, Tableau*& tableau) :
  cOsdObject(true), setup(setup), confdir(confdir), deck(deck), tableau(tableau)
{
  osd = NULL;
  currentPile = 0;
  status = cursorOnPile;
  info = new Bitmap(360, 60);
  infoText = NULL;
  colors = full_colors;
}

/** Destructor */
Game::~Game()
{
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
void Game::Show()
{
  osd = cOsdProvider::NewOsd(0, 0);
  if (osd)
  {
    tArea area = { setup.osd_left,
                   setup.osd_top,
                   setup.osd_left + (setup.osd_width & ~0x03) - 1,
                   setup.osd_top + setup.osd_height - 1,
                   4 };
    eOsdError err = osd->CanHandleAreas(&area, 1);
    if (err == oeOutOfMemory &&
        setup.osd_error_compensation == SetupData::ReduceColors)
    {
      // Reduce colors
      area.bpp = 2;
      err = osd->CanHandleAreas(&area, 1);
    }
    while (err == oeOutOfMemory)
    {
      if (setup.osd_error_compensation != SetupData::ShrinkWidth)
      {
        // Shrink height
        area.y1 += 2;
        area.y2 -= 2;
      }
      if (setup.osd_error_compensation == SetupData::ShrinkWidth ||
          setup.osd_error_compensation == SetupData::ShrinkWidthHeight)
      {
        // Shrink width
        area.x1 += 2;
        area.x2 -= 2;
      }
      err = osd->CanHandleAreas(&area, 1);
    }

    // Before setting the osd area - check if we are near the osd memory limit -
    // then we use an workaround for driver error (no output near memory limit)
    // I hope an extra shrink of 12 lines height is enough to avoid the error
    area.y2 += 12;
    err = osd->CanHandleAreas(&area, 1);
    area.y2 -= 12;
    if (err == oeOutOfMemory)
    {
      area.y1 += 6;
      area.y2 -= 6;
    }
    osd->SetAreas(&area, 1);
    if (area.bpp <= 2)
      colors = reduced_colors;

    // Load bitmaps
    cursor = new Bitmap(cursorWidth, cursorHeight, confdir, cursorName);
    back = new Bitmap(cardWidth, cardHeight, confdir, coverName);
    frame = new Bitmap(cardWidth, cardHeight, colors.card_frame,
                                              colors.card_frame_bg);
    for (int s = 0; s < suitCount; ++s)
      for (int r = 0; r < rankCount; ++r)
        cards[s][r] = new Bitmap(cardWidth, cardHeight, confdir,
                                 suitNames[s], rankNames[r]);

    if (deck == NULL || tableau == NULL)
      start();
    paint();
  }
}

/** Process user events */
eOSState Game::ProcessKey(eKeys key)
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
        case kGreen:
          tableau->unselect();
          tableau->backward();
          status = cursorOnPile;
          break;
        case kYellow:
          tableau->unselect();
          tableau->forward();
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
        case kLeft:
        case kRight:
        case kDown:
          status = cursorOnPile;
          break;
        case kOk:
          if (tableau->pack->empty())
            infoText = tr("No cards left");
          else if (!tableau->noPileEmpty())
            infoText = tr("Deal not allowed with empty piles");
          else
            tableau->deal();
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
void Game::start()
{
  delete deck;
  delete tableau;
  deck = NULL;
  tableau = NULL;

  int deckCount, dealCount, pileCount;

  if (setup.variation == SetupData::Mini)
  {
    deckCount = 1;
    dealCount = 4;
    pileCount = 7;
  }
  else if (setup.variation == SetupData::Normal)
  {
    deckCount = 2;
    dealCount = 5;
    pileCount = 10;
  }
  else // SetupData::Custom: custom variation
  {
    deckCount = setup.deck_count;
    dealCount = setup.deal_count;
    pileCount = setup.pile_count;
  }

  if (pileCount > rankCount * suitCount * deckCount)
    pileCount = rankCount * suitCount * deckCount;
  int finalCount = suitCount * deckCount;
  deck = new Deck(rankCount, suitCount, deckCount);
  tableau = new Tableau(*deck, pileCount, finalCount, dealCount);
}

/** Paint all pieces of the game */
void Game::paint()
{
  const cBitmap* bm = osd->GetBitmap(0);
  int x1 = bm->X0();
  int x2 = bm->X0() + bm->Width() - 1;
  int y1 = bm->Y0();
  int y2 = bm->Y0() + bm->Height() - 1;

  // Save and restore palette to reduce flickering
  cPalette savePalette(*bm);
  osd->DrawRectangle(x1, y1, x2, y2, colors.background);
  osd->SetPalette(savePalette, 0);

  // Paint red frame
  osd->DrawRectangle(x1,     y1,     x2,     y1 + 1, colors.osd_frame);
  osd->DrawRectangle(x1,     y1,     x1 + 1, y2,     colors.osd_frame);
  osd->DrawRectangle(x1,     y2 - 1, x2,     y2,     colors.osd_frame);
  osd->DrawRectangle(x2 - 1, y1,     x2,     y2,     colors.osd_frame);

  if (!setup.hide_toprow || status == cursorOnPack)
  {
    paintPack();

    unsigned int f;
    for (f = 0; f < tableau->finals.size(); ++f)
      if (tableau->finals[f]->empty())
        paintFinal(f);
    for (f = 0; f < tableau->finals.size(); ++f)
      if (!tableau->finals[f]->empty())
        paintFinal(f);
  }

  unsigned int p;
  for (p = 0; p < tableau->piles.size(); ++p)
    if (tableau->piles[p]->empty())
      paintPile(p);
  for (p = 0; p < tableau->piles.size(); ++p)
    if (!tableau->piles[p]->empty())
      paintPile(p);

  if (infoText)
  {
    info->text(infoText);
    osd->DrawBitmap(x1 + (x2 - x1 + 1 - info->Width()) / 2, y1 + 10, *info);
    infoText = NULL;
  }
  osd->Flush();
}

/** Paint the pack */
void Game::paintPack()
{
  const cBitmap* bm = osd->GetBitmap(0);
  int packX = bm->X0() + 1;
  int packY = bm->Y0() + 1;
  if (tableau->pack->empty())
    paintFrame(packX, packY);
  else
    paintBack(packX, packY);
  if (status == cursorOnPack)
    paintCursor(packX, packY);
}

/** Paint a final heap */
void Game::paintFinal(unsigned int f)
{
  const cBitmap* bm = osd->GetBitmap(0);
  int offset = tableau->piles.size() - tableau->finals.size();
  int finalX = bm->X0() + 1 + ((f + offset) * (bm->Width() - 1 - cardWidth)) /
                              (tableau->piles.size() - 1);
  int finalY = bm->Y0() + 1;
  if (tableau->finals[f]->empty())
    paintFrame(finalX, finalY);
  else
    paintCard(finalX, finalY, tableau->finals[f]->top());
}

/** Paint a pile */
void Game::paintPile(unsigned int p)
{
  const cBitmap* bm = osd->GetBitmap(0);
  int pileX = bm->X0() + 1 + (p * (bm->Width() - 1 - cardWidth)) /
                             (tableau->piles.size() - 1);
  int pileY = bm->Y0() + 1;
  if (!setup.hide_toprow || status == cursorOnPack)
    pileY += cardHeight + 1;
  paintFrame(pileX, pileY);

  int count = tableau->piles[p]->count();
  int closed = count - tableau->piles[p]->open();
  int unselected = count - tableau->piles[p]->selected();
  int dist = cardHeight / 6;
  if (pileY + (count + 1) * dist > bm->Y0() + bm->Height())
    dist = (bm->Y0() + bm->Height() - pileY) / (count + 1);

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
void Game::paintCursor(int x, int y)
{
  int x0 = x + (cardWidth - cursorWidth) / 2;
  int y0 = y + (cardHeight - cursorHeight) / 2;
  tColor color = colors.inactive_cursor;
  if (status == selectedPile)
    color = colors.active_cursor;
  for (x = 0; x < cursorWidth; ++x)
    for (y = 0; y < cursorHeight; ++y)
      if (cursor->Color(*cursor->Data(x, y)) != clrTransparent)
        osd->DrawRectangle(x0 + x, y0 + y, x0 + x, y0 + y, color);
}

/** Paint an empty card frame */
void Game::paintFrame(int x, int y)
{
  osd->DrawBitmap(x, y, *frame);
}

/** Paint a card back */
void Game::paintBack(int x, int y)
{
  osd->DrawBitmap(x, y, *back);
}

/** Paint a card */
void Game::paintCard(int x, int y, const Card& card)
{
  osd->DrawBitmap(x, y, *cards[card.suit][card.rank]);
}

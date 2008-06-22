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
 * $Id: game.h 97 2007-09-24 22:29:48Z tom $
 */

#ifndef VDR_SPIDER_GAME_H
#define VDR_SPIDER_GAME_H

#include "spider.h"
namespace Spider { class Deck; class Tableau; class Card; }
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>


namespace SpiderPlugin
{
  class SetupData;
  class Bitmap;


  //--- class SpiderPlugin::Game -----------------------------------------------

  /** Main menu of the plugin */
  class Game : public cOsdObject
  {
    const SetupData& setup;
    const char* confdir;
    cOsd* osd;
    Bitmap* info;
    const char* infoText;
    Spider::Deck*& deck;
    Spider::Tableau*& tableau;
    unsigned int currentPile;
    enum { cursorOnPile, selectedPile, cursorOnPack, gameOver } status;
    struct Colors
    {
      tColor background;
      tColor osd_frame;
      tColor card_frame;
      tColor card_frame_bg;
      tColor inactive_cursor;
      tColor active_cursor;
    };
    Colors colors;
    static const Colors full_colors;
    static const Colors reduced_colors;

  public:

    /** Constructor */
    Game(const SetupData& setup, const char* confdir,
         Spider::Deck*& deck, Spider::Tableau*& tableau);

    /** Destructor */
    virtual ~Game();

    /** Display the game on the OSD */
    virtual void Show();

    /** Process user events */
    virtual eOSState ProcessKey(eKeys key);

  private:

    /** Start a new game */
    void start();

    /** Paint all pieces of the game */
    void paint();

    /** Paint the cursor onto a card */
    void paintCursor(int x, int y);

    /** Paint the pack */
    void paintPack();

    /** Paint a final heap */
    void paintFinal(unsigned int f);

    /** Paint a pile */
    void paintPile(unsigned int p);

    /** Paint an empty card frame */
    void paintFrame(int x, int y);

    /** Paint a card back */
    void paintBack(int x, int y);

    /** Paint a card */
    void paintCard(int x, int y, const Spider::Card& card);
  };

} // namespace SpiderPlugin

#endif // VDR_SPIDER_GAME_H

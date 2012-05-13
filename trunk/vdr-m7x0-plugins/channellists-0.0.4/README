This is a "plugin" for the Video Disk Recorder (VDR).

Written by:                  Marc Wernecke <zulu@zulu-entertainment.de>

Project's homepage:          http://www.zulu-entertainment.de

Latest version available at: http://www.zulu-entertainment.de

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
See the file COPYING for more information.


Description:
------------
Channellists - A VDR Plugin to manage your Channellists

Based on vdr-channelswitcher-0.0.1b but rebuild from scratch.
The classes for saving and restore the timers are from the vdr-setup-0.3.1.
Also I took some inspiration from other plugins like extrecmenu...


Requirements:
-------------
VDR >= 1.4.5


Installation:
-------------
Theres nothing special about it. Install it like any other standard
plugin (unpack, soft link, make plugins).


Usage:
-----
With this plugin you can manage your channellist collection 
The plugin shows a list of all channellists stored in /vdrconfdir/plugins/channellists
These channellists have named like this:
channels.conf.cable   channels.conf.sat   channels.conf.foo   channels.conf.bar

Menu      Options   Are:
------------------------
Ok        Reload    save timers and rewrite the current channels.conf with the selected list
Red       Rename    open a editmenu for renaming the selected list
          Skript    execute a external script
Green     Copy      open a editmenu for copying the selected list
          Skript    execute a external script
Yellow    Delete    delete the selected list
Blue      Save      open a editmenu for saving the current channels.conf
Back      Exit      leave the menu

Commandline Option:
-------------------
-s SCRIPT,   --script=SCRIPT      path to update script (default: channellists-update.sh)

Setup Options:
--------------
- Hide main menu entry:    Hides the plugin main menu entry
- Action after switch:     ...ing channellist can be Channels, Menu, back or none
- Button Red:              can be Rename or Script
- Button Green:            can be Copy or Script

SVDRP:
------
The plugin provides a svdrp command LOAD with an optional argument /path/to/channels.conf
With no argument LOAD reloads the current channels.conf

svdrpsend.pl plug channellists load
svdrpsend.pl plug channellists load /path/to/my/channellist




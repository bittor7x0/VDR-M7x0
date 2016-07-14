/*
 * i18n.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.h,v 1.1 2003/11/11 15:06:58 opj Exp $
 */

#ifndef __GAMES_I18N_H
#define __GAMES_I18N_H

#include <config.h> // only for VDRVERSNUM
#include <i18n.h>

const tI18nPhrase Phrases[] =
{
  { "OSD Games Collection", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "Colección Juegos OSD", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    "", // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#endif
#endif
  },
  { "Games", // English
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // suomi
    "", // TODO
    "Juegos", // Español
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302
    "", // TODO
#if VDRVERSNUM >= 10307
    "", // TODO
#endif
#endif
  },
  { "X Position", // English
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // suomi
    "", // TODO
    "Posición X", // Español
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302  
    "", // TODO               
#if VDRVERSNUM >= 10307  
    "", // TODO                 
#endif  
#endif  
  },
  { "Y Position", // English
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // suomi
    "", // TODO
    "Posición Y", // Español
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302  
    "", // TODO               
#if VDRVERSNUM >= 10307  
    "", // TODO                 
#endif  
#endif  
  },
  { "Computer Skill", // English
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // suomi
    "", // TODO
    "Habilidad del ordenador", // Español
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302  
    "", // TODO               
#if VDRVERSNUM >= 10307  
    "", // TODO                 
#endif  
#endif  
  },
  { "Key Repeat", // English
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // suomi
    "", // TODO
    "Botón repetir", // Español
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302  
    "", // TODO               
#if VDRVERSNUM >= 10307  
    "", // TODO                 
#endif  
#endif  
  },
  { "OSD Size", // English
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // suomi
    "", // TODO
    "Tamaño OSD", // Español
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302  
    "", // TODO               
#if VDRVERSNUM >= 10307  
    "", // TODO                 
#endif  
#endif  
  },
  { "On", // English
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // suomi
    "", // TODO
    "Activar", // Español
    "", // TODO
    "", // TODO  
    "", // TODO   
    "", // TODO
    "", // TODO   
#if VDRVERSNUM >= 10302  
    "", // TODO               
#if VDRVERSNUM >= 10307  
    "", // TODO                 
#endif  
#endif  
  },
  { "Off", // English
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // suomi
    "", // TODO
    "Desactivar", // Español
    "", // TODO
    "", // TODO  
    "", // TODO   
    "", // TODO
    "", // TODO   
#if VDRVERSNUM >= 10302  
    "", // TODO               
#if VDRVERSNUM >= 10307  
    "", // TODO                 
#endif  
#endif  
  },
  { NULL }
};

#endif // __GAMES_I18N_H

/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 1.3 2002/06/23 13:05:59 kls Exp $
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Page",       //English               
    "Seite",      //German
    "",           //Slovenian
    "",     //Italian
    "",           //Dutch
    "",           //Portuguese
    "Page",       //French
    "",           //Norwegian
    "",      //Finnish
    "",           //Polish
    "Página",     //Spanish
    "",           //Greek
    "",           //Swedish
    "",           //Romanian
    "",           //Hugarian
    "",     //Catalan
    "",   //Russian
  },              
  { "not found until now",      //English
    "noch nicht gefunden", //German
    "",               //Slovenian
    "",    //Italian
    "",               //Dutch
    "",               //Portuguese
    "pas trouvé",     //French
    "",               //Norwegian
    "",       //Finnish
    "",               //Polish
    "no encontrada",  //Spanish
    "",               //Greek
    "",               //Swedish
    "",               //Romanian
    "",               //Hugarian
    "",     //Catalan
    "",     //Russian
  },
  { "Teletext (OSD)", //English
    "Videotext (OSD)", //German
    "",               //Slovenian
    "", //Italian
    "",               //Dutch
    "",               //Portuguese
    "Teletext (OSD)", //French
    "",               //Norwegian
    "",      //Finnish
    "",               //Polish
    "Teletexto (OSD)",//Spanish
    "",               //Greek
    "",               //Swedish
    "",               //Romanian
    "",               //Hugarian
    "", //Catalan
    "",      //Russian
  },
  { "Zoom",           //English
    "Vergrößern",     //German
    "",               //Slovenian
    "",           //Italian
    "",               //Dutch
    "",               //Portuguese
    "Zoom",           //French
    "",               //Norwegian
    "",       //Finnish
    "",               //Polish
    "Zoom",           //Spanish
    "",               //Greek
    "",               //Swedish
    "",               //Romanian
    "",               //Hugarian
    "",           //Catalan
    "",      //Russian
  },
  { "Half page",      //English
    "Halbe Seite",    //German
    "",               //Slovenian
    "",   //Italian
    "",               //Dutch
    "",               //Portuguese
    "Demi-Page",      //French
    "",               //Norwegian
    "",  //Finnish
    "",               //Polish
    "Media página",   //Spanish
    "",               //Greek
    "",               //Swedish
    "",               //Romanian
    "",               //Hugarian
    "",   //Catalan
    "",   //Russian   
  },
  { "Change channel",    //English
    "Kanal wechseln",    //German
    "",                  //Slovenian
    "",     //Italian
    "",                  //Dutch
    "",                  //Portuguese
    "Changer la chaîne", //French
    "",                  //Norwegian
    "",    //Finnish
    "",                  //Polish
    "Cambio cadena",     //Spanish
    "",                  //Greek
    "",                  //Swedish
    "",                  //Romanian
    "",                  //Hugarian
    "",      //Catalan
    "", //Russian
  },
  { "Jump to...",        //English
    "Springe zu...",     //German
    "",                  //Slovenian
    "",        //Italian
    "",                  //Dutch
    "",                  //Portuguese
    "Aller à...",        //French
    "",                  //Norwegian
    "", //Finnish
    "",                  //Polish
    "Salta a...",        //Spanish
    "",                  //Greek
    "",                  //Swedish
    "",                  //Romanian
    "",                  //Hugarian
    "",        //Catalan
    "",      //Russian
  },
  { "Background transparency",   //English
    "Hintergrund-Transparenz",   //German
    "",                          //Slovenian
    "",  //Italian
    "",                          //Dutch
    "",                          //Portuguese
    "Fond transparent",          //French
    "",                          //Norwegian
    "",      //Finnish
    "",                          //Polish
    "Transparencia del fondo",   //Spanish
    "",                          //Greek
    "",                          //Swedish
    "",                          //Romanian
    "",                          //Hugarian
    "",    //Catalan
    "", //Russian
  },
  { "Red key",            //English
    "Rote Taste",         //German
    "",                   //Slovenian
    "",        //Italian
    "",                   //Dutch
    "",                   //Portuguese
    "Touche rouge",       //French
    "",                   //Norwegian
    "",   //Finnish
    "",                   //Polish
    "Tecla roja",         //Spanish
    "",                   //Greek
    "",                   //Swedish
    "",                   //Romanian
    "",                   //Hugarian
    "",     //Catalan
    "",     //Russian
  },
  { "  Page number",      //English
    "  Seitenzahl",       //German
    "",                   //Slovenian
    "", //Italian
    "",                   //Dutch
    "",                   //Portuguese
    "  Nombre de pages",  //French
    "",                   //Norwegian
    "",       //Finnish
    "",                   //Polish
    "  Número de página", //Spanish
    "",                   //Greek
    "",                   //Swedish
    "",                   //Romanian
    "",                   //Hugarian
    "", //Catalan
    "",   //Russian
  },
  { "Green key",          //English
    "Grüne Taste",        //German
    "",                   //Slovenian
    "",        //Italian
    "",                   //Dutch
    "",                   //Portuguese
    "Touche verte",       //French
    "",                   //Norwegian
    "",     //Finnish
    "",                   //Polish
    "Tecla verde",        //Spanish
    "",                   //Greek
    "",                   //Swedish
    "",                   //Romanian
    "",                   //Hugarian
    "",        //Catalan
    "",     //Russian
  },
  { "Yellow key",         //English
    "Gelbe Taste",        //German
    "",                   //Slovenian
    "",       //Italian
    "",                   //Dutch
    "",                   //Portuguese
    "Touche jaune",       //French
    "",                   //Norwegian
    "",  //Finnish
    "",                   //Polish
    "Tecla amarilla",     //Spanish
    "",                   //Greek
    "",                   //Swedish
    "",                   //Romanian
    "",                   //Hugarian
    "",        //Catalan
    "",      //Russian
  },
  { "Blue key",           //English
    "Blaue Taste",        //German
    "",                   //Slovenian
    "",          //Italian
    "",                   //Dutch
    "",                   //Portuguese
    "Touche bleue",       //French
    "",                   //Norwegian
    "",    //Finnish
    "",                   //Polish
    "Tecla azul",         //Spanish
    "",                   //Greek
    "",                   //Swedish
    "",                   //Romanian
    "",                   //Hugarian
    "",        //Catalan
    "",       //Russian
  },
  { "Channel (press OK): ",        //English
    "Sender (OK drücken): ",       //German
    "",                            //Slovenian
    "",         //Italian
    "",                            //Dutch
    "",                            //Portuguese
    "Chaine (Appuyer sur OK): ",   //French
    "",                            //Norwegian
    "",          //Finnish
    "",                            //Polish
    "Canal (pulse OK):",           //Spanish
    "",                            //Greek
    "",                            //Swedish
    "",                            //Romanian
    "",                            //Hugarian
    "",           //Catalan
    "",          //Russian
  },
  { "Show clock",            //English
    "Uhr anzeigen",          //German
    "",                      //Slovenian
    "",   //Italian
    "",                      //Dutch
    "",                      //Portuguese
    "Afficher l'heure",      //French
    "",                      //Norwegian
    "",           //Finnish
    "",                      //Polish
    "Visualiza el reloj",    //Spanish
    "",                      //Greek
    "",                      //Swedish
    "",                      //Romanian
    "",                      //Hugarian
    "", //Catalan
    "",       //Russian
  },
  { "Setup$Suspend receiving",    //English
    "Empfang unterbrechen",       //German
    "",                           //Slovenian
    "",         //Italian
    "",                           //Dutch
    "",                           //Portuguese
    "Suspension de la réception", //French
    "",                           //Norwegian
    "",  //Finnish
    "",                           //Polish
    "Suspende la recepción",      //Spanish
    "",                           //Greek
    "",                           //Swedish
    "",                           //Romanian
    "",                           //Hugarian
    "",         //Catalan
    "",        //Russian
  },
  { "Suspend receiving",          //English
    "Empfang unterbrechen",       //German
    "",                           //Slovenian
    "",         //Italian
    "",                           //Dutch
    "",                           //Portuguese
    "Suspension de la réception", //French
    "",                           //Norwegian
    "",  //Finnish
    "",                           //Polish
    "Suspende la recepción",      //Spanish
    "",                           //Greek
    "",                           //Swedish
    "",                           //Romanian
    "",                           //Hugarian
    "",         //Catalan
    "",        //Russian
  },
  { "Switch background",        //English
    "Hintergrund ändern",       //German
    "",                         //Slovenian
    "",            //Italian
    "",                         //Dutch
    "",                         //Portuguese
    "Change le fond d'écran",   //French
    "",                         //Norwegian
    "",            //Finnish
    "",                         //Polish
    "Cambia el fondo",          //Spanish
    "",                         //Greek
    "",                         //Swedish
    "",                         //Romanian
    "",                         //Hugarian
    "",           //Catalan
    "",         //Russian
  },
  { "Auto-update pages",                  //English
    "Seiten aktualisieren",               //German
    "",                                   //Slovenian
    "",    //Italian
    "",                                   //Dutch
    "",                                   //Portuguese
    "Mise a jour des pages",              //French
    "",                                   //Norwegian
    "",         //Finnish
    "",   //Polish
    "",                                   //Spanish
    "",                                   //Greek
    "",                                   //Swedish
    "",                                   //Romanian
    "",                                   //Hugarian
    "",   //Catalan
    "",             //Russian
  },
  { "OSD height",       //English
    "OSD-Höhe",         //German
    "",                 //Slovenian
    "",      //Italian
    "",                 //Dutch
    "",                 //Portuguese
    "OSD Hauteur",      //French
    "",                 //Norwegian
    "",          //Finnish
    "",                 //Polish
    "Altura OSD",       //Spanish
    "",                 //Greek
    "",                 //Swedish
    "",                 //Romanian
    "",                 //Hugarian
    "",       //Catalan
    "",      //Russian
  },
  { "OSD width",        //English
    "OSD-Breite",       //German
    "",                 //Slovenian
    "",    //Italian
    "",                 //Dutch
    "",                 //Portuguese
    "OSD Largeur",      //French
    "",                 //Norwegian
    "",           //Finnish
    "",                 //Polish
    "Anchura OSD",      //Spanish
    "",                 //Greek
    "",                 //Swedish
    "",                 //Romanian
    "",                 //Hugarian
    "",     //Catalan
    "",      //Russian
  },
  { "OSD horizontal align",      //English
    "OSD horizontale Anordnung", //German
    "",                          //Slovenian
    "",                          //Italian
    "",                          //Dutch
    "",                          //Portuguese
    "",                          //French
    "",                          //Norwegian
    "",             //Finnish
    "",                          //Polish
    "",                          //Spanish
    "",                          //Greek
    "",                          //Swedish
    "",                          //Romanian
    "",                          //Hugarian
    "",                          //Catalan
    "",                          //Russian
  },
  { "OSD vertical align",        //English
    "OSD vertikale Anordnung",   //German
    "",                          //Slovenian
    "",                          //Italian
    "",                          //Dutch
    "",                          //Portuguese
    "",                          //French
    "",                          //Norwegian
    "",             //Finnish
    "",                          //Polish
    "",                          //Spanish
    "",                          //Greek
    "",                          //Swedish
    "",                          //Romanian
    "",                          //Hugarian
    "",                          //Catalan
    "",                          //Russian
  },
  { "Displays teletext on the OSD",           //English
    "Zeigt den Videotext auf dem OSD an",     //German
    "",                                       //Slovenian
    "",                                       //Italian
    "",                                       //Dutch
    "",                                       //Portuguese
    "",                                       //French
    "",                                       //Norwegian
    "",                        //Finnish
    "",                                       //Polish
    "Visualiza el teletexto en el OSD",       //Spanish
    "",                                       //Greek
    "",                                       //Swedish
    "",                                       //Romanian
    "",                                       //Hugarian
    "",        //Catalan
    "",                //Russian
  },
  { NULL }
  };

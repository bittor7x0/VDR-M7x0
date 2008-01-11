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
    "Pagina",     //Italian
    "",           //Dutch
    "",           //Portuguese
    "Page",       //French
    "",           //Norwegian
    "Sivua",      //Finnish
    "",           //Polish
    "P·gina",     //Spanish
    "",           //Greek
    "",           //Swedish
    "",           //Romanian
    "",           //Hugarian
    "P‡gina",     //Catalan
    "¡‚‡–›ÿÊ–",   //Russian
  },              
  { "not found until now",      //English
    "noch nicht gefunden", //German
    "",               //Slovenian
    "non trovata",    //Italian
    "",               //Dutch
    "",               //Portuguese
    "pas trouvÈ",     //French
    "",               //Norwegian
    "ei lˆydy",       //Finnish
    "",               //Polish
    "no encontrada",  //Spanish
    "",               //Greek
    "",               //Swedish
    "",               //Romanian
    "",               //Hugarian
    "no trobada",     //Catalan
    "›’ ›–Ÿ‘’›–",     //Russian
  },
  { "Teletext (OSD)", //English
    "Videotext (OSD)", //German
    "",               //Slovenian
    "Teletext (OSD)", //Italian
    "",               //Dutch
    "",               //Portuguese
    "Teletext (OSD)", //French
    "",               //Norwegian
    "Teksti-TV",      //Finnish
    "",               //Polish
    "Teletexto (OSD)",//Spanish
    "",               //Greek
    "",               //Swedish
    "",               //Romanian
    "",               //Hugarian
    "Teletext (OSD)", //Catalan
    "¬’€’‚’⁄·‚",      //Russian
  },
  { "Zoom",           //English
    "Vergrˆﬂern",     //German
    "",               //Slovenian
    "Zoom",           //Italian
    "",               //Dutch
    "",               //Portuguese
    "Zoom",           //French
    "",               //Norwegian
    "Suurenna",       //Finnish
    "",               //Polish
    "Zoom",           //Spanish
    "",               //Greek
    "",               //Swedish
    "",               //Romanian
    "",               //Hugarian
    "Zoom",           //Catalan
    "„“’€ÿÁÿ‚Ï",      //Russian
  },
  { "Half page",      //English
    "Halbe Seite",    //German
    "",               //Slovenian
    "Mezza pagina",   //Italian
    "",               //Dutch
    "",               //Portuguese
    "Demi-Page",      //French
    "",               //Norwegian
    "Puolikas sivu",  //Finnish
    "",               //Polish
    "Media p·gina",   //Spanish
    "",               //Greek
    "",               //Swedish
    "",               //Romanian
    "",               //Hugarian
    "Mitja p‡gina",   //Catalan
    "ﬂﬁ€-·‚‡–›ÿÊÎ",   //Russian   
  },
  { "Change channel",    //English
    "Kanal wechseln",    //German
    "",                  //Slovenian
    "Cambio canale",     //Italian
    "",                  //Dutch
    "",                  //Portuguese
    "Changer la chaÓne", //French
    "",                  //Norwegian
    "Vaihda kanavaa",    //Finnish
    "",                  //Polish
    "Cambio cadena",     //Spanish
    "",                  //Greek
    "",                  //Swedish
    "",                  //Romanian
    "",                  //Hugarian
    "Canvi cadena",      //Catalan
    "ﬂ’‡’⁄€ÓÁÿ‚Ï ⁄–›–€", //Russian
  },
  { "Jump to...",        //English
    "Springe zu...",     //German
    "",                  //Slovenian
    "Salta a...",        //Italian
    "",                  //Dutch
    "",                  //Portuguese
    "Aller ‡...",        //French
    "",                  //Norwegian
    "Siirry sivulle...", //Finnish
    "",                  //Polish
    "Salta a...",        //Spanish
    "",                  //Greek
    "",                  //Swedish
    "",                  //Romanian
    "",                  //Hugarian
    "Salta a...",        //Catalan
    "ﬂ’‡’Ÿ‚ÿ ⁄...",      //Russian
  },
  { "Background transparency",   //English
    "Hintergrund-Transparenz",   //German
    "",                          //Slovenian
    "Trasparenza dello sfondo",  //Italian
    "",                          //Dutch
    "",                          //Portuguese
    "Fond transparent",          //French
    "",                          //Norwegian
    "Taustan l‰pin‰kyvyys",      //Finnish
    "",                          //Polish
    "Transparencia del fondo",   //Spanish
    "",                          //Greek
    "",                          //Swedish
    "",                          //Romanian
    "",                          //Hugarian
    "TransparËncia del fons",    //Catalan
    "¡‚’ﬂ’›Ï ﬂ‡ﬁ◊‡–Á›ﬁ·‚ÿ ‰ﬁ›–", //Russian
  },
  { "Red key",            //English
    "Rote Taste",         //German
    "",                   //Slovenian
    "Tasto Rosso",        //Italian
    "",                   //Dutch
    "",                   //Portuguese
    "Touche rouge",       //French
    "",                   //Norwegian
    "Punainen n‰pp‰in",   //Finnish
    "",                   //Polish
    "Tecla roja",         //Spanish
    "",                   //Greek
    "",                   //Swedish
    "",                   //Romanian
    "",                   //Hugarian
    "Tecla vermella",     //Catalan
    "∫‡–·›–Ô ⁄›ﬁﬂ⁄–",     //Russian
  },
  { "  Page number",      //English
    "  Seitenzahl",       //German
    "",                   //Slovenian
    "  Numero di pagina", //Italian
    "",                   //Dutch
    "",                   //Portuguese
    "  Nombre de pages",  //French
    "",                   //Norwegian
    "  Sivunumero",       //Finnish
    "",                   //Polish
    "  N˙mero de p·gina", //Spanish
    "",                   //Greek
    "",                   //Swedish
    "",                   //Romanian
    "",                   //Hugarian
    "  Nombre de p‡gina", //Catalan
    "  Ωﬁ‹’‡ ·‚‡–›ÿÊÎ",   //Russian
  },
  { "Green key",          //English
    "Gr¸ne Taste",        //German
    "",                   //Slovenian
    "Tasto Verde",        //Italian
    "",                   //Dutch
    "",                   //Portuguese
    "Touche verte",       //French
    "",                   //Norwegian
    "Vihre‰ n‰pp‰in",     //Finnish
    "",                   //Polish
    "Tecla verde",        //Spanish
    "",                   //Greek
    "",                   //Swedish
    "",                   //Romanian
    "",                   //Hugarian
    "Tecla verda",        //Catalan
    "∑’€Ò›–Ô ⁄›ﬁﬂ⁄–",     //Russian
  },
  { "Yellow key",         //English
    "Gelbe Taste",        //German
    "",                   //Slovenian
    "Tasto Giallo",       //Italian
    "",                   //Dutch
    "",                   //Portuguese
    "Touche jaune",       //French
    "",                   //Norwegian
    "Keltainen n‰pp‰in",  //Finnish
    "",                   //Polish
    "Tecla amarilla",     //Spanish
    "",                   //Greek
    "",                   //Swedish
    "",                   //Romanian
    "",                   //Hugarian
    "Tecla groga",        //Catalan
    "∂Ò€‚–Ô ⁄›ﬁﬂ⁄–",      //Russian
  },
  { "Blue key",           //English
    "Blaue Taste",        //German
    "",                   //Slovenian
    "Tasto Blu",          //Italian
    "",                   //Dutch
    "",                   //Portuguese
    "Touche bleue",       //French
    "",                   //Norwegian
    "Sininen n‰pp‰in",    //Finnish
    "",                   //Polish
    "Tecla azul",         //Spanish
    "",                   //Greek
    "",                   //Swedish
    "",                   //Romanian
    "",                   //Hugarian
    "Tecla blava",        //Catalan
    "¡ÿ›ÔÔ ⁄›ﬁﬂ⁄–",       //Russian
  },
  { "Channel (press OK): ",        //English
    "Sender (OK dr¸cken): ",       //German
    "",                            //Slovenian
    "Canale (premere OK)",         //Italian
    "",                            //Dutch
    "",                            //Portuguese
    "Chaine (Appuyer sur OK): ",   //French
    "",                            //Norwegian
    "Kanava (paina OK):",          //Finnish
    "",                            //Polish
    "Canal (pulse OK):",           //Spanish
    "",                            //Greek
    "",                            //Swedish
    "",                            //Romanian
    "",                            //Hugarian
    "Canal (premi OK):",           //Catalan
    "∫–›–€ (Ω–÷‹ÿ‚’ æ∫)",          //Russian
  },
  { "Show clock",            //English
    "Uhr anzeigen",          //German
    "",                      //Slovenian
    "Visualizza la hora",   //Italian
    "",                      //Dutch
    "",                      //Portuguese
    "Afficher l'heure",      //French
    "",                      //Norwegian
    "N‰yt‰ kello",           //Finnish
    "",                      //Polish
    "Visualiza el reloj",    //Spanish
    "",                      //Greek
    "",                      //Swedish
    "",                      //Romanian
    "",                      //Hugarian
    "Visualitza l'hora", //Catalan
    "øﬁ⁄–◊Î“–‚Ï Á–·Î",       //Russian
  },
  { "Setup$Suspend receiving",    //English
    "Empfang unterbrechen",       //German
    "",                           //Slovenian
    "Sospendi ricezione",         //Italian
    "",                           //Dutch
    "",                           //Portuguese
    "Suspension de la rÈception", //French
    "",                           //Norwegian
    "Keskeyt‰ vastaanottaminen",  //Finnish
    "",                           //Polish
    "Suspende la recepciÛn",      //Spanish
    "",                           //Greek
    "",                           //Swedish
    "",                           //Romanian
    "",                           //Hugarian
    "SuspËn la recepciÛ",         //Catalan
    "ø‡ÿﬁ·‚–›ﬁ“ÿ‚Ï ﬂ‡ÿÒ‹",        //Russian
  },
  { "Suspend receiving",          //English
    "Empfang unterbrechen",       //German
    "",                           //Slovenian
    "Sospendi ricezione",         //Italian
    "",                           //Dutch
    "",                           //Portuguese
    "Suspension de la rÈception", //French
    "",                           //Norwegian
    "Keskeyt‰ vastaanottaminen",  //Finnish
    "",                           //Polish
    "Suspende la recepciÛn",      //Spanish
    "",                           //Greek
    "",                           //Swedish
    "",                           //Romanian
    "",                           //Hugarian
    "SuspËn la recepciÛ",         //Catalan
    "ﬂ‡ÿﬁ·‚–›ﬁ“ÿ‚Ï ﬂ‡ÿÒ‹",        //Russian
  },
  { "Switch background",        //English
    "Hintergrund ‰ndern",       //German
    "",                         //Slovenian
    "Cambia sfondo",            //Italian
    "",                         //Dutch
    "",                         //Portuguese
    "Change le fond d'Ècran",   //French
    "",                         //Norwegian
    "Vaihda tausta",            //Finnish
    "",                         //Polish
    "Cambia el fondo",          //Spanish
    "",                         //Greek
    "",                         //Swedish
    "",                         //Romanian
    "",                         //Hugarian
    "Canvia el fons",           //Catalan
    "›’ﬂ‡ﬁ◊‡–Á›ÎŸ ‰ﬁ›",         //Russian
  },
  { "Auto-update pages",                  //English
    "Seiten aktualisieren",               //German
    "",                                   //Slovenian
    "Aggiornamento pagina automatico",    //Italian
    "",                                   //Dutch
    "",                                   //Portuguese
    "Mise a jour des pages",              //French
    "",                                   //Norwegian
    "Automaattinen sivup‰ivitys",         //Finnish
    "ActualizaciÛn p·ginas autom·tica",   //Polish
    "",                                   //Spanish
    "",                                   //Greek
    "",                                   //Swedish
    "",                                   //Romanian
    "",                                   //Hugarian
    "ActualitzaciÛ p‡gines autom‡tica",   //Catalan
    "∞“‚ﬁﬁ—›ﬁ“€’›ÿ’ ·‚‡–›ÿÊ",             //Russian
  },
  { "OSD height",       //English
    "OSD-Hˆhe",         //German
    "",                 //Slovenian
    "Altezza OSD",      //Italian
    "",                 //Dutch
    "",                 //Portuguese
    "OSD Hauteur",      //French
    "",                 //Norwegian
    "Korkeus",          //Finnish
    "",                 //Polish
    "Altura OSD",       //Spanish
    "",                 //Greek
    "",                 //Swedish
    "",                 //Romanian
    "",                 //Hugarian
    "Altura OSD",       //Catalan
    "≤Î·ﬁ‚– ‹’›Ó",      //Russian
  },
  { "OSD width",        //English
    "OSD-Breite",       //German
    "",                 //Slovenian
    "Larghezza OSD",    //Italian
    "",                 //Dutch
    "",                 //Portuguese
    "OSD Largeur",      //French
    "",                 //Norwegian
    "Leveys",           //Finnish
    "",                 //Polish
    "Anchura OSD",      //Spanish
    "",                 //Greek
    "",                 //Swedish
    "",                 //Romanian
    "",                 //Hugarian
    "Ampl‡ria OSD",     //Catalan
    "»ÿ‡ÿ›– ‹’›Ó",      //Russian
  },
  { "OSD horizontal align",      //English
    "OSD horizontale Anordnung", //German
    "",                          //Slovenian
    "",                          //Italian
    "",                          //Dutch
    "",                          //Portuguese
    "",                          //French
    "",                          //Norwegian
    "Pystykeskitys",             //Finnish
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
    "Vaakakeskitys",             //Finnish
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
    "Teksti-TV (OSD)",                        //Finnish
    "",                                       //Polish
    "Visualiza el teletexto en el OSD",       //Spanish
    "",                                       //Greek
    "",                                       //Swedish
    "",                                       //Romanian
    "",                                       //Hugarian
    "Visualitza el teletext en l'OSD",        //Catalan
    "øﬁ⁄–◊ ‚’€’‚’⁄·‚– “ ‹’›Ó",                //Russian
  },
  { NULL }
  };

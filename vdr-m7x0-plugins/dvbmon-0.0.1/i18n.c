/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id:
 */

#include "i18n.h"

 const tI18nPhrase Phrases[] = {
  { "DVB Monitor", //EN
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Monitor DVB", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Monitor signal of DVB Channels", //EN
    "Zeigt Empfangsqualität von DVB Kanälen an.",// DE
    "", // Slovenski
    "Ricerca Transponders di Canali DVB",// IT
    "Doorzoek Transponders naar DVB kanalen", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Kanavahaku DVB-transpondereille",// FI
    "", // Polski
    "Monitor de señal para canales DVB", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "¿ŞØáÚ ÚĞİĞÛŞÒ", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button$Done",
    "Fertig", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Aceptar", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button$Freq+",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Freq+", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button$Freq-",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Freq-", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button$Auto",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Auto", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button$Manually",
    "Manuell", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Manual", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Cycle time (1s..60s)",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Tiempo de refresco (1s..60s)", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Tuner 1: %6.1fMHz  %s",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Sintonizador 1: %6.1fMHz  %s", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "empty channel list",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "lista de canales vacía", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Tuner 2 not available!",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "¡Sintonizador 2 no disponible!", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Empty channel list?",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "¿Lista de canales vacía?", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Frequency  Q curr.    Q prev.",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Frecuencia  C actu.    C prev.", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Tuner 2: %6.1fMHz  %s",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Sintonizador 2: %6.1fMHz  %s", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Signal",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "Señal", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "BER",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "BER", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "SNR",
    "", // DE
    "", // Slovenski
    "", // IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // FI
    "", // Polski
    "SNR", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { NULL }
  };


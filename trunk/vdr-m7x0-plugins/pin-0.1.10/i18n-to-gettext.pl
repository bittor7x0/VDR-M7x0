#!/usr/bin/perl -w

# Convert i18n texts to gettext
#
# Extracts all texts from the file i18n.c and creates language
# specific *.po files.
#
# Usage: i18n-to-gettext.pl
#
# See the main source file 'vdr.c' for copyright information and
# how to reach the author.
#
# $Id: i18n-to-gettext.pl 1.4 2007/08/24 13:19:34 kls Exp $

# How to convert an actual plugin:
#
# - If your code contains something like
#
#   const char *Text = "Some text";
#   ...
#   Function(tr(Text));
#
#   you need to enclose the text constant in trNOOP(...), as in
#
#   const char *Text = trNOOP("Some text");
#
#   The actual usage of the tr() macro remains unchanged.
#
# - If you have comments following the texts in your i18n.c file
#   that should be seen by the translators, move them from the
#   i18n.c file into the file where the text is actually used,
#   and mark them with the TRANSLATORS keyword, as in
#
#   // TRANSLATORS: note the plural!
#   Add(new cMenuEditIntItem( tr("Setup.EPG$Preferred languages"), ...
#
# - Fill in your email address in the line
#   $MSGIDBUGS = "" ...
#   below, so that users can reach you in case there is a problem
#   with a message id.
#
# - Run this Perl script from within your plugin's source directory.
#   This script will run 'xgettext' to extract all of your texts
#   into a file named <pluginname>.pot. After that it will extract
#   your existing i18n texts from the file i18n.c (or wherever you
#   have defined them) and generate *.po files for all languages
#   currently known by VDR.
#
# - Your Makefile will be automatically adjusted to allow for
#   building the *.mo files necessary for supporting gettext().
#   Please check carefully whether the automatic adjustments have
#   succeeded, though. The modified Makefile will be written to
#   Makefile.new, so you can call this script any numer of times
#   in case you need to do some fine tuning of your texts (especially
#   adding some trNOOP() in case the script reports "missing phrases".
#   Once you're done converting your plugin source to using gettext,
#   rename Makefile.new to Makefile.
#
# - You may want to remove the old internationalized texts from your
#   source. Usually this means removing the files i18n.[hc], stripping
#   i18n.o from the OBJS variable in the Makefile and removing all
#   '#include "i18n.h"' lines from the rest of your source files.
#   These steps are not done automatically.

# Check for the existence of a Makefile:

die "Makefile not found!" unless (-f "Makefile");

# Determine the plugin name:

$PLUGIN = `grep '^PLUGIN *=' Makefile | head -1` || die "can't find plugin name!";
$PLUGIN =~ s/.*= *(.*)\n/$1/;
die "can't find plugin name!" unless ($PLUGIN);

# Locate the file containing the texts:

$I18NFILE = "";
for ("i18n.c", "i18n.h", `ls *.c`) { # try i18n.[ch] explicitly first
    chomp($f = $_);
    if (-f $f && `grep tI18nPhrase $f`) {
       $I18NFILE = $f;
       last;
       }
    }
die "can't find internationalized texts!" unless ($I18NFILE);

# Plugin specific information:

$TITLE = "VDR plugin language source file";
$COPYRIGHT = "2007 Klaus Schmidinger <kls\@cadsoft.de>";
$PACKAGE = "VDR";
$VERSION = "1.5.7";
$MSGIDBUGS = "vdr\@jwendel.de" || die "enter your email address here"; # escape the '@', as in "user\@domain.tld"
$LANGUAGETEAM = "<vdr\@linuxtv.org>";

# Gettext environment:

$XGETTEXT = "xgettext -C -cTRANSLATORS --no-wrap -F -k -ktr -ktrNOOP --msgid-bugs-address='$MSGIDBUGS'";
$PODIR = "po";
$POTFILE = "$PODIR/$PLUGIN.pot";

# Languages as known by VDR 1.5.6:

@LANGS = (
  "en_US",
  "de_DE",
  "sl_SI",
  "it_IT",
  "nl_NL",
  "pt_PT",
  "fr_FR",
  "nn_NO",
  "fi_FI",
  "pl_PL",
  "es_ES",
  "el_GR",
  "sv_SE",
  "ro_RO",
  "hu_HU",
  "ca_ES",
  "ru_RU",
  "hr_HR",
  "et_EE",
  "da_DK",
  "cs_CZ",
  "tr_TR"
  );

@CHARSETS = (
  "ISO-8859-15",
  "ISO-8859-15",
  "ISO-8859-2",
  "ISO-8859-15",
  "ISO-8859-15",
  "ISO-8859-1",
  "ISO-8859-1",
  "ISO-8859-1",
  "ISO-8859-15",
  "ISO-8859-2",
  "ISO-8859-15",
  "ISO-8859-7",
  "ISO-8859-1",
  "ISO-8859-2",
  "ISO-8859-2",
  "ISO-8859-1",
  "ISO-8859-5",
  "ISO-8859-2",
  "ISO-8859-13",
  "ISO-8859-15",
  "ISO-8859-2",
  "ISO-8859-9",
  );

@TRANSLATORS = (
  "Klaus Schmidinger <kls\@cadsoft.de>, 2000",
  "Klaus Schmidinger <kls\@cadsoft.de>, 2000",
  "Miha Setina <mihasetina\@softhome.net>, 2000; Matjaz Thaler <matjaz.thaler\@guest.arnes.si>, 2003",
  "Alberto Carraro <bertocar\@tin.it>, 2001; Antonio Ospite <ospite\@studenti.unina.it>, 2003; Sean Carlos <seanc\@libero.it>, 2005",
  "Arnold Niessen <niessen\@iae.nl> <arnold.niessen\@philips.com>, 2001; Hans Dingemans <hans.dingemans\@tacticalops.nl>, 2003; Maarten Wisse <Maarten.Wisse\@urz.uni-hd.de>, 2005",
  "Paulo Lopes <pmml\@netvita.pt>, 2001",
  "Jean-Claude Repetto <jc\@repetto.org>, 2001; Olivier Jacques <jacquesolivier\@hotmail.com>, 2003; Gregoire Favre <greg\@magma.unil.ch>, 2003; Nicolas Huillard <nhuillard\@e-dition.fr>, 2005",
  "J�rgen Tvedt <pjtvedt\@online.no>, 2001; Truls Slevigen <truls\@slevigen.no>, 2002",
  "Hannu Savolainen <hannu\@opensound.com>, 2002; Jaakko Hyv�tti <jaakko\@hyvatti.iki.fi>, 2002; Niko Tarnanen <niko.tarnanen\@hut.fi>, 2003; Rolf Ahrenberg <rahrenbe\@cc.hut.fi>, 2003",
  "Michael Rakowski <mrak\@gmx.de>, 2002",
  "Ruben Nunez Francisco <ruben.nunez\@tang-it.com>, 2002",
  "Dimitrios Dimitrakos <mail\@dimitrios.de>, 2002",
  "Tomas Prybil <tomas\@prybil.se>, 2002; Jan Ekholm <chakie\@infa.abo.fi>, 2003",
  "Paul Lacatus <paul\@campina.iiruc.ro>, 2002; Lucian Muresan <lucianm\@users.sourceforge.net>, 2004",
  "Istvan Koenigsberger <istvnko\@hotmail.com>, 2002; Guido Josten <guido.josten\@t-online.de>, 2002",
  "Marc Rovira Vall <tm05462\@salleURL.edu>, 2003; Ramon Roca <ramon.roca\@xcombo.com>, 2003; Jordi Vil� <jvila\@tinet.org>, 2003",
  "Vyacheslav Dikonov <sdiconov\@mail.ru>, 2004",
  "Drazen Dupor <drazen.dupor\@dupor.com>, 2004; Dino Ravnic <dino.ravnic\@fer.hr>, 2004",
  "Arthur Konovalov <kasjas\@hot.ee>, 2004",
  "Mogens Elneff <mogens\@elneff.dk>, 2004",
  "Vladim�r B�rta <vladimir.barta\@k2atmitec.cz>, 2006",
  "Oktay Yolge�en <oktay_73\@yahoo.de>, 2007"
  );

@LASTTRANSLATOR = (
  "Klaus Schmidinger <kls\@cadsoft.de>",
  "Klaus Schmidinger <kls\@cadsoft.de>",
  "Matjaz Thaler <matjaz.thaler\@guest.arnes.si>",
  "Sean Carlos <seanc\@libero.it>",
  "Maarten Wisse <Maarten.Wisse\@urz.uni-hd.de>",
  "Paulo Lopes <pmml\@netvita.pt>",
  "Nicolas Huillard <nhuillard\@e-dition.fr>",
  "Truls Slevigen <truls\@slevigen.no>",
  "Rolf Ahrenberg <rahrenbe\@cc.hut.fi>",
  "Michael Rakowski <mrak\@gmx.de>",
  "Ruben Nunez Francisco <ruben.nunez\@tang-it.com>",
  "Dimitrios Dimitrakos <mail\@dimitrios.de>",
  "Tomas Prybil <tomas\@prybil.se>",
  "Lucian Muresan <lucianm\@users.sourceforge.net>",
  "Istvan Koenigsberger <istvnko\@hotmail.com>, Guido Josten <guido.josten\@t-online.de>",
  "Jordi Vil� <jvila\@tinet.org>",
  "Vyacheslav Dikonov <sdiconov\@mail.ru>",
  "Drazen Dupor <drazen.dupor\@dupor.com>",
  "Arthur Konovalov <kasjas\@hot.ee>",
  "Mogens Elneff <mogens\@elneff.dk>",
  "Vladim�r B�rta <vladimir.barta\@k2atmitec.cz>",
  "Oktay Yolge�en <oktay_73\@yahoo.de>"
  );

# Collect all translated texts:

open(F, $I18NFILE) || die "$I18NFILE: $!\n";
$InComment = 0;
while (<F>) {
      chomp;
      s/\t/ /g; # get rid of tabs
      s/  *$//; # get rid of trailing blanks
      s/^ *\/\/.*//; # remove comment lines
      s/ *\/\/.*//; # strip trailing comments
      s/\/\*.*\*\///g; # strip c comments
      if (/\/\*/) {
         $InComment = 1;
         s/\/\*.*$//; # remove start of comment
         }
      elsif (/\*\//) {
         $InComment = 0;
         s/^.*\*\///; # remove end of comment
         }
      elsif ($InComment) {
         next;
         }
      next if (/^ *$/); # skip empty lines
      next if (/#if/);
      next if (/#endif/);
      next unless ($found or $found = /const *tI18nPhrase .*{/); # sync on phrases
      next if (/const *tI18nPhrase .*{/); # skip sync line
      last if (/{ *NULL *}/); # stop after last phrase
      if (/{ *(.*),/) {
         $Original = $1;
         $Index = 0;
         }
      elsif (/}/) {
         }
      else {
         $Translated = $_;
         $Translated =~ s/ *(.*),/$1/;
         $Index++;
         $Map{$Original}[$Index] = $Translated;
         }
      }
close(F);

# Generate the .pot file:

(mkdir($PODIR) || die "$PODIR: $!\n") unless -d $PODIR;

system("$XGETTEXT -o $POTFILE *.c");

# Generate .po files for all languages:

open(POT, $POTFILE) || die "$POTFILE: $!\n";
@POT = <POT>;
close(POT);

for ($Index = 1; $LANGS[$Index]; $Index++) {

    $Authors = "# " . join("\n# ", split(/; */, $TRANSLATORS[$Index])) . "\n";

    $POFILE = "$PODIR/$LANGS[$Index].po";
    open(PO, ">$POFILE") || die "$POFILE: $!\n";
    $msgid = "";
    for (@POT) {
        next if (/^#, fuzzy$/); # skip the 'fuzzy' keyword
        $msgid = $1 if (/^msgid (".+")/);
        $date = $1 if (/POT-Creation-Date: (.*)\\n/);
        $s = $_;
        if (!$msgid) {
           $s =~ s/SOME DESCRIPTIVE TITLE/$TITLE/;
           $s =~ s/YEAR THE PACKAGE'S COPYRIGHT HOLDER/$COPYRIGHT/;
           $s =~ s/PACKAGE/$PACKAGE/;
           $s =~ s/VERSION/$VERSION/;
           $s = $Authors if (/FIRST AUTHOR/);
           $s =~ s/YEAR-MO-DA HO:MI\+ZONE/$date/;
           $s =~ s/FULL NAME <EMAIL\@ADDRESS>/$LASTTRANSLATOR[$Index]/;
           $s =~ s/LANGUAGE <LL\@li\.org>/$LANGUAGETEAM/;
           $s =~ s/CHARSET/$CHARSETS[$Index]/;
           }
        else {
           $s =~ s/""/$Map{$msgid}[$Index]/ if ($msgid && /^msgstr ""/ && defined $Map{$msgid}[$Index]);
           $Used{$msgid} = 1;
           }
        print PO $s;
        }
    close(PO);
    }

# Report missing phrases:

for (keys %Map) {
    push(@Missing, "$_\n") unless $Used{$_};
    }

if (@Missing) {
   print "Missing phrases (may need to use trNOOP):\n\n";
   print sort(@Missing);
   }

# Adjust the Makefile:

$MAKEI18N = q{### Internationalization (I18N):

PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Ndirs  = $(notdir $(foreach file, $(I18Npo), $(basename $(file))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	xgettext -C -cTRANSLATORS --no-wrap -F -k -ktr -ktrNOOP --msgid-bugs-address='<} . $MSGIDBUGS . q{>' -o $@ $(wildcard *.c)

$(I18Npo): $(I18Npot)
	msgmerge -U --no-wrap -F --backup=none -q $@ $<

i18n: $(I18Nmo)
	@mkdir -p $(LOCALEDIR)
	for i in $(I18Ndirs); do\
	    mkdir -p $(LOCALEDIR)/$$i/LC_MESSAGES;\
	    cp $(PODIR)/$$i.mo $(LOCALEDIR)/$$i/LC_MESSAGES/vdr-$(PLUGIN).mo;\
	    done

};

open(OLD, "Makefile") || die "Makefile: $!";
open(NEW, ">Makefile.new") || die "Makefile.new: $!";
while (<OLD>) {
      chomp;
      if (/Targets:/) {
         print NEW $MAKEI18N;
         }
      elsif (/^all:/) {
         $_ .= " i18n";
         }
      elsif (/^clean:/) {
         $_ .= "\n\t\@-rm -f \$(PODIR)/*.mo \$(PODIR)/*.pot";
         }
      print NEW "$_\n";
      }
close(NEW);
close(OLD);

/*
 * pin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Date: 29.01.2006, horchi
 */

#include <stdio.h>
#include <sys/types.h>
#include <regex.h>

#include "rep.h"
#include "def.h"

//**************************************************************************
//  Regular Expression Searching
//**************************************************************************

int rep(const char* string, const char* expression, Option options)
{
  const char* tmpA;
  const char* tmpB;

  return rep( string, expression, tmpA, tmpB, options);
}


int rep(const char* string, const char* expression, const char*& s_location, 
        Option options)
{
  const char* tmpA;

  return rep( string, expression, s_location, tmpA, options);
}


int rep(const char* string, const char* expression, const char*& s_location, 
        const char*& e_location, Option options)
{
   regex_t reg;
   regmatch_t rm;
   int status;
   int opt = 0;

   // Vorbereiten von reg fuer die Expressionsuche mit regexec
   // Flags:  REG_EXTENDED = Use Extended Regular Expressions
   //         REG_ICASE    = Ignore case in match.

   reg.re_nsub = 0;

   // Options umwandeln
   if (options & repUseRegularExpression)
     opt = opt | REG_EXTENDED;
   if (options & repIgnoreCase)
     opt = opt | REG_ICASE;
 
   if (regcomp( &reg, expression, opt) != 0)
     return fail;  

   // Suchen des ersten Vorkommens von reg in string

   status = regexec( &reg, string, 1, &rm, 0);
   regfree(&reg);

   if (status != 0) 
     return fail; 

   // Suche erfolgreich =>
   // Setzen der ermittelten Start- und Endpositionen

   s_location = (char*)(string + rm.rm_so);
   e_location = (char*)(string + rm.rm_eo);

   return success; 
}


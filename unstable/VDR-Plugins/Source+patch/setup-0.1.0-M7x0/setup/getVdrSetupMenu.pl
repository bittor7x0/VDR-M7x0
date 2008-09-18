#!/usr/bin/perl

sub getClass
{

  push @classes, $_;
  if(/(\s*class\s+cMenuSetup)(.*)(\s+:\s+public\s+cMenuSetupBase)/)
  {
    push @identifiers,$2;
  }
  while(<INPUT>)
  {
    push @classes, $_;
    if(/}\s*;/)
      { last;}
  };

  push @classes, "\n\n";
}

#####################################################################################
# Extracts VDR Setup Class Definitions from <vdr-dir>/menu.c
#####################################################################################
my $file   = $ARGV[0];
my $output = "setupsystemmenu.cpp";
my @headers=[];
local @classes=[];
local @identifiers=[];

open(INPUT,  $file)      or die "Can't open $file ";
open(OUTPUT, ">$output") or die "Can't open $file ";

 while (<INPUT>) 
 {   
    if (/^\s*#include/)
    {
      #printf(OUTPUT $_);
      push @headers, $_;
    }
    if (/^\s*class\s+\w+MenuSetupBase/) 
    {
       getClass;
    }
    if (/^\s*class\s+\w+\s*:\s*public\s+cMenuSetupBase/) 
    {
       getClass;
    }
 }



 printf( OUTPUT "/***********************************************************\n");
 printf( OUTPUT " * Header file holding Class Definitions from <VDR>/menu.c *\n");
 printf( OUTPUT " *                                                         *\n");
 printf( OUTPUT " * This file is generated automaticly!                     *\n");
 printf( OUTPUT " * Do not make changes!                                    *\n");
 printf( OUTPUT " *                                                         *\n");
 printf( OUTPUT " ***********************************************************/\n\n");
 printf( OUTPUT "// Headers included by <VDR>/menu.c\n");

for($i=1; $i<= $#headers; $i++)
{
  printf( OUTPUT "%s", $headers[$i]);
}

printf( OUTPUT "\n// local headers needed\n");
printf( OUTPUT "#include \"setupsystemmenu.h\"\n");
printf( OUTPUT "#include \"setupmenu.h\"\n");
printf( OUTPUT "#include \"debug.h\"\n");

printf( OUTPUT "// ************************************\n");
printf( OUTPUT "// SetupClasses defined in <VDR>/menu.c\n");
printf( OUTPUT "// ************************************\n\n");
for($i=1; $i<= $#classes; $i++)
{
  printf( OUTPUT "%s", $classes[$i]);
}

printf( OUTPUT "//############################################################################\n");
printf( OUTPUT "// cSetupSystemMenu\n");
printf( OUTPUT "//############################################################################\n");
printf( OUTPUT "cSetupSystemMenu::cSetupSystemMenu()\n");
printf( OUTPUT "{\n");
printf( OUTPUT "}\n");
printf( OUTPUT "cSetupSystemMenu::~cSetupSystemMenu()\n");
printf( OUTPUT "{\n");
printf( OUTPUT "}\n");
printf( OUTPUT "// Get SystemMenu\n");
printf( OUTPUT "cOsdMenu *cSetupSystemMenu::GetSystemMenu(const char *sysCommand, Config  *config)\n");
printf( OUTPUT "{\n");
printf( OUTPUT " cOsdMenu *menu=NULL;\n");
printf( OUTPUT " if( sysCommand != NULL)\n");
printf( OUTPUT " {\n");
printf( OUTPUT "    if( strcmp(sysCommand, \"ActPlugins\") == 0)\n");
printf( OUTPUT "        menu =new cSetupPluginMenu(config);\n");
printf( OUTPUT "//    else\n");
printf( OUTPUT "//      if(strcmp(sysCommand, \"VDRMenu\") == 0)\n");
printf( OUTPUT "//        menu = new cSetupVdrMenu(tr(\"Menu Edit\"));\n");


for($i=1; $i<= $#identifiers; $i++)
{
   printf( OUTPUT "    else\n");
   printf( OUTPUT "      if(strcmp(sysCommand, \"%s\") == 0)\n", $identifiers[$i]);
   printf( OUTPUT "         menu = new cMenuSetup%s;\n", $identifiers[$i]);
}

printf( OUTPUT "   else\n");
printf( OUTPUT "      DEBUG3(\"%%s: <menu System=%%s> not known\\n\", DBG_PREFIX, sysCommand);\n");
printf( OUTPUT "  }\n");
printf( OUTPUT "  return (menu);\n");
printf( OUTPUT "}\n");

close(OUTPUT);
close(FILE)



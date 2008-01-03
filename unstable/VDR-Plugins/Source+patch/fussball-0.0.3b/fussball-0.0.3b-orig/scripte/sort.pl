#!/usr/bin/perl


my ( $bez, $stand, $dummy);
while(<STDIN>)
{
        $bez   = $_;
        chomp($bez);
        $stand = <STDIN>;
        $dummy = <STDIN>;
        printf "%-25s %s", $bez, $stand;
}

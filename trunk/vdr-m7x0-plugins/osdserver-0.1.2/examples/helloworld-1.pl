#!/usr/bin/perl
use OSDServer;

my $server = OSDServer->Open() or die "open";
$server->Message("Hello World!");
$server->Close();

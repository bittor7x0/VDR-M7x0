#!/usr/bin/perl
use OSDServer;

my $server = OSDServer->Open() or die "open";
if ($server->Message("Is this easy?") eq "keyOk") {
	print "This is easy!\n";
}
$server->Close();

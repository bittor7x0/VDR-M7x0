#!/usr/bin/perl
use OSDServer;

my $server = OSDServer->Open() or die "open";
my $menu = $server->NewMenu("Hello World editing");
$menu->SetColumns(10);
$menu->EnableEvent(["keyOk"]);
my $input = $menu->AddNewEditStrItem("Edit Text", "Hello World");
$menu->Show();
my (undef,undef,$event) = $menu->SleepEvent();
if ($event eq "keyOk") {
	$text = $input->GetValue();
	print "$text\n";
}
$server->Close();

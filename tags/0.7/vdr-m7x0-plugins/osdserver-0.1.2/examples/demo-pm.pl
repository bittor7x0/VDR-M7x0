#!/usr/bin/perl

use OSDServer;

use strict;
use warnings;

use constant true => 1;
use constant false => 0;

my $who="OSDServer";
my $what="cool";

sub EditUserComment($) {
	my ($server)=@_;
	
    $server->EnterLocal()
    or return false;
    
	my $menu = $server->NewMenu("Edit User comment");
	defined $menu
	and $menu->SetColumns(15)
    and $menu->EnableEvent(["keyOk", "close"])
    or return false;

    $menu->AddNewOsdItem("--- Who do you want to comment? --------------------", UnSelectable => 1)
    or return false;
    
    my $whoedit = $menu->AddNewEditListItem("Who", ["OSDServer", "VDR", "Linux", "Windows", "Klaus", "Bill"], SelectName => $who)
    or return false;
    
    $whoedit->SetCurrent();
    
    $menu->AddNewOsdItem("--- And how do you like it? --------------------", UnSelectable => 1)
    or return false;
    
    my $whatedit = $menu->AddNewEditStrItem("It is", $what)
    or return false;
    
    $server->GetFocusMenu()->AddSubMenu($menu)
    or return false;
    
    $menu->Show()
    or return false;
    
    while (1) {
    	my ($id, $idt, $event) = $menu->SleepEvent();
    	
        if ($id eq $menu && $event eq "keyOk") {
        	$who = $whoedit->GetValue(Name => 1);
    	    return false if !defined $who;
            
    	    $what = $whatedit->GetValue();
    	    return false if !defined $what;
            
            $menu->SendState("osBack")
            or return false;
            
            last;
        }
        if ($id eq $menu && $event eq "close") {
            last;
        }
    }    
    $menu->Delete()
    and $server->LeaveLocal()
    or return false;
    
    return true;
}

sub HelloWorld($) {
	my ($server)=@_;

	my $menu = $server->NewMenu("Hello world");	
	defined $menu
	and $menu->SetColorKeyText(Blue => "Help")
    and $menu->SetColumns(15)
    and $menu->EnableEvent(["keyBlue", "close"])
    or return false;
    
    defined $menu->AddNewOsdItem("--- What do you think? --------------------", UnSelectable => 1)
    or return false;
    
    my $opt1 = $menu->AddNewOsdItem("This is awesome!");
    defined $opt1
    and $opt1->SetCurrent()
    and $opt1->EnableEvent(["keyOk"])
    or return false;

	my $opt2 = $menu->AddNewOsdItem("You ain't seen nothing yet");	
	defined $opt2
    and $opt2->EnableEvent(["keyOk"])
    or return false;

	my $opt3 = $menu->AddNewOsdItem("All your OSD are belong to us");
	defined $opt3
    and $opt3->EnableEvent(["keyOk"])
    or return false;

	my $opt4 = $menu->AddNewOsdItem("User comment: $who is $what");
	defined $opt4
    and $opt4->EnableEvent(["keyOk", "keyRed", "focus", "blur"])
    or return false;

    $menu->Show()
    or return false;
    
    while (1) {
    	my ($id, $idt, $event) = $menu->SleepEvent();
    	return false if !defined $event;
    
        if ($id eq $menu && $event eq "close") {
            $server->Message("You did not pick anything.")
            or return false;
            last;
        }
        
    	if ($id eq $opt1 && $event eq "keyOk") {
            $server->Message("It is!")
            and $menu->SendState("osEnd")
            or return false;
            last;
    	} elsif ($id eq $opt2 && $event eq "keyOk") {
            $server->Message("B-B-B-Baybee!")
            and $menu->SendState("osEnd")
            or return false;
            last;
    	} elsif ($id eq $opt3 && $event eq "keyOk") {
            $server->Message("You are on the way to display!")
            and $menu->SendState("osEnd")
            or return false;
            last;
    	} elsif ($id eq $opt4 && $event eq "keyOk") {
            $server->Message("$who is $what")
            and $menu->SendState("osEnd")
            or return false;
            last;
        } elsif ($id eq $opt4 && $event eq "focus") {
            $menu->SetColorKeyText(Red => "edit")
            and $menu->Show()
            or return false;
    	} elsif ($id eq $opt4 && $event eq "blur") {
            $menu->SetColorKeyText(Red => "")
            and $menu->Show()
            or return false;
    	} elsif ($id eq $opt4 && $event eq "keyRed") {
    	    EditUserComment($server)
    	    and $opt4->SetText("User comment: $who is $what")
    	    and $menu->Show()
    	    or return false;
    	}

        if ($id eq $menu && $event eq "keyBlue") {
            $server->Message("Pick one or leave with back")
            or return false;
        }        
    }
    $menu->Delete();
}

###############
#  Main Code  #
###############

my $server = OSDServer->Open();

$server->SetDebug(1);

if (!defined $server) {
	print STDERR "$server->{NetError}\n";
	exit 1;
}

HelloWorld($server);
# Do helloworld

$server->Close();
# ... and good bye

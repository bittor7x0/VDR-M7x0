#!/usr/bin/perl

use strict;
use warnings;
use Socket;

# Convenient return values
my $true = 1;
my $false = 0;


# Some generic stuff for handling a connection

sub NetError(*) {
    # Network error
    my ($msg)=@_;
    print STDERR "$msg\n";
    exit 1;
}

sub error() {
    # Fatal error. Send quit command, close socket and terminate.
    SendCmd("Quit");

	close SOCK;

    exit 1;
}

sub ConnectServer(**) {
    # Connect to the OSDServer
    my ($host,$port)=@_;    
    
    my $iaddr = inet_aton($host)                || NetError("no host: $host");
    my $paddr = sockaddr_in($port, $iaddr);
    
    my $proto = getprotobyname('tcp');
    socket(SOCK, PF_INET, SOCK_STREAM, $proto)  || NetError("socket: $!");
    connect(SOCK, $paddr)                       || NetError("connect: $!");
    select(SOCK); $| = 1; select(STDOUT);
}

my @reply2xx;
my @reply3xx;
my @reply4xx;
my @reply5xx;
my @reply6xx;

sub ReadReply() {
    # Read a complete server reply until 2xx return code,
    # and store replies in each category by number
	@reply2xx=undef;
	@reply3xx=undef;
	@reply4xx=undef;
	@reply5xx=undef;
	@reply6xx=undef;
	
	while (<SOCK>) {
	    chomp;
	    $_ =~ s/\r$//;
	    
		print "< $_\n";
		# screen echo
		
		if ($_ =~ /^2/) { @reply2xx=split(/\s+/,$_,2); last; }
		if ($_ =~ /^3/) { @reply3xx=split(/\s+/,$_); }
		if ($_ =~ /^4/) { @reply4xx=split(/\s+/,$_,2); }
		if ($_ =~ /^5/) { @reply5xx=split(/\s+/,$_,2); }
		if ($_ =~ /^6/) { 
		    my @tmpreply6xx=split(/\s+/,$_,2); 
		    if (defined $reply6xx[0] && $reply6xx[0] == $tmpreply6xx[0]) {
		        $tmpreply6xx[1] = "$tmpreply6xx[1]\n$reply6xx[1]";
		    }
		    @reply6xx = @tmpreply6xx;
		}
	}
	return $false if defined $reply4xx[0];
	return $true;
}

sub SendCmd(*) {
    # Send a command and read the reply

	my ($cmd)=@_;
	
	print "> $cmd\n";
	# screen echo
	
	print SOCK "$cmd\n";
	# network send
	
	return ReadReply;
}

sub IsEvent(**) {
    # Helper to check reply for a certain event

    my ($obj,$ev) = @_;
    return $false if !defined $reply3xx[0];
    return $false if $reply3xx[0] != 300;
    return $false if $reply3xx[1] ne $obj;
    return $false if $reply3xx[2] ne $ev;

    return $true;
}


sub QuoteString(*) {
    # Quote arbitrary string for use in '' and ""
    my ($str)=@_;
    
    $str =~ s/\\/\\\\/g;
    $str =~ s/'/\\'/g;
    $str =~ s/"/\\"/g;
    $str =~ s/\r/\\r/g;
    $str =~ s/\n/\\n/g;
    $str =~ s/\t/\\t/g;
       
    return $str;   
}

sub UnquoteString(*) {
    # Unquote string
    my ($str)=@_;
    
    $str =~ s/\\'/'/g;
    $str =~ s/\\"/"/g;
    $str =~ s/\\r/\r/g;
    $str =~ s/\\n/\n/g;
    $str =~ s/\\t/\t/g;
    $str =~ s/\\\\/\\/g;
       
    return $str;   
}

# Enough generic stuff, now for some hello world


my $who="OSDServer";
my $what="cool";

sub EditUserComment() {
    SendCmd "enterlocal" or return $false;
    # Preserve global variable space, so we can re-use 'menu'
    
    SendCmd "menu=New Menu 'Edit User comment'" or return $false;
    SendCmd "menu.SetColumns 15" or return $false;
    SendCmd "menu.EnableEvent keyOk close" or return $false;

    SendCmd "menu.AddNew OsdItem -unselectable '--- Who do you want to comment? --------------------'" or return $false;
    SendCmd "who=menu.AddNew EditListItem Who OSDServer VDR Linux Windows Klaus Bill -SelectName '$who'" or return $false;
    SendCmd "who.SetCurrent" or return $false;
    
    SendCmd "menu.AddNew OsdItem -unselectable '--- And how do you like it? --------------------'" or return $false;
    SendCmd "what=menu.AddNew EditStrItem 'It is' '$what'" or return $false;
    
    SendCmd "_focus.addsubmenu menu" or return $false;
    SendCmd "menu.show" or return $false;
    
    while (1) {
        SendCmd "menu.SleepEvent" or return $false;

        if (IsEvent("menu","keyOk")) {
    	    SendCmd "who.GetValue -name -quoted" or return $false;
    	    if (!defined $reply5xx[0] || $reply5xx[0] != 500) { return $false; }
    	    $who = $reply5xx[1];
            
    	    SendCmd "what.GetValue -quoted" or return $false;
    	    if (!defined $reply5xx[0] || $reply5xx[0] != 500) { return $false; }
    	    $what = $reply5xx[1];
            
            SendCmd "menu.SendState osBack" or return $false;
            SendCmd "delete menu" or return $false;
            SendCmd "leavelocal" or return $false;
            return $true;
        }
        if (IsEvent("menu","close")) {
            SendCmd "delete menu" or return $false;    
            SendCmd "leavelocal" or return $false;
            return $true;
        }
    }    
}

sub HelloWorld() {
    SendCmd "menu=New Menu 'Hello world'" or return $false;
    SendCmd "menu.SetColorKeyText -blue 'Help'" or return $false;
    SendCmd "menu.SetColumns 15" or return $false;
    SendCmd "menu.EnableEvent keyBlue close" or return $false;

    SendCmd "menu.AddNew OsdItem -unselectable '--- What do you think? --------------------'" or return $false;

    SendCmd "opt1=menu.AddNew OsdItem 'This is awesome!'" or return $false;
    SendCmd "opt1.SetCurrent" or return $false;
    SendCmd "opt1.EnableEvent keyOk" or return $false;

    SendCmd "opt2=menu.AddNew OsdItem 'You ain\\'t seen nothing yet'" or return $false;
    SendCmd "opt2.EnableEvent keyOk" or return $false;

    SendCmd "opt3=menu.AddNew OsdItem 'All your OSD are belong to us'" or return $false;
    SendCmd "opt3.EnableEvent keyOk" or return $false;

    SendCmd "opt4=menu.AddNew OsdItem 'User comment: $who is $what'" or return $false;
    SendCmd "opt4.EnableEvent keyOk keyRed focus blur" or return $false;

    SendCmd "menu.Show" or return $false;
    
    while (1) {
        SendCmd "menu.SleepEvent" or return $false;
    
        if (IsEvent("menu","close")) {
            SendCmd "Message 'You did not pick anything.'" or return $false;
            return $true;
        }
        
    	if (IsEvent("opt1","keyOk")) {
            SendCmd "Message 'It is!'" or return $false;
            SendCmd "menu.SendState osEnd" or return $false;
            return $true;
    	} elsif (IsEvent("opt2","keyOk")) {
            SendCmd "Message 'B-B-B-Baybee!'" or return $false;
            SendCmd "menu.SendState osEnd" or return $false;
            return $true;
    	} elsif (IsEvent("opt3","keyOk")) {
            SendCmd "Message 'You are on the way to display!'" or return $false;
            SendCmd "menu.SendState osEnd" or return $false;
            return $true;
    	} elsif (IsEvent("opt4","keyOk")) {
            SendCmd "Message '$who is $what'" or return $false;
            SendCmd "menu.SendState osEnd" or return $false;
            return $true;
        } elsif (IsEvent("opt4","focus")) {
            SendCmd "menu.SetColorKeyText -red edit" or return $false;
            SendCmd "menu.Show" or return $false;
    	} elsif (IsEvent("opt4","blur")) {
            SendCmd "menu.SetColorKeyText -red ''" or return $false;
            SendCmd "menu.Show" or return $false;
    	} elsif (IsEvent("opt4","keyRed")) {
    	    EditUserComment or return $false;
    	    SendCmd "opt4.SetText 'User comment: $who is $what'"  or return $false;
            SendCmd "menu.Show" or return $false;
    	}


        if (IsEvent("menu","keyBlue")) {
            SendCmd "Message 'Pick one or leave with back'" or return $false;
        }        
    }
}

###############
#  Main Code  #
###############

ConnectServer localhost,2010;
# Connect to the server process

ReadReply;
# Read server welcome

SendCmd "Version 0.1" or error;
# Identify to server with protocol version#

HelloWorld or error;
# Do helloworld

SendCmd "Quit";
# ... and good bye

close(SOCK);
# close socket

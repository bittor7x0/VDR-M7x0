
##################
package OSDServer;
##################

use strict;
use warnings;
use Socket;

# Convenient return values
use constant true => 1;
use constant false => 0;


sub new($) {
	my ($class)=@_;
	my $self = {
		SOCK => undef,
		nextid => 0,
		ids => {},
		reply2xx => [],
		reply3xx => [],
		reply4xx => [],
		reply5xx => [],
		reply6xx => [],
		NetError => "",
		Welcome => "",
		Debug => 0,
		FocusMenu => undef,
	};
	bless($self, $class);
	return $self;
}

# Some generic stuff for handling a connection

sub Close($) {
    my ($self)=@_;
    $self->SendCmd("Quit");
	close $self->{SOCK};
	$self->{SOCK} = undef;
}

sub error($) {
	my ($self)=@_;
    $self->Close();
    exit 1;
}

sub nextid($) {
	my ($self)=@_;
	return "id".++$self->{nextid};
}

sub SetDebug($$) {
	my ($self,$debug)=@_;
	$self->{Debug} = $debug;
}

sub ConnectServer($;$$) {
    # Connect to the OSDServer
    my ($self,$host,$port)=@_;
    
    if (!defined $host) { $host = "localhost"; }
    if (!defined $port) { $port = 2010; }
    
    my $iaddr = inet_aton($host);
    if (!$iaddr) {
    	$self->{NetError} = "no host: $host";
    	return false;
    }
    my $paddr = sockaddr_in($port, $iaddr);
    
    my $proto = getprotobyname('tcp');
    if (!socket($self->{SOCK}, PF_INET, SOCK_STREAM, $proto)) {
    	$self->{NetError} = "socket: $!";
    	return false;
    }
    if (!connect($self->{SOCK}, $paddr)) {
    	$self->{NetError} = "connect: $!";
    	return false;
    }
    my $oldhandle = select($self->{SOCK}); $| = 1; select($oldhandle);

	return true;    
}

sub SendGreeting($) {
   my ($self)=@_;
 	
	# Read server welcome
	if (!$self->ReadReply) {
		return false;
	}
	
    return false if !defined $self->{reply2xx}[0];
    return false if $self->{reply2xx}[0] != 201;
    
	$self->{Welcome} = $self->{reply3xx}[1];
	
	# Identify to server with protocol version#
	return false if !$self->SendCmd("Version 0.1");
	
	return true;
}

sub Open($;$$) {
    my ($self,$host,$port)=@_;
	
    if (!defined $host) { $host = "localhost"; }
    if (!defined $port) { $port = 2010; }
    
	if (!ref($self)) { 
		$self = OSDServer->new(); 
	}
	
	return undef if !$self->ConnectServer($host,$port);

	return undef if !$self->SendGreeting();
	
	return $self;
}


sub ReadReply($) {
    # Read a complete server reply until 2xx return code,
    # and store replies in each category by number
    my ($self)=@_;    

	$self->{reply2xx}=[];
	$self->{reply3xx}=[];
	$self->{reply4xx}=[];
	$self->{reply5xx}=[];
	$self->{reply6xx}=[];
	
	my $sock = $self->{SOCK};
	while (<$sock>) {
	    chomp;
	    $_ =~ s/\r$//;
	    
		print "< $_\n" if $self->{Debug};
		# screen echo
		
		if ($_ =~ /^2/) { $self->{reply2xx}=[split(/\s+/,$_,2)]; last; }
		if ($_ =~ /^3/) { $self->{reply3xx}=[split(/\s+/,$_)]; }
		if ($_ =~ /^4/) { $self->{reply4xx}=[split(/\s+/,$_,2)]; }
		if ($_ =~ /^5/) { $self->{reply5xx}=[split(/\s+/,$_,2)]; }
		if ($_ =~ /^6/) { 
		    my @tmpreply6xx=split(/\s+/,$_,2); 
		    if (defined $self->{reply6xx}[0] && $self->{reply6xx}[0] == $tmpreply6xx[0]) {
		        $tmpreply6xx[1] = "$tmpreply6xx[1]\n$self->{reply6xx}[1]";
		    }
		    $self->{reply6xx} = [@tmpreply6xx];
		}
	}
	return false if defined $self->{reply4xx}[0];
	return true;
}

sub GetCode2xx($) {
    my ($self)=@_;
    return false if !defined $self->{reply2xx}[0];
    return $self->{reply2xx}[0];
}

sub GetReply2xx($) {
    my ($self)=@_;
    return [@{$self->{reply2xx}}];
}

sub GetCode3xx($) {
    my ($self)=@_;
    return false if !defined $self->{reply3xx}[0];
    return $self->{reply3xx}[0];
}

sub GetReply3xx($) {
    my ($self)=@_;
    return [@{$self->{reply3xx}}];
}

sub GetCode4xx($) {
    my ($self)=@_;
    return false if !defined $self->{reply4xx}[0];
    return $self->{reply4xx}[0];
}

sub GetReply4xx($) {
    my ($self)=@_;
    return [@{$self->{reply4xx}}];
}

sub GetCode5xx($) {
    my ($self)=@_;
    return false if !defined $self->{reply5xx}[0];
    return $self->{reply5xx}[0];
}

sub GetReply5xx($) {
    my ($self)=@_;
    return [@{$self->{reply5xx}}];
}

sub GetCode6xx($) {
    my ($self)=@_;
    return false if !defined $self->{reply6xx}[0];
    return $self->{reply6xx}[0];
}

sub GetReply6xx($) {
    my ($self)=@_;
    return [@{$self->{reply6xx}}];
}

sub SendCmd($$) {
    # Send a command and read the reply

	my ($self,$cmd)=@_;
	
	print "> $cmd\n" if $self->{Debug};
	# screen echo
	
	my $sock = $self->{SOCK};
	print $sock "$cmd\n";
	# network send
	
	return $self->ReadReply();
}

sub QuoteString($) {
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

sub UnquoteString($) {
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

sub GetFocusMenu($) {
	my ($self) = @_;	
	if (!defined $self->{FocusMenu}) {
		$self->{FocusMenu} = OSDServer::Menu->attach($self, "_FOCUS");
	}
	return $self->{FocusMenu};
}

sub EnterLocal($) {
	my ($self) = @_;	
	return $self->SendCmd("ENTERLOCAL");
}

sub LeaveLocal($) {
	my ($self) = @_;	
	return $self->SendCmd("LEAVELOCAL");
}

sub Message($$%) {
	my ($self, $msg, %opts) = @_;
	
	if (defined $opts{StatusClear} && $opts{StatusClear}) {
		return $self->SendCmd('MESSAGE -statusclear');
	} elsif (defined $opts{Status} && $opts{Status}) {
		return $self->SendCmd('MESSAGE -status -- "'.QuoteString($msg).'"');
	} else {
		my $cmd = "MESSAGE";

		if (defined $opts{Info} && $opts{Info}) {
			$cmd = "$cmd -info";
		} elsif (defined $opts{Warn} && $opts{Warn}) {
			$cmd = "$cmd -warn";
		} elsif (defined $opts{Error} && $opts{Error}) {
			$cmd = "$cmd -error";
		}
		if (defined $opts{Queue} && $opts{Queue}) {
			$cmd = "$cmd -queue";
			if (defined $opts{Timeout}) {
				$cmd = sprintf("$cmd -timeout %i", int($opts{Timeout}));
			}
		}
		if (defined $opts{Seconds}) {
			$cmd = sprintf("$cmd -seconds %i", int($opts{Seconds}));
		}
		$cmd = $cmd.' -- "'.QuoteString($msg).'"';

		return undef if !$self->SendCmd($cmd); # error
		
		return "" if $self->GetCode3xx() == 301; # timeout
	
		return undef if $self->GetCode3xx() != 300; # unknown code
		
	    return $self->GetReply3xx()->[2];
	}	
}

sub NewMenu($$) {
	my $self = shift;
	return OSDServer::Menu->new($self, $self->nextid(), @_);
}

sub NewOsdItem($$%) {
	my $self = shift;
	return OSDServer::OsdItem->new($self, $self->nextid(), undef, @_);
}

sub NewEditStrItem($$$) {
	my $self = shift;
	return OSDServer::EditStrItem->new($self, $self->nextid(), undef, @_);
}

sub NewEditIntItem($$$%) {
	my $self = shift;
	return OSDServer::EditIntItem->new($self, $self->nextid(), undef, @_);
}

sub NewEditListItem($$\@%) {
	my $self = shift;
	return OSDServer::EditIntItem->new($self, $self->nextid(), undef, @_);
}

sub Delete($$) {
	my ($self, $id)=@_;
	$id->Delete();
}
	

##########################
package OSDServer::Object;
##########################

use constant true => 1;
use constant false => 0;

sub new($$$) {
	my ($class, $server, $id)=@_;
	my $self = {
		server => $server,
		id => $id,
	};
	$server->{ids}->{$id} = $self;
	bless($self, $class);
	return $self;
}

sub EnableEvent($\@) {
	my ($self, $events) = @_;
	my @events2 = map( { '"'.OSDServer::QuoteString($_).'"' } @{$events});
	
	return $self->SendCmd("ENABLEEVENT -- ".join(' ', @events2));	
}

sub id($) { return $_[0]->{id}; }

sub SendCmd($$;$) { 
	my $self = shift;
	my $assign = (@_ > 1) ? shift()." = " : "";
	my $cmd = shift;
	
	return $self->{server}->SendCmd($assign.$self->id().'.'.$cmd); 
}

sub Delete($) {
	my ($self) = @_;
	return false if !$self->SendCmd("DELETE");
	
	delete $self->{server}->{ids}->{$self->id()};
	$self->{id}=undef;
	$self->{server}=undef;
	return true;
}


########################
package OSDServer::Menu;
########################

use constant true => 1;
use constant false => 0;

@OSDServer::Menu::ISA = "OSDServer::Object";

sub new($$$$) {
	my ($class, $server, $name, $title) = @_;
	my $self = OSDServer::Object->new($server, $name);
	$self->{items} = {};
	bless($self, $class);

	$title = OSDServer::QuoteString($title);	
	return undef if !$self->{server}->SendCmd("$self->{id} = NEW MENU -- \"$title\"");
	
	return $self;
}

sub attach($$$) {
	my ($class, $server, $name) = @_;
	my $self = OSDServer::Object->new($server, $name);
	$self->{items} = {};
	bless($self, $class);
	
	return $self;
}

sub Delete($) {	
	my ($self) = @_;
	# for garbage collector: break cyclic refs
	foreach my $item (values %{$self->{items}}) {
		delete $item->{menu};
	}
	$self->{items} = undef;
	return $self->SUPER::Delete();
}

sub Add($$) {
	my ($self, $item) = @_;
	return false if !defined $item;
	
	return false if !$self->SendCmd("ADD ".$item->id());
	$self->{items}->{$item->id()} = $item;
	$item->{menu} = $self;
	return true;
}

sub AddNewOsdItem($$%) {
	my $self = shift;
	my $item = OSDServer::OsdItem->new($self->{server}, $self->{server}->nextid(), $self, @_);
	if (defined $item) {
		$self->{items}->{$item->id()} = $item;
		$item->{menu} = $self;
	}	
	return $item;
}

sub AddNewEditStrItem($$$) {
	my $self = shift;
	my $item = OSDServer::EditStrItem->new($self->{server}, $self->{server}->nextid(), $self, @_);
	if (defined $item) {
		$self->{items}->{$item->id()} = $item;
		$item->{menu} = $self;
	}	
	return $item;
}

sub AddNewEditIntItem($$$%) {
	my $self = shift;
	my $item = OSDServer::EditIntItem->new($self->{server}, $self->{server}->nextid(), $self, @_);
	if (defined $item) {
		$self->{items}->{$item->id()} = $item;
		$item->{menu} = $self;
	}	
	return $item;
}

sub AddNewEditListItem($$\@%) {
	my $self = shift;
	my $item = OSDServer::EditListItem->new($self->{server}, $self->{server}->nextid(), $self, @_);
	if (defined $item) {
		$self->{items}->{$item->id()} = $item;
		$item->{menu} = $self;
	}	
	return $item;
}

sub AddSubMenu($$) {
	my ($self, $menu) = @_;
	return $self->SendCmd("ADDSUBMENU ".$menu->id());
}

sub SetColorKeyText($%) {
	my ($self, %opts) = @_;
	
	my $cmd = "SETCOLORKEYTEXT";
	
	if (defined $opts{Red}) {		
		$cmd = $cmd.' -red "'.OSDServer::QuoteString($opts{Red}).'"';
	}
	if (defined $opts{Green}) {		
		$cmd = $cmd.' -green "'.OSDServer::QuoteString($opts{Green}).'"';
	}
	if (defined $opts{Yellow}) {		
		$cmd = $cmd.' -yellow "'.OSDServer::QuoteString($opts{Yellow}).'"';
	}
	if (defined $opts{Blue}) {		
		$cmd = $cmd.' -blue "'.OSDServer::QuoteString($opts{Blue}).'"';
	}
	
	return $self->SendCmd($cmd);	
}

sub SetColumns($$;$$$$) {
	my ($self, @cols) = @_;
	
	@cols=map(int, @cols);
	
	return $self->SendCmd("SETCOLUMNS ".join(' ', @cols));
}

sub SetCurrent($$) {
	my ($self, $newcurrent) = @_;
	return $self->SendCmd("SETCURRENT ".int($newcurrent));
}

sub GetCurrent($) {
	my ($self, $newcurrent) = @_;
	return false if !$self->SendCmd("GETCURRENT");

	return undef if $self->{server}->GetCode3xx() != 302; # unknown code
	
	my $itemno = $self->{server}->GetReply3xx()->[1];
	my $itemid = $self->{server}->GetReply3xx()->[2];
	my $item = $self->{server}->{ids}->{$itemid};
	$item = "" if !defined $item;
	return [$itemno, $item, $itemid];
}

sub Show($) {
	my ($self) = @_;	
	return $self->SendCmd("SHOW");
}

sub SendState($$) {
	my ($self, $state) = @_;	
	return $self->SendCmd('SENDSTATE -- "'.OSDServer::QuoteString($state).'"');
}

sub SleepEvent($%) {
	my ($self, %opts) = @_;
	
	my $cmd = "SLEEPEVENT";
	if (defined $opts{Timeout}) {
		$cmd = $cmd.'-timeout '.int($opts{Timeout});
	} elsif (defined $opts{TimeoutMS}) {
		$cmd = $cmd.'-timeoutms '.int($opts{TimeoutMS});
	}
	
	return undef if !$self->SendCmd($cmd);
	
	return ("", "", "") if $self->{server}->GetCode3xx() == 302; # timeout
	
	return undef if $self->{server}->GetCode3xx() != 300; # unknown code
	
	my $itemid = $self->{server}->GetReply3xx()->[1];
	my $item = $self->{server}->{ids}->{$itemid};
	$item = "" if !defined $item;
	my $event = $self->{server}->GetReply3xx()->[2];
	return ($item, $itemid, $event);
}

############################
package OSDServer::MenuItem;
############################

@OSDServer::MenuItem::ISA = "OSDServer::Object";

sub SetCurrent($) {
	my ($self) = @_;
	return $self->SendCmd("SETCURRENT");
}

sub Delete($) {
	my ($self) = @_;
	if (defined $self->{menu}) {
		delete $self->{menu}->{items}->{$self->id()};
	}
	return $self->SUPER::Delete();
}


###########################
package OSDServer::OsdItem;
###########################

@OSDServer::OsdItem::ISA = "OSDServer::MenuItem";

sub new($$$$$%) {
	my ($class, $server, $name, $menu, $title, %opts) = @_;
	my $self = OSDServer::Object->new($server, $name);
	bless($self, $class);

	my $par = "";
	
	if (defined $opts{UnSelectable} && $opts{UnSelectable}) {
		$par = " -unselectable";
	}

	$par = $par.' -- "'.OSDServer::QuoteString($title).'"';

	if (defined $menu) {
		return undef if !$menu->SendCmd($self->id(), "ADDNEW OSDITEM".$par)
	} else {
		return undef if !$self->{server}->SendCmd($self->id()." = NEW OSDITEM".$par);
	}	
	
	return $self;
}

sub SetUnSelectable($) {
	my ($self) = @_;
	return $self->SendCmd("SETUNSELECTABLE");
}

sub SetSelectable($) {
	my ($self) = @_;
	return $self->SendCmd("SETSELECTABLE");
}

sub SetText($$) {
	my ($self, $text) = @_;
	return $self->SendCmd('SETTEXT -- "'.OSDServer::QuoteString($text).'"');
}

###############################
package OSDServer::EditStrItem;
###############################

@OSDServer::EditStrItem::ISA = "OSDServer::MenuItem";

sub new($$$$$$) {
	my ($class, $server, $name, $menu, $desc, $value) = @_;
	my $self = OSDServer::Object->new($server, $name);
	bless($self, $class);

	my $par = ' -- "'.OSDServer::QuoteString($desc).'" "'.OSDServer::QuoteString($value).'"';

	if (defined $menu) {
		return undef if !$menu->SendCmd($self->id(), "ADDNEW EDITSTRITEM".$par)
	} else {
		return undef if !$self->{server}->SendCmd($self->id()." = NEW EDITSTRITEM".$par);
	}	
	
	return $self;
}

sub GetValue($) {
	my ($self) = @_;
	
	return undef if !$self->SendCmd("GETVALUE -quoted");

	return undef if $self->{server}->GetCode5xx() != 500; # unknown code
	
	return OSDServer::UnquoteString($self->{server}->GetReply5xx()->[1]);
}

###############################
package OSDServer::EditIntItem;
###############################

@OSDServer::EditIntItem::ISA = "OSDServer::MenuItem";

sub new($$$$$$%) {
	my ($class, $server, $name, $menu, $desc, $value, %opts) = @_;
	my $self = OSDServer::Object->new($server, $name);
	bless($self, $class);

	my $par = "";
	
	if (defined $opts{Min}) {
		$par = $par.' -min '.int($opts{Min});
	}
	if (defined $opts{Max}) {
		$par = $par.' -max '.int($opts{Max});
	}
	if (defined $opts{MinString}) {
		$par = $par.' -minstring "'.OSDServer::QuoteString($opts{MinString}).'"';
	}
	if (defined $opts{MaxString}) {
		$par = $par.' -maxstring "'.OSDServer::QuoteString($opts{MaxString}).'"';
	}

	$par = $par.' -- "'.OSDServer::QuoteString($desc).'" '.int($value);

	if (defined $menu) {
		return undef if !$menu->SendCmd($self->id(), "ADDNEW EDITINTITEM".$par)
	} else {
		return undef if !$self->{server}->SendCmd($self->id()." = NEW EDITINTITEM".$par);
	}	
	
	return $self;
}

sub GetValue($) {
	my ($self) = @_;
	
	return undef if !$self->SendCmd("GETVALUE");

	return undef if $self->{server}->GetCode5xx() != 500; # unknown code
	
	return int($self->{server}->GetReply5xx()->[1]);
}

###############################
package OSDServer::EditListItem;
###############################

@OSDServer::EditListItem::ISA = "OSDServer::MenuItem";

sub new($$$$$\@%) {
	my ($class, $server, $name, $menu, $desc, $values, %opts) = @_;
	my $self = OSDServer::Object->new($server, $name);
	bless($self, $class);

	my $par = "";

	if (defined $opts{Select}) {
		$par = $par.' -select '.int($opts{Select});
	} elsif (defined $opts{SelectName}) {
		$par = $par.' -selectname "'.OSDServer::QuoteString($opts{SelectName}).'"';
	}
		
	$par = $par.' -- "'.OSDServer::QuoteString($desc).'"';
	
	for my $value (@{$values}) {
		$par = $par.' "'.OSDServer::QuoteString($value).'"';
	}
	
	if (defined $menu) {
		return undef if !$menu->SendCmd($self->id(), "ADDNEW EDITLISTITEM".$par)
	} else {
		return undef if !$self->{server}->SendCmd($self->id()." = NEW EDITLISTITEM".$par);
	}	
	
	return $self;
}

sub GetValue($%) {
	my ($self,%opts) = @_;
	
	my $par = ' -quoted';
	if (defined $opts{Name} && $opts{Name}) {
		$par = $par.' -name';
	}
	
	return undef if !$self->SendCmd("GETVALUE".$par);

	return undef if $self->{server}->GetCode5xx() != 500; # unknown code
	
	my $result = $self->{server}->GetReply5xx()->[1];
	
	if (defined $opts{Name} && $opts{Name}) {
		return OSDServer::UnquoteString($result);
	} else {
		return int($result);
	}
}




1;
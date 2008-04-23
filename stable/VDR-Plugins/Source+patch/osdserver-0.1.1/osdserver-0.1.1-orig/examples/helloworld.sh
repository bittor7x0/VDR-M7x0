#!/bin/bash
#
# HelloWorld for OSDServer on localhost
#

# convenient return values
true=0
false=1


# Some generic stuff for handling a connection


function error() {
    # Fatal error. Send quit command, close FIFO and terminate netcat
    [ "${reply2xx[0]}" != 202 ] && SendCmd Quit

    exec 3>&-
    exec 4>&-

    kill $pid

    exit 1
}

function ConnectServer() {
    # Connect to the OSDServer

    # Set up temporary fifo and open as file descriptor 3 and 4
    mkfifo --mode=700 /tmp/pipe-in$$ /tmp/pipe-out$$
    exec 3<> /tmp/pipe-in$$
    exec 4<> /tmp/pipe-out$$
    rm /tmp/pipe-in$$ /tmp/pipe-out$$
    
    # Connect to server using the fifo
    { 
        netcat $1 $2
        
        # In case of connection loss:
        echo 499 disconnected
        echo 202 Good Bye.
    } <&3 >&4 &
    pid=$!
    
    # Sending to the server: use >&3
    # Receive from the server: use <&4
}


function ReadReply() {
    # Read a complete server reply until 2xx return code,
    # and store replies in each category by number
    reply2xx=()
    reply3xx=()
    reply4xx=()
    reply5xx=()
    reply6xx=()

    while read -r code line <&4 ; do
        echo "< $code $line"
        # screen echo
        
        case $code in
            2*)     IFS=$' \t\n\r' reply2xx=($code "$line")
                    ;;
            3*)     IFS=$' \t\n\r' reply3xx=($code $line)
                    ;;
            4*)     IFS=$' \t\n\r' reply4xx=($code "$line")
                    ;;
            5*)     IFS=$' \t\n\r' reply5xx=($code "$line")
                    ;;
            6*)     IFS=$' \t\n\r' reply6xx=($code "$line")
                    ;;
        esac
        [ -n "${reply2xx[0]}" ] && break
    done

    [ -n "${reply4xx[0]}" ] && return $false
    return $true
}

function SendCmd() {
    # Send a command and read the reply

    echo "> $*"
    # screen echo

    echo "$*" >&3
    # network send

    ReadReply
}

function IsEvent() {
    # Helper to check reply for a certain event

    [ "${reply3xx[0]}" != 300 ] && return $false
    [ "${reply3xx[1]}" != "$1" ] && return $false
    [ "${reply3xx[2]}" != "$2" ] && return $false

    return $true
}


function QuoteString() {
    # Quote arbitrary string for use in '' and ""
    local str="${!1}"
    
    str="${str//'\'/\\\\}"
    str="${str//'\\'/\\\\}"
    # work around bash bug: double quoted '\'
    
    str="${str//\'/$'\\\''}"
    # This is bogus, anyone knows something better to replace ' by \' ?
    
    str="${str//\"/\\\"}"
    str="${str//$'\r'/\\r}"
    str="${str//$'\n'/\\n}"
    str="${str//$'\t'/\\t}"
    
    eval "$1=\$str"
}

function UnquoteString() {
    # Unquote string
    local str="${!1}"

    str="${str//\\r/$'\r'}"
    str="${str//\\n/$'\n'}"
    str="${str//\\t/$'\t'}"
    str="${str//\\\"/\"}"
    str="${str//\\\'/\'}"
    str="${str//\\\\/\\}"

    eval "$1=\$str"
}


# Enough generic stuff, now for some hello world


who="OSDServer"
what="cool"

function EditUserComment() {
    SendCmd "enterlocal" || return $false
    # Preserve global variable space, so we can re-use 'menu'
    
    SendCmd "menu=New Menu 'Edit User comment'" || return $false
    SendCmd "menu.SetColumns 15" || return $false
    SendCmd "menu.EnableEvent keyOk close" || return $false

    SendCmd "menu.AddNew OsdItem -unselectable '--- Who do you want to comment? --------------------'" || return $false
    SendCmd "who=menu.AddNew EditListItem Who OSDServer VDR Linux Windows Klaus Bill -SelectName '$who'" || return $false
    SendCmd "who.SetCurrent" || return $false
    
    SendCmd "menu.AddNew OsdItem -unselectable '--- And how do you like it? --------------------'" || return $false
    SendCmd "what=menu.AddNew EditStrItem 'It is' '$what'" || return $false
    
    SendCmd "_focus.addsubmenu menu" || return $false
    SendCmd "menu.show" || return $false
    
    while true; do
        SendCmd "menu.SleepEvent" || return $false

        if IsEvent menu keyOk ; then
    	    SendCmd "who.GetValue -name -quoted" || return $false
    	    [ "${reply5xx[0]}" != 500 ] && return $false
    	    who="${reply5xx[1]}"
            
    	    SendCmd "what.GetValue -quoted" || return $false
    	    [ "${reply5xx[0]}" != 500 ] && return $false
    	    what="${reply5xx[1]}"
            
            SendCmd "menu.SendState osBack" || return $false
            SendCmd "delete menu" || return $false    
            SendCmd "leavelocal" || return $false    
            
            return $true
        fi
        if IsEvent menu close ; then
            SendCmd "delete menu" || return $false    
            SendCmd "leavelocal" || return $false    
            return $true
        fi
    done   
}

function HelloWorld() {
    SendCmd "menu=New Menu 'Hello world'" || return $false
    SendCmd "menu.SetColorKeyText -blue 'Help'" || return $false
    SendCmd "menu.SetColumns 15" || return $false
    SendCmd "menu.EnableEvent keyBlue close" || return $false

    SendCmd "lead=menu.AddNew OsdItem -unselectable '--- What do you think? --------------------'" || return $false

    SendCmd "opt1=menu.AddNew OsdItem 'This is awesome!'" || return $false
    SendCmd "opt1.SetCurrent" || return $false
    SendCmd "opt1.EnableEvent keyOk" || return $false
    
    SendCmd "opt2=menu.AddNew OsdItem 'You ain\\'t seen nothing yet'" || return $false
    SendCmd "opt2.EnableEvent keyOk" || return $false

    SendCmd "opt3=menu.AddNew OsdItem 'All your OSD are belong to us'" || return $false
    SendCmd "opt3.EnableEvent keyOk" || return $false

    SendCmd "opt4=menu.AddNew OsdItem 'User comment: $who is $what'" || return $false
    SendCmd "opt4.EnableEvent keyOk keyRed focus blur" || return $false

    SendCmd "menu.Show" || return $false

    while true ; do
        SendCmd "menu.SleepEvent" || return $false
        
        if IsEvent menu close ; then    
            SendCmd "Message 'You did not pick anything.'" || return $false
            return $true
        fi
        
    	if IsEvent opt1 keyOk ; then
            SendCmd "Message 'It is!'" || return $false
            SendCmd "menu.SendState osEnd" || return $false
            return $true
        fi
        
    	if IsEvent opt2 keyOk ; then
            SendCmd "Message 'B-B-B-Baybee!'" || return $false
            SendCmd "menu.SendState osEnd" || return $false
            return $true
        fi
        
    	if IsEvent opt3 keyOk ; then
            SendCmd "Message 'You are on the way to display!'" || return $false
            SendCmd "menu.SendState osEnd" || return $false
            return $true
        fi
        
    	if IsEvent opt4 keyOk ; then
            SendCmd "Message '$who is $what'" || return $false
            SendCmd "menu.SendState osEnd" || return $false
            return $true
    	fi
    	if IsEvent opt4 focus ; then
            SendCmd "menu.SetColorKeyText -red edit" || return $false
            SendCmd "menu.Show" || return $false
    	fi
    	if IsEvent opt4 blur ; then
            SendCmd "menu.SetColorKeyText -red ''" || return $false
            SendCmd "menu.Show" || return $false
    	fi
    	if IsEvent opt4 keyRed ; then
    	    EditUserComment || return $false
    	    SendCmd "opt4.SetText 'User comment: $who is $what'" || return $false
            SendCmd "menu.Show" || return $false
    	fi

        if IsEvent menu keyBlue ; then
            SendCmd "Message 'Pick one or leave with back'" || return $false
        fi
    done
}


###############
#  Main Code  #
###############


ConnectServer localhost 2010
# Connect to the server process

ReadReply || error
# Read server welcome

SendCmd "Version 0.1" || error
# Identify to server with protocol version

HelloWorld || error
# Do helloworld

SendCmd Quit
# ... and good bye

exec 3>&-
exec 4>&-
# close FIFOs


#include "channel-menu.h"
#include "setup-radiolist.h"

cRadioListMenu::cRadioListMenu(void) {
    cOsdMenu("Channels", CHNUMWIDTH);
    number = 0;
    Setup();
    Channels.IncBeingEdited();
}

void cRadioListMenu::Setup(void) {
    cChannel *currentChannel = GetChannel(Current());
    if (!currentChannel)
	currentChannel = Channels.GetByNumber(cDevice::CurrentChannel());
    
    cMenuChannelItem *currentItem = NULL;
    Clear();
	
    for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
	if (channel->Vpid() == 0 && (!channel->GroupSep() || RadiolistSetup.DisplaySeperators == 1) ) {
    	    if (!channel->GroupSep() || cMenuChannelItem::SortMode() == cMenuChannelItem::csmNumber && *channel->Name()) {
		cMenuChannelItem *item = new cMenuChannelItem(channel);
    		Add(item);
		
		if (channel == currentChannel)
	    	    currentItem = item;
	    }
	}
    }
    
    if (cMenuChannelItem::SortMode() != cMenuChannelItem::csmNumber)
	Sort();
    SetCurrent(currentItem);
    SetHelp(tr("Button$Edit"), tr("Button$New"), tr("Button$Delete"), tr("Button$Mark"));
    Display();    
}

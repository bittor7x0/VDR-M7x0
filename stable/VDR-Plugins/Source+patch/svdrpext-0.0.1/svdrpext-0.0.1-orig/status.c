#include <string.h>
#include "status.h"

cSvdrpExtStatus::cSvdrpExtStatus() {
	title = message = text = NULL;
	red = green = yellow = blue = NULL;
	selected = -1;
	memset(&tabs, 0, sizeof(tabs));
}
cSvdrpExtStatus::~cSvdrpExtStatus() {
	OsdClear();
}

void cSvdrpExtStatus::OsdClear() {
	free(title);
	free(message);
	free(red);
	free(green);
	free(yellow);
	free(blue);
	free(text);
	title = message = text = NULL;
	red = green = yellow = blue = NULL;
	items.Clear();
	selected = -1;
	memset(&tabs, 0, sizeof(tabs));
}

void cSvdrpExtStatus::OsdTitle(const char *Text) {
	if (Text) {
		title = strdup(Text);
	}
}

void cSvdrpExtStatus::OsdStatusMessage(const char *Message) {
	if (Message) {
		message = strdup(Message);
	}
}

void cSvdrpExtStatus::OsdHelpKeys(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
	if (Red) {
		red = strdup(Red);
	}
	if (Green) {
		green = strdup(Green);
	}
	if (Yellow) {
		yellow = strdup(Yellow);
	}
	if (Blue) {
		blue = strdup(Blue);
	}
}

void cSvdrpExtStatus::OsdItem(const char *Text, int Index) {
	const char* tab;
	const char* colStart = Text;
	for (int col = 0; col < MaxTabs &&
			(tab = strchr(colStart, '\t')); col++) {
		int width = tab - colStart + 1;
		if (width > tabs[col])
			tabs[col] = width;
		colStart = colStart + width;
	}
	char* s = strdup(Text);
	items.Add(new cSvdrpExtItem(s));
}

void cSvdrpExtStatus::OsdCurrentItem(const char *Text) {
	int i = -1;
	char* s = strdup(Text);
	cSvdrpExtItem * currentItem = NULL;
	for (cSvdrpExtItem *item = items.First(); item; item = items.Next(item)) {
		if (++i == selected)
			currentItem = item;
		if (strcmp(item->Text(), s) == 0) {
			selected = i;
			free(s);
			return;
		}
	}
	// no match: the same item is still selected but its text changed
	if (currentItem) {
		currentItem->Update(s);
	}
}

void cSvdrpExtStatus::OsdTextItem(const char *Text, bool Scroll) {
	if (Text) {
		text = strdup(Text);
		strreplace(text, '\n', '|');
	}
}

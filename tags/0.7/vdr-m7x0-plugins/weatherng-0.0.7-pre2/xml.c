/*
 *  $Id: xml.c,v 1.1.1.1 2005/01/17 20:02:10 beejay Exp $
 *  This module was kindly provided by Clemens Kirchgatterer
 */

#include <ctype.h>

#include <iostream>
#include <fstream>

#include "xml.h"

using namespace std;

enum {
	LOOK4START, // looking for first element start
	LOOK4TAG, // looking for element tag
	INTAG, // reading tag
	INCOMMENT, // reading comment
	LOOK4CEND1, // looking for second '-' in -->
	LOOK4CEND2, // looking for '>' in -->
	LOOK4ATTRN, // looking for attr name, > or /
	INATTRN, // reading attr name
	LOOK4ATTRV, // looking for attr value
	SAWSLASH, // saw / in element opening
	INATTRV, // in attr value
	LOOK4CLOSETAG, // looking for closing tag after <
	INCLOSETAG, // reading closing tag
};

XML::XML(const string &file) {
	char *buffer;
	long size;

	ifstream f(file.c_str(), ios::in|ios::binary|ios::ate);
	size = f.tellg();
	f.seekg(0, ios::beg);
	buffer = new char [size];
	f.read(buffer, size);
	f.close();
	data = buffer;
	delete[] buffer;
	nodestartcb = NULL;
	nodeendcb = NULL;
	cdatacb = NULL;
	parseerrorcb = NULL;
	progresscb = NULL;
}

XML::XML(const char *mem, unsigned int len) {
	data.assign(mem, len);
	nodestartcb = NULL;
	nodeendcb = NULL;
	cdatacb = NULL;
	parseerrorcb = NULL;
	progresscb = NULL;
}

void
XML::nodeStartCB(XML_NODE_START_CB(cb)) {
	nodestartcb = cb;
}

void
XML::nodeEndCB(XML_NODE_END_CB(cb)) {
	nodeendcb = cb;
}

void
XML::cdataCB(XML_CDATA_CB(cb)) {
	cdatacb = cb;
}

void
XML::parseErrorCB(XML_PARSE_ERROR_CB(cb)) {
	parseerrorcb = cb;
}

void
XML::progressCB(XML_PROGRESS_CB(cb)) {
	progresscb = cb;
}

int
XML::parse(void) {
	float percent = 0;
	unsigned int len;
	int last = 0;

	state = LOOK4START;
	linenr = 1;
	skipping = false;
	len = data.length();
	for (unsigned int i=0; i<len; i++) {
		if (readChar(data[i])) return (-1);
		if (progresscb) {
			percent = ((float)i/len)*100;
			if ((int)percent>last) {
				progresscb((int)percent);
				last = ((int)percent);
			}
		}
	}
	return (0);
}

bool
XML::isTokenChar(bool start, int c) {
	return (isalpha(c) || c == '_' || (!start && isdigit(c)));
}

int
XML::readChar(int c) {
	// new line?
	if (c == '\n') linenr++;

	switch (state) {

		// looking for element start
		case LOOK4START:
		if (c == '<') {
			if (cdatacb) {
				int pos = 0;
				while ((pos = cdata.find('&', pos)) != -1) {
					if (cdata.substr(pos, 4) == "&lt;")
					cdata.replace(pos, 4, "<");
					else if (cdata.substr(pos, 4) == "&gt;")
					cdata.replace(pos, 4, ">");
					else if (cdata.substr(pos, 5) == "&amp;")
					cdata.replace(pos, 5, "&");
					++pos;
				}
				if (!cdatacb(cdata))
				return (-1);
			}
			cdata = "";
			attr.clear();
			tag = "";
			state = LOOK4TAG;
		} else
		cdata += c;
		// silently ignore until resync
		break;

		// looking for element tag
		case LOOK4TAG:
		// skip comments and declarations.
		if (skipping) {
			if (c == '>') {
				skipping = false;
				state = LOOK4START;
			}
			break;
		} else {
			if (c == '?') {
				skipping = true;
				break;
			}
		}
		if (isTokenChar(1, c)) {
			tag += c;
			state = INTAG;
		} else if (c == '/') {
			state = LOOK4CLOSETAG;
		} else if (c == '!') {
			state = INCOMMENT;
		} else if (!isspace(c)) {
			if (parseerrorcb) {
				parseerrorcb(linenr, "Bogus tag char", c);
			}
			return (-1);
		}
		break;

		// reading tag
		case INTAG:
		if (isTokenChar(0, c)) {
			tag += c;
		} else if (c == '>') {
			if (nodestartcb)
			if (!nodestartcb(tag, attr))
			return (-1);
			state = LOOK4START;
		} else if (c == '/') {
			state = SAWSLASH;
		} else {
			state = LOOK4ATTRN;
		}
		break;

		// reading comment
		case INCOMMENT:
		if (c == '-') {
			state = LOOK4CEND1;
		}
		break;

		// looking for second '-' in "-->"
		case LOOK4CEND1:
		if (c == '-') {
			state = LOOK4CEND2;
		} else {
			state = INCOMMENT;
		}
		break;

		// looking for '>' in "-->"
		case LOOK4CEND2:
		if (c == '>') {
			state = LOOK4START;
		} else if (c != '-') {
			state = INCOMMENT;
		}
		break;

		// looking for attr name, > or /
		case LOOK4ATTRN:
		if (c == '>') {
			if (nodestartcb)
			if (!nodestartcb(tag, attr))
			return (-1);
			state = LOOK4START;
		} else if (c == '/') {
			state = SAWSLASH;
		} else if (isTokenChar(1, c)) {
			attrn = "";
			attrn += c;
			state = INATTRN;
		} else if (!isspace(c)) {
			if (parseerrorcb) {
				parseerrorcb(linenr, "Bogus 1st attr name char", c);
			}
			return (-2);
		}
		break;

		// saw / in element opening
		case SAWSLASH:
		if (c == '>') {
			if (nodestartcb)
			if (!nodestartcb(tag, attr))
			return (-1);
			if (nodeendcb)
			if (!nodeendcb(tag))
			return (-1);
			state = LOOK4START;
		} else {
			if (parseerrorcb) {
				parseerrorcb(linenr, "Bogus char before >", c);
			}
			return (-3);
		}
		break;

		// reading attr name
		case INATTRN:
		if (isTokenChar(0, c)) {
			attrn += c;
		} else if (isspace(c) || c == '=') {
			state = LOOK4ATTRV;
		} else {
			if (parseerrorcb) {
				parseerrorcb(linenr, "Bogus attr name char", c);
			}
			return (-4);
		}
		break;

		// looking for attr value
		case LOOK4ATTRV:
		if (c == '\'' || c == '"') {
			delim = c;
			attrv = "";
			state = INATTRV;
		} else if (!(isspace(c) || c == '=')) {
			if (parseerrorcb) {
				parseerrorcb(linenr, "No attribute value", c);
			}
			return (-5);
		}
		break;

		// in attr value
		case INATTRV:
		if (c == delim) {
			attr[attrn] = attrv;
			state = LOOK4ATTRN;
		} else if (!iscntrl(c)) {
			attrv += c;
		}
		break;

		// looking for closing tag after <
		case LOOK4CLOSETAG:
		if (isTokenChar(1, c)) {
			tag += c;
			state = INCLOSETAG;
		} else if (!isspace(c)) {
			if (parseerrorcb) {
				parseerrorcb(linenr, "Bogus preend tag char", c);
			}
			return (-6);
		}
		break;

		// reading closing tag
		case INCLOSETAG:
		if (isTokenChar(0, c)) {
			tag += c;
		} else if (c == '>') {
			if (nodeendcb)
			if (!nodeendcb(tag))
			return false;
			state = LOOK4START;
		} else if (!isspace(c)) {
			if (parseerrorcb) {
				parseerrorcb(linenr, "Bogus end tag char", c);
			}
			return (-7);
		}
		break;
	}

	return (0);
}

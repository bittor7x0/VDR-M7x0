/*
 *  $Id: xml.h,v 1.1.1.1 2005/01/17 20:02:10 beejay Exp $
 *  This module was kindly provided by Clemens Kirchgatterer
 */

#ifndef __XML_H
#define __XML_H

#include <string>
#include <map>

#define XML_NODE_START_CB(CB) \
bool (*CB)(const std::string &tag, std::map<std::string, std::string> &attr)
#define XML_NODE_END_CB(CB) \
bool (*CB)(const std::string &tag)
#define XML_CDATA_CB(CB) \
bool (*CB)(const std::string &text)
#define XML_PARSE_ERROR_CB(CB) \
void (*CB)(int line, const char *txt, char c)
#define XML_PROGRESS_CB(CB) \
void (*CB)(int percent)

class XML {

public:

	XML(const std::string &file);
	XML(const char *mem, unsigned int len);

	void nodeStartCB(XML_NODE_START_CB(cb));
	void nodeEndCB(XML_NODE_END_CB(cb));
	void cdataCB(XML_CDATA_CB(cb));
	void parseErrorCB(XML_PARSE_ERROR_CB(cb));
	void progressCB(XML_PROGRESS_CB(cb));

	int parse(void);

	int lineNr(void) const { return linenr; }

protected:

	bool isTokenChar(bool start, int c);
	int  readChar(int c);

private:

	bool skipping;
	int state;
	int linenr;
	int delim;

	std::string data, cdata, tag, attrn, attrv;
	std::map<std::string, std::string> attr;

	XML_NODE_START_CB(nodestartcb);
	XML_NODE_END_CB(nodeendcb);
	XML_CDATA_CB(cdatacb);
	XML_PARSE_ERROR_CB(parseerrorcb);
	XML_PROGRESS_CB(progresscb);

};

#endif // __XML_H

/*
 * svdrpservice.h: Public interface of the plugin's services
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _SVDRPSERVICE__H
#define _SVDRPSERVICE__H

#include <vdr/tools.h>

class cLine: public cListObject {
	private:
		char *Line;
	public:
		const char *Text() { return Line; }
		cLine(const char *s) { Line = s ? strdup(s) : NULL; };
		virtual ~cLine() { if (Line) free(Line); };
};

struct SvdrpConnection_v1_0 {
//in+out
	cString		serverIp;
	unsigned short	serverPort;
//in
	bool		shared;
//in+out
	int		handle;
};

struct SvdrpCommand_v1_0 {
//in
	cString		command;
	int		handle;
//out
	cList<cLine>	reply;
	unsigned short	responseCode;
};

#endif //_SVDRPSERVICE__H

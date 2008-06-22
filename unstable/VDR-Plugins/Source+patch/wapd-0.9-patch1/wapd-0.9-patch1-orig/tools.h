/*
 * tools.h: wapd server tools
 *
 * WAP remote control plugin for VDR (the Video Disk Recorder)
 * Copyright (C) 2002  Thomas Heiligenmann  <thomas@heiligenmann.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA
 *
 */


#ifndef __WAPD_TOOLS_H
#define __WAPD_TOOLS_H


#ifdef WAPD_HTML

#define MIME_TYPE                  "text/html"
#define WML_PROLOGUE()             "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n" \
   "<!DOCTYPE doctype PUBLIC \"-//w3c//dtd html 4.0 transitional//en\">\n"
#define WML_START(title)           "<html>\n<head>\n<title>" title "</title>\n</head>\n<body>\n"
#define WML_END()                  "</body>\n</html>\n"
#define WML_BR()                   "<br>\n"
#define WML_A(href,name)           "<a href=\"" href "\">" name "</a>"
#define WML_A_KEY(nr,href,ak,name) nr " <a href=\"" href "\" \"" ak "\" >" name "</a>"
#define WML_INPUT(name,value,fmt)  "<input name=\"" name "\" value=\"" value "\" type=\"text\"/>\n"

#else

#define MIME_TYPE                  "text/vnd.wap.wml"
#define WML_PROLOGUE()             "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n" \
  "<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML 1.3//EN\" \"http://www.wapforum.org/DTD/wml13.dtd\">\n"
#define WML_START(title)           "<wml><card title=\"" title "\"><p>\n"
#define WML_END()                  "</p>\n</card></wml>\n"
#define WML_BR()                   "<br/>\n"
#define WML_A(href,name)           "<a href=\"" href "\">" name "</a>"
#define WML_A_KEY(nr,href,ak,name) nr " <a href=\"" href "\" accesskey=\"" ak "\">" name "</a>"
#define WML_INPUT(name,value,fmt)  "<input name=\"" name "\" value=\"" value "\" format=\"" fmt "\"/>\n"

#endif //WAPD_HTML
char *base64decode(char *s);
char *unurilize(char *s);
char *wmlescape(char *s);

#endif //__WAPD_TOOLS_H

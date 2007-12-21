/*
 * global.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */


#ifndef _CTRLGLOBAL_H_
#define _CTRLGLOBAL_H_

//#define DEBUG

#ifdef DEBUG
#define DGATEWAY
#define DKEYBOARD
//#define DTELNET1
//#define DTELNET2
#endif


#define conESYSLOG( x... ) esyslog("[control] " x);
#define conISYSLOG( x... ) isyslog("[control] " x);
#define conDSYSLOG( x... ) dsyslog("[control] " x);

#define conTSYSLOG( x... ) printf("[control] " x); printf("\n"); fflush(stderr);



#ifdef DGATEWAY
#define conLOG_GATEW( x... ) conTSYSLOG(x)
#else
#define conLOG_GATEW( x... )
#endif

#ifdef DKEYBOARD
#define conLOG_KEYB( x... ) conTSYSLOG(x)
#else
#define conLOG_KEYB( x... )
#endif

#ifdef DSTACK
#define conLOG_STACK( x... ) conTSYSLOG(x)
#else
#define conLOG_STACK( x... )
#endif

#ifdef DTELNET1
#define conLOG_TELNET1( x... ) conTSYSLOG(x)
#else
#define conLOG_TELNET1( x... )
#endif

#ifdef DTELNET2
#define conLOG_TELNET2( x... ) conTSYSLOG(x)
#else
#define conLOG_TELNET2( x... )
#endif


#define assert(x)

#endif //_CTRLGLOBAL_H_



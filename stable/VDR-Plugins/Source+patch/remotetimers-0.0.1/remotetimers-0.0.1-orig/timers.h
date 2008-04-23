/*
 * svdrp.h: SVDRP interface for remote timers
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 */

#ifndef _REMOTETIMERS_TIMERS__H
#define _REMOTETIMERS_TIMERS__H

#include <stdlib.h>
#include "../svdrpservice/svdrpservice.h"
#include <vdr/tools.h>
#include <vdr/plugin.h>

class cRemoteTimer: public cTimer {
	private:
		int id;
		char *orig;
	public:
		cRemoteTimer();
		cRemoteTimer(const cEvent* &Event);
		virtual ~cRemoteTimer();
		cRemoteTimer& operator= (const cRemoteTimer &Timer);
		int Id() const { return id; }
		const char* Orig() const { return orig; }
		bool Parse(const char *s);
};

enum eRemoteTimersState { rtsOk, rtsRefresh, rtsLocked, rtsRecording, rtsRejected, rtsUpdated, rtsUnexpected, rtsConnError };

class cRemoteTimers: public cList<cRemoteTimer> {
	private:
		SvdrpConnection_v1_0    svdrp;
		cPlugin			*plugin;
		int			refcount;

		unsigned short CmdDELT(cRemoteTimer *Timer);
		unsigned short CmdLSTT(int Number, char*& TimerString);
		unsigned short CmdMODT(cRemoteTimer *Timer);
		unsigned short CmdNEWT(cRemoteTimer *Timer, int& Number);
	protected:
		eRemoteTimersState Verify(cRemoteTimer *Timer, char*& TimerString);
		eRemoteTimersState List(int Number, char*& TimerString);
	public:
		cRemoteTimers();
		~cRemoteTimers();
		bool Connect();
		void Disconnect();
		bool Offline() { return svdrp.handle == -1; }
		cString GetErrorMessage(eRemoteTimersState state);
		eRemoteTimersState Refresh();
		eRemoteTimersState Delete(cRemoteTimer *Timer);
		eRemoteTimersState Modify(cRemoteTimer *Timer);
		eRemoteTimersState New(cRemoteTimer *Timer);
		cRemoteTimer* GetMatch(const cEvent *Event, int *Match);
		cRemoteTimer* GetTimer(cTimer *Timer);
};

extern cRemoteTimers RemoteTimers;
#endif //_REMOTETIMERS_TIMERS__H

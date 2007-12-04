/*
 * $Id: control.h,v 1.2 2005/01/02 18:43:34 lordjaxom Exp $
 */

#ifndef VDR_TASTE_CONTROL_H
#define VDR_TASTE_CONTROL_H

#include <vdr/player.h>

class cControlTaste : public cControl {
private:
	static bool           mRequested;

  int                   mLastChannel;
	const cChannel       *mChannel;
	const cEvent         *mPresent;
	const cEvent         *mFollowing;
	uint64                mStart;
	bool                  mSwitch;
  cSkinDisplayChannel  *mOsd;

protected:
	virtual void Show(void);
  virtual void Hide(void) {}

public:
  cControlTaste(int Direction, const cChannel *Channel, const cEvent *Present, const cEvent *Following);
  ~cControlTaste();

  virtual eOSState ProcessKey(eKeys Key);

	static bool IsRequested(void) { bool r = mRequested; mRequested = false; return r; }
};

#endif // VDR_TASTE_CONTROL_H

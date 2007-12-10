/*
 * $Id: status.h,v 1.1.1.1 2004/11/19 16:45:31 lordjaxom Exp $
 */

#ifndef VDR_TASTE_STATUS_H
#define VDR_TASTE_STATUS_H

#include <vdr/status.h>

class cStatusTaste : public cStatus {
private:
  int mLastChannel;

protected:
  virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);

public:
  cStatusTaste(void);
};

#endif // VDR_TASTE_STATUS_H

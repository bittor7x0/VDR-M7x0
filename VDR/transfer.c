/*
 * transfer.c: Transfer mode
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */
#include <sys/ioctl.h>
#include "transfer.h"
#include "m7x0_dvb/dmx_ext.h"

//M7X0 BEGIN AK

// --- cTransfer -------------------------------------------------------------

cTransfer::cTransfer(const int PPid, const int VPid, const int APid, const int TPid)
:cReceiver(0, -1, 0, NULL,  NULL, NULL),
cPlayer(VPid ? pmTransferer : pmTransfererAudioOnly)
{
 pPid = PPid;
 vPid = VPid;
 aPid = APid;
 tPid = TPid;
}

cTransfer::~cTransfer()
{
  cReceiver::Detach();
  cPlayer::Detach();
}

void cTransfer::Activate(bool On)
{
  if (!On) {

     if (cReceiver::device){
        cReceiver::device->DelPid(cReceiver::device->pidHandles[cDevice::ptAudio].pid, cDevice::ptAudio);
        cReceiver::device->DelPid(cReceiver::device->pidHandles[cDevice::ptVideo].pid, cDevice::ptVideo);
        cReceiver::device->DelPid(cReceiver::device->pidHandles[cDevice::ptPcr].pid, cDevice::ptPcr);
        cReceiver::device->DelPid(cReceiver::device->pidHandles[cDevice::ptTeletext].pid, cDevice::ptTeletext);
        cReceiver::device->DelPid(cReceiver::device->pidHandles[cDevice::ptDolby].pid, cDevice::ptDolby);
        }
    if (cPlayer::device){
        cPlayer::device->StartLiveView(false,false);
        }
     }
  else if (cPlayer::device && cReceiver::device) {
     //cPlayer::device->SetAudioBypass(false);
     if (!(cReceiver::device->AddPid(pPid, cDevice::ptPcr) && cReceiver::device->AddPid(vPid, cDevice::ptVideo) && cReceiver::device->AddPid(aPid, cDevice::ptAudio))) {
        esyslog("ERROR: failed to set PIDs while activate Transferer");
        return;
        }
     cReceiver::device->AddPid(tPid, cDevice::ptTeletext);
     cPlayer::device->StartLiveView(true,false);
     cPlayer::device->EnsureAudioTrack(false);
     }
}
void cTransfer::SetAudioTrack(eTrackType Type, const tTrackId *TrackId)
{
  if (cPlayer::device && cReceiver::device) {
     cPlayer::device->StartLiveView(false,true);
     cReceiver::device->SetAudioTrackDevice(Type,TrackId);
     cPlayer::device->StartLiveView(true,true);
     if (IS_DOLBY_TRACK(Type)) {
        int diff; int stat;
        if (cReceiver::device->pidHandles[cDevice::ptPcr].handle >= 0) {
           stat = ioctl(cReceiver::device->pidHandles[cDevice::ptPcr].handle,DMX_GET_AUDIO_SYNC_DIFF,&diff);
           dsyslog("cTransfer DEBUG: AC3 Audio Snyc Diff ioctl-Status %d Difference %d", stat, diff);
           }
        }
     }
}



// --- cTransferControl ------------------------------------------------------

cDevice *cTransferControl::receiverDevice = NULL;

cTransferControl::cTransferControl(cDevice *ReceiverDevice, const int PPid, const int VPid, const int APid, const int TPid)
:cControl(transfer = new cTransfer(PPid, VPid, APid, TPid), true)
{
  ReceiverDevice->AttachReceiver(transfer);
  receiverDevice = ReceiverDevice;
}

cTransferControl::~cTransferControl()
{
  receiverDevice = NULL;
  delete transfer;
}

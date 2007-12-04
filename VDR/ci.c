/*
 * ci.c: Common Interface
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "ci.h"
#include <ctype.h>
//M7X0 BEGIN AK
#include "m7x0_dvb/ca.h"
//M7X0 END AK
#include <malloc.h>
#include <netinet/in.h>
#include <poll.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include "pat.h"
#include "tools.h"

/* these might come in handy in case you want to use this code without VDR's other files:
#ifndef MALLOC
#define MALLOC(type, size)  (type *)malloc(sizeof(type) * (size))
#endif

#ifndef esyslog
static int SysLogLevel = 3;
#define esyslog(a...) void( (SysLogLevel > 0) ? void(fprintf(stderr, a)), void(fprintf(stderr, "\n")) : void() )
#define isyslog(a...) void( (SysLogLevel > 1) ? void(fprintf(stderr, a)), void(fprintf(stderr, "\n")) : void() )
#define dsyslog(a...) void( (SysLogLevel > 2) ? void(fprintf(stderr, a)), void(fprintf(stderr, "\n")) : void() )
#endif
*/

// Set these to 'true' for debug output:
static bool DumpTPDUDataTransfer = false;
static bool DebugProtocol = false;

#define dbgprotocol(a...) if (DebugProtocol) fprintf(stderr, a)

#define OK       0
#define TIMEOUT -1
#define ERROR   -2

// --- Workarounds -----------------------------------------------------------

// The Irdeto AllCAM 4.7 (and maybe others, too) does not react on AOT_ENTER_MENU
// during the first few seconds of a newly established connection
#define WRKRND_TIME_BEFORE_ENTER_MENU  15 // seconds

// --- Helper functions ------------------------------------------------------

#define SIZE_INDICATOR 0x80

static const uint8_t *GetLength(const uint8_t *Data, int &Length)
///< Gets the length field from the beginning of Data.
///< \return Returns a pointer to the first byte after the length and
///< stores the length value in Length.
{
  Length = *Data++;
  if ((Length & SIZE_INDICATOR) != 0) {
     int l = Length & ~SIZE_INDICATOR;
     Length = 0;
     for (int i = 0; i < l; i++)
         Length = (Length << 8) | *Data++;
     }
  return Data;
}

static uint8_t *SetLength(uint8_t *Data, int Length)
///< Sets the length field at the beginning of Data.
///< \return Returns a pointer to the first byte after the length.
{
  uint8_t *p = Data;
  if (Length < 128)
     *p++ = Length;
  else {
     int n = sizeof(Length);
     for (int i = n - 1; i >= 0; i--) {
         int b = (Length >> (8 * i)) & 0xFF;
         if (p != Data || b)
            *++p = b;
         }
     *Data = (p - Data) | SIZE_INDICATOR;
     p++;
     }
  return p;
}

static char *CopyString(int Length, const uint8_t *Data)
///< Copies the string at Data.
///< \return Returns a pointer to a newly allocated string.
{
  // Some CAMs send funny characters at the beginning of strings.
  // Let's just skip them:
  while (Length > 0 && (*Data == ' ' || *Data == 0x05 || *Data == 0x96 || *Data == 0x97)) {
        Length--;
        Data++;
        }
  char *s = MALLOC(char, Length + 1);
  strncpy(s, (char *)Data, Length);
  s[Length] = 0;
  // The character 0x8A is used as newline, so let's put a real '\n' in there:
  strreplace(s, 0x8A, '\n');
  return s;
}

static char *GetString(int &Length, const uint8_t **Data)
///< Gets the string at Data.
///< \return Returns a pointer to a newly allocated string, or NULL in case of error.
///< Upon return Length and Data represent the remaining data after the string has been skipped.
{
  if (Length > 0 && Data && *Data) {
     int l = 0;
     const uint8_t *d = GetLength(*Data, l);
     char *s = CopyString(l, d);
     Length -= d - *Data + l;
     *Data = d + l;
     return s;
     }
  return NULL;
}

// --- cTPDU -----------------------------------------------------------------

#define MAX_TPDU_SIZE  2048
#define MAX_TPDU_DATA  (MAX_TPDU_SIZE - 4)

#define DATA_INDICATOR 0x80

#define T_SB           0x80
#define T_RCV          0x81
#define T_CREATE_TC    0x82
#define T_CTC_REPLY    0x83
#define T_DELETE_TC    0x84
#define T_DTC_REPLY    0x85
#define T_REQUEST_TC   0x86
#define T_NEW_TC       0x87
#define T_TC_ERROR     0x88
#define T_DATA_LAST    0xA0
#define T_DATA_MORE    0xA1

class cTPDU {
private:
  int size;
  uint8_t data[MAX_TPDU_SIZE];
  const uint8_t *GetData(const uint8_t *Data, int &Length);
public:
  cTPDU(void) { size = 0; }
  cTPDU(uint8_t Slot, uint8_t Tcid, uint8_t Tag, int Length = 0, const uint8_t *Data = NULL);
  uint8_t Slot(void) { return data[0]; }
  uint8_t Tcid(void) { return data[1]; }
  uint8_t Tag(void)  { return data[2]; }
  const uint8_t *Data(int &Length) { return GetData(data + 3, Length); }
  uint8_t Status(void);
  int Write(int fd);
  int Read(int fd);
  void Dump(int fd, bool Outgoing);
  };

cTPDU::cTPDU(uint8_t Slot, uint8_t Tcid, uint8_t Tag, int Length, const uint8_t *Data)
{
  size = 0;
  data[0] = Slot;
  data[1] = Tcid;
  data[2] = Tag;
  switch (Tag) {
    case T_RCV:
    case T_CREATE_TC:
    case T_CTC_REPLY:
    case T_DELETE_TC:
    case T_DTC_REPLY:
    case T_REQUEST_TC:
         data[3] = 1; // length
         data[4] = Tcid;
         size = 5;
         break;
    case T_NEW_TC:
    case T_TC_ERROR:
         if (Length == 1) {
            data[3] = 2; // length
            data[4] = Tcid;
            data[5] = Data[0];
            size = 6;
            }
         else
            esyslog("ERROR: invalid data length for TPDU tag 0x%02X: %d", Tag, Length);
         break;
    case T_DATA_LAST:
    case T_DATA_MORE:
         if (Length <= MAX_TPDU_DATA) {
            uint8_t *p = data + 3;
            p = SetLength(p, Length + 1);
            *p++ = Tcid;
            if (Length)
               memcpy(p, Data, Length);
            size = Length + (p - data);
            }
         else
            esyslog("ERROR: invalid data length for TPDU tag 0x%02X: %d", Tag, Length);
         break;
    default:
         esyslog("ERROR: unknown TPDU tag: 0x%02X", Tag);
    }
 }

int cTPDU::Write(int fd)
{
  Dump(fd, true);
  if (size)
     return safe_write(fd, data, size) == size ? OK : ERROR;
  esyslog("ERROR: attemp to write TPDU with zero size");
  return ERROR;
}

int cTPDU::Read(int fd)
{
  size = safe_read(fd, data, sizeof(data));
  if (size < 0) {
     esyslog("ERROR: %m");
     size = 0;
     return ERROR;
     }
  Dump(fd, false);
  return OK;
}

void cTPDU::Dump(int fd, bool Outgoing)
{
  if (DumpTPDUDataTransfer) {
#define MAX_DUMP 256
     fprintf(stderr, "%2d %s ", fd, Outgoing ? "-->" : "<--");
     for (int i = 0; i < size && i < MAX_DUMP; i++)
         fprintf(stderr, "%02X ", data[i]);
     fprintf(stderr, "%s\n", size >= MAX_DUMP ? "..." : "");
     if (!Outgoing) {
        fprintf(stderr, "      ");
        for (int i = 0; i < size && i < MAX_DUMP; i++)
            fprintf(stderr, "%2c ", isprint(data[i]) ? data[i] : '.');
        fprintf(stderr, "%s\n", size >= MAX_DUMP ? "..." : "");
        }
     }
}

const uint8_t *cTPDU::GetData(const uint8_t *Data, int &Length)
{
  if (size) {
     Data = GetLength(Data, Length);
     if (Length) {
        Length--; // the first byte is always the tcid
        return Data + 1;
        }
     }
  return NULL;
}

uint8_t cTPDU::Status(void)
{
  if (size >= 4 && data[size - 4] == T_SB && data[size - 3] == 2) {
     //XXX test tcid???
     return data[size - 1];
     }
  return 0;
}

// --- cCiTransportConnection ------------------------------------------------

enum eState { stIDLE, stCREATION, stACTIVE, stDELETION };

class cCiTransportConnection {
  friend class cCiTransportLayer;
private:
  int fd;
  uint8_t slot;
  uint8_t tcid;
  eState state;
  cTPDU *tpdu;
  int lastResponse;
  bool dataAvailable;
  void Init(int Fd, uint8_t Slot, uint8_t Tcid);
  int SendTPDU(uint8_t Tag, int Length = 0, const uint8_t *Data = NULL);
  int RecvTPDU(void);
  int CreateConnection(void);
  int Poll(void);
  eState State(void) { return state; }
  int LastResponse(void) { return lastResponse; }
  bool DataAvailable(void) { return dataAvailable; }
public:
  cCiTransportConnection(void);
  ~cCiTransportConnection();
  int Slot(void) const { return slot; }
  int SendData(int Length, const uint8_t *Data);
  int RecvData(void);
  const uint8_t *Data(int &Length);
  //XXX Close()
  void Reset(void);
  };

cCiTransportConnection::cCiTransportConnection(void)
{
  tpdu = NULL;
  Init(-1, 0, 0);
}

cCiTransportConnection::~cCiTransportConnection()
{
  delete tpdu;
}

void cCiTransportConnection::Init(int Fd, uint8_t Slot, uint8_t Tcid)
{
  fd = Fd;
  slot = Slot;
  tcid = Tcid;
  state = stIDLE;
  if (fd >= 0 && !tpdu)
     tpdu = new cTPDU;
  lastResponse = ERROR;
  dataAvailable = false;
//XXX Clear()???
}

int cCiTransportConnection::SendTPDU(uint8_t Tag, int Length, const uint8_t *Data)
{
  cTPDU TPDU(slot, tcid, Tag, Length, Data);
  return TPDU.Write(fd);
}

#define CAM_READ_TIMEOUT  3500 // ms

int cCiTransportConnection::RecvTPDU(void)
{
  struct pollfd pfd[1];
  pfd[0].fd = fd;
  pfd[0].events = POLLIN;
  lastResponse = ERROR;
  if (poll(pfd, 1, CAM_READ_TIMEOUT) > 0 && (pfd[0].revents & POLLIN) && tpdu->Read(fd) == OK && tpdu->Tcid() == tcid) {
     switch (state) {
       case stIDLE:     break;
       case stCREATION: if (tpdu->Tag() == T_CTC_REPLY) {
                           dataAvailable = tpdu->Status() & DATA_INDICATOR;
                           state = stACTIVE;
                           lastResponse = tpdu->Tag();
                           }
                        break;
       case stACTIVE:   switch (tpdu->Tag()) {
                          case T_SB:
                          case T_DATA_LAST:
                          case T_DATA_MORE:
                          case T_REQUEST_TC: break;
                          case T_DELETE_TC:  if (SendTPDU(T_DTC_REPLY) != OK)
                                                return ERROR;
                                             Init(fd, slot, tcid);
                                             break;
                          default: return ERROR;
                          }
                        dataAvailable = tpdu->Status() & DATA_INDICATOR;
                        lastResponse = tpdu->Tag();
                        break;
       case stDELETION: if (tpdu->Tag() == T_DTC_REPLY) {
                           Init(fd, slot, tcid);
                           //XXX Status()???
                           lastResponse = tpdu->Tag();
                           }
                        break;
       }
     }
  else if (FATALERRNO) {
     esyslog("ERROR: CAM: Read failed: slot %d, tcid %d - %m", slot, tcid);
     Init(-1, slot, tcid);
     }
  return lastResponse;
}

int cCiTransportConnection::SendData(int Length, const uint8_t *Data)
{
  while (state == stACTIVE && Length > 0) {
        uint8_t Tag = T_DATA_LAST;
        int l = Length;
        if (l > MAX_TPDU_DATA) {
           Tag = T_DATA_MORE;
           l = MAX_TPDU_DATA;
           }
        if (SendTPDU(Tag, l, Data) != OK || RecvTPDU() != T_SB)
           break;
        Length -= l;
        Data += l;
        }
  return Length ? ERROR : OK;
}

int cCiTransportConnection::RecvData(void)
{
  if (SendTPDU(T_RCV) == OK)
     return RecvTPDU();
  return ERROR;
}

const uint8_t *cCiTransportConnection::Data(int &Length)
{
  return tpdu->Data(Length);
}

#define MAX_CONNECT_RETRIES  2

int cCiTransportConnection::CreateConnection(void)
{
  if (state == stIDLE) {
     if (SendTPDU(T_CREATE_TC) == OK) {
        state = stCREATION;
        if (RecvTPDU() == T_CTC_REPLY)
           return OK;
        // the following is a workaround for CAMs that don't quite follow the specs...
        else {
           for (int i = 0; i < MAX_CONNECT_RETRIES; i++) {
               dsyslog("CAM: retrying to establish connection");
               if (RecvTPDU() == T_CTC_REPLY) {
                  dsyslog("CAM: connection established");
                  return OK;
                  }
               }
           return ERROR;
           }
        }
     }
  return ERROR;
}

int cCiTransportConnection::Poll(void)
{
  if (state == stACTIVE) {
     if (SendTPDU(T_DATA_LAST) == OK)
        return RecvTPDU();
     }
  return ERROR;
}

void cCiTransportConnection::Reset(void)
{
  Init(-1, 0, 0);
}

// --- cCiTransportLayer -----------------------------------------------------

#define MAX_CI_CONNECT  16 // maximum possible value is 254

class cCiTransportLayer {
private:
  int fd;
  int numSlots;
  cCiTransportConnection tc[MAX_CI_CONNECT];
public:
  cCiTransportLayer(int Fd, int NumSlots);
  cCiTransportConnection *NewConnection(int Slot);
  bool ResetSlot(int Slot, bool Wait = false);
  bool ModuleReady(int Slot);
  cCiTransportConnection *Process(int Slot);
  };

cCiTransportLayer::cCiTransportLayer(int Fd, int NumSlots)
{
  fd = Fd;
  numSlots = NumSlots;
  for (int s = 0; s < numSlots; s++)
      ResetSlot(s);
  cCondWait::SleepMs(2000);
}

cCiTransportConnection *cCiTransportLayer::NewConnection(int Slot)
{
  for (int i = 0; i < MAX_CI_CONNECT; i++) {
      if (tc[i].State() == stIDLE) {
         dbgprotocol("Creating connection: slot %d, tcid %d\n", Slot, i + 1);
         tc[i].Init(fd, Slot, i + 1);
         if (tc[i].CreateConnection() == OK)
            return &tc[i];
         break;
         }
      }
  return NULL;
}

bool cCiTransportLayer::ResetSlot(int Slot, bool Wait)
{
  for (int i = 0; i < MAX_CI_CONNECT; i++) {
      if (tc[i].State() != stIDLE && tc[i].Slot() == Slot)
         tc[i].Reset();
      }
  dbgprotocol("Resetting slot %d...", Slot);
  if (ioctl(fd, CA_RESET, 1 << Slot) != -1) {
     if (Wait)
        cCondWait::SleepMs(2000);
     dbgprotocol("ok.\n");
     return true;
     }
  else
     esyslog("ERROR: can't reset CAM slot %d: %m", Slot);
  dbgprotocol("failed!\n");
  return false;
}

bool cCiTransportLayer::ModuleReady(int Slot)
{
  ca_slot_info_t sinfo;
  sinfo.num = Slot;
  if (ioctl(fd, CA_GET_SLOT_INFO, &sinfo) != -1)
     return sinfo.flags & CA_CI_MODULE_READY;
  else
     esyslog("ERROR: can't get info on CAM slot %d: %m", Slot);
  return false;
}

cCiTransportConnection *cCiTransportLayer::Process(int Slot)
{
  for (int i = 0; i < MAX_CI_CONNECT; i++) {
      cCiTransportConnection *Tc = &tc[i];
      if (Tc->Slot() == Slot) {
         switch (Tc->State()) {
           case stCREATION:
           case stACTIVE:
                if (!Tc->DataAvailable()) {
                   if (Tc->Poll() != OK)
                      ;//XXX continue;
                   }
                switch (Tc->LastResponse()) {
                  case T_REQUEST_TC:
                       //XXX
                       break;
                  case T_DATA_MORE:
                  case T_DATA_LAST:
                  case T_CTC_REPLY:
                  case T_SB:
                       if (Tc->DataAvailable())
                          Tc->RecvData();
                       break;
                  case TIMEOUT:
                  case ERROR:
                  default:
                       //XXX Tc->state = stIDLE;//XXX Init()???
                       return NULL;
                       break;
                  }
                //XXX this will only work with _one_ transport connection per slot!
                return Tc;
                break;
           default: ;
           }
         }
      }
  return NULL;
}

// -- cCiSession -------------------------------------------------------------

// Session Tags:

#define ST_SESSION_NUMBER           0x90
#define ST_OPEN_SESSION_REQUEST     0x91
#define ST_OPEN_SESSION_RESPONSE    0x92
#define ST_CREATE_SESSION           0x93
#define ST_CREATE_SESSION_RESPONSE  0x94
#define ST_CLOSE_SESSION_REQUEST    0x95
#define ST_CLOSE_SESSION_RESPONSE   0x96

// Session Status:

#define SS_OK             0x00
#define SS_NOT_ALLOCATED  0xF0

// Resource Identifiers:

#define RI_RESOURCE_MANAGER            0x00010041
#define RI_APPLICATION_INFORMATION     0x00020041
#define RI_CONDITIONAL_ACCESS_SUPPORT  0x00030041
#define RI_HOST_CONTROL                0x00200041
#define RI_DATE_TIME                   0x00240041
#define RI_MMI                         0x00400041

// Application Object Tags:

#define AOT_NONE                    0x000000
#define AOT_PROFILE_ENQ             0x9F8010
#define AOT_PROFILE                 0x9F8011
#define AOT_PROFILE_CHANGE          0x9F8012
#define AOT_APPLICATION_INFO_ENQ    0x9F8020
#define AOT_APPLICATION_INFO        0x9F8021
#define AOT_ENTER_MENU              0x9F8022
#define AOT_CA_INFO_ENQ             0x9F8030
#define AOT_CA_INFO                 0x9F8031
#define AOT_CA_PMT                  0x9F8032
#define AOT_CA_PMT_REPLY            0x9F8033
#define AOT_TUNE                    0x9F8400
#define AOT_REPLACE                 0x9F8401
#define AOT_CLEAR_REPLACE           0x9F8402
#define AOT_ASK_RELEASE             0x9F8403
#define AOT_DATE_TIME_ENQ           0x9F8440
#define AOT_DATE_TIME               0x9F8441
#define AOT_CLOSE_MMI               0x9F8800
#define AOT_DISPLAY_CONTROL         0x9F8801
#define AOT_DISPLAY_REPLY           0x9F8802
#define AOT_TEXT_LAST               0x9F8803
#define AOT_TEXT_MORE               0x9F8804
#define AOT_KEYPAD_CONTROL          0x9F8805
#define AOT_KEYPRESS                0x9F8806
#define AOT_ENQ                     0x9F8807
#define AOT_ANSW                    0x9F8808
#define AOT_MENU_LAST               0x9F8809
#define AOT_MENU_MORE               0x9F880A
#define AOT_MENU_ANSW               0x9F880B
#define AOT_LIST_LAST               0x9F880C
#define AOT_LIST_MORE               0x9F880D
#define AOT_SUBTITLE_SEGMENT_LAST   0x9F880E
#define AOT_SUBTITLE_SEGMENT_MORE   0x9F880F
#define AOT_DISPLAY_MESSAGE         0x9F8810
#define AOT_SCENE_END_MARK          0x9F8811
#define AOT_SCENE_DONE              0x9F8812
#define AOT_SCENE_CONTROL           0x9F8813
#define AOT_SUBTITLE_DOWNLOAD_LAST  0x9F8814
#define AOT_SUBTITLE_DOWNLOAD_MORE  0x9F8815
#define AOT_FLUSH_DOWNLOAD          0x9F8816
#define AOT_DOWNLOAD_REPLY          0x9F8817
#define AOT_COMMS_CMD               0x9F8C00
#define AOT_CONNECTION_DESCRIPTOR   0x9F8C01
#define AOT_COMMS_REPLY             0x9F8C02
#define AOT_COMMS_SEND_LAST         0x9F8C03
#define AOT_COMMS_SEND_MORE         0x9F8C04
#define AOT_COMMS_RCV_LAST          0x9F8C05
#define AOT_COMMS_RCV_MORE          0x9F8C06

class cCiSession {
private:
  uint16_t sessionId;
  uint32_t resourceId;
  cCiTransportConnection *tc;
protected:
  int GetTag(int &Length, const uint8_t **Data);
  const uint8_t *GetData(const uint8_t *Data, int &Length);
  int SendData(int Tag, int Length = 0, const uint8_t *Data = NULL);
public:
  cCiSession(uint16_t SessionId, uint32_t ResourceId, cCiTransportConnection *Tc);
  virtual ~cCiSession();
  const cCiTransportConnection *Tc(void) { return tc; }
  uint16_t SessionId(void) { return sessionId; }
  uint32_t ResourceId(void) { return resourceId; }
  virtual bool HasUserIO(void) { return false; }
  virtual bool Process(int Length = 0, const uint8_t *Data = NULL);
  };

cCiSession::cCiSession(uint16_t SessionId, uint32_t ResourceId, cCiTransportConnection *Tc)
{
  sessionId = SessionId;
  resourceId = ResourceId;
  tc = Tc;
}

cCiSession::~cCiSession()
{
}

int cCiSession::GetTag(int &Length, const uint8_t **Data)
///< Gets the tag at Data.
///< \return Returns the actual tag, or AOT_NONE in case of error.
///< Upon return Length and Data represent the remaining data after the tag has been skipped.
{
  if (Length >= 3 && Data && *Data) {
     int t = 0;
     for (int i = 0; i < 3; i++)
         t = (t << 8) | *(*Data)++;
     Length -= 3;
     return t;
     }
  return AOT_NONE;
}

const uint8_t *cCiSession::GetData(const uint8_t *Data, int &Length)
{
  Data = GetLength(Data, Length);
  return Length ? Data : NULL;
}

int cCiSession::SendData(int Tag, int Length, const uint8_t *Data)
{
  uint8_t buffer[2048];
  uint8_t *p = buffer;
  *p++ = ST_SESSION_NUMBER;
  *p++ = 0x02;
  *p++ = (sessionId >> 8) & 0xFF;
  *p++ =  sessionId       & 0xFF;
  *p++ = (Tag >> 16) & 0xFF;
  *p++ = (Tag >>  8) & 0xFF;
  *p++ =  Tag        & 0xFF;
  p = SetLength(p, Length);
  if (p - buffer + Length < int(sizeof(buffer))) {
     memcpy(p, Data, Length);
     p += Length;
     return tc->SendData(p - buffer, buffer);
     }
  esyslog("ERROR: CAM: data length (%d) exceeds buffer size", Length);
  return ERROR;
}

bool cCiSession::Process(int Length, const uint8_t *Data)
{
  return true;
}

// -- cCiResourceManager -----------------------------------------------------

class cCiResourceManager : public cCiSession {
private:
  int state;
public:
  cCiResourceManager(uint16_t SessionId, cCiTransportConnection *Tc);
  virtual bool Process(int Length = 0, const uint8_t *Data = NULL);
  };

cCiResourceManager::cCiResourceManager(uint16_t SessionId, cCiTransportConnection *Tc)
:cCiSession(SessionId, RI_RESOURCE_MANAGER, Tc)
{
  dbgprotocol("New Resource Manager (session id %d)\n", SessionId);
  state = 0;
}

bool cCiResourceManager::Process(int Length, const uint8_t *Data)
{
  if (Data) {
     int Tag = GetTag(Length, &Data);
     switch (Tag) {
       case AOT_PROFILE_ENQ: {
            dbgprotocol("%d: <== Profile Enquiry\n", SessionId());
            uint32_t resources[] = { htonl(RI_RESOURCE_MANAGER),
                                     htonl(RI_APPLICATION_INFORMATION),
                                     htonl(RI_CONDITIONAL_ACCESS_SUPPORT),
                                     htonl(RI_DATE_TIME),
                                     htonl(RI_MMI)
                                   };
            dbgprotocol("%d: ==> Profile\n", SessionId());
            SendData(AOT_PROFILE, sizeof(resources), (uint8_t*)resources);
            state = 3;
            }
            break;
       case AOT_PROFILE: {
            dbgprotocol("%d: <== Profile\n", SessionId());
            if (state == 1) {
               int l = 0;
               const uint8_t *d = GetData(Data, l);
               if (l > 0 && d)
                  esyslog("CI resource manager: unexpected data");
               dbgprotocol("%d: ==> Profile Change\n", SessionId());
               SendData(AOT_PROFILE_CHANGE);
               state = 2;
               }
            else {
               esyslog("ERROR: CI resource manager: unexpected tag %06X in state %d", Tag, state);
               }
            }
            break;
       default: esyslog("ERROR: CI resource manager: unknown tag %06X", Tag);
                return false;
       }
     }
  else if (state == 0) {
     dbgprotocol("%d: ==> Profile Enq\n", SessionId());
     SendData(AOT_PROFILE_ENQ);
     state = 1;
     }
  return true;
}

// --- cCiApplicationInformation ---------------------------------------------

class cCiApplicationInformation : public cCiSession {
private:
  int state;
  time_t creationTime;
  uint8_t applicationType;
  uint16_t applicationManufacturer;
  uint16_t manufacturerCode;
  char *menuString;
public:
  cCiApplicationInformation(uint16_t SessionId, cCiTransportConnection *Tc);
  virtual ~cCiApplicationInformation();
  virtual bool Process(int Length = 0, const uint8_t *Data = NULL);
  bool EnterMenu(void);
  const char *GetMenuString(void) { return menuString; }
  };

cCiApplicationInformation::cCiApplicationInformation(uint16_t SessionId, cCiTransportConnection *Tc)
:cCiSession(SessionId, RI_APPLICATION_INFORMATION, Tc)
{
  dbgprotocol("New Application Information (session id %d)\n", SessionId);
  state = 0;
  creationTime = time(NULL);
  menuString = NULL;
}

cCiApplicationInformation::~cCiApplicationInformation()
{
  free(menuString);
}

bool cCiApplicationInformation::Process(int Length, const uint8_t *Data)
{
  if (Data) {
     int Tag = GetTag(Length, &Data);
     switch (Tag) {
       case AOT_APPLICATION_INFO: {
            dbgprotocol("%d: <== Application Info\n", SessionId());
            int l = 0;
            const uint8_t *d = GetData(Data, l);
            if ((l -= 1) < 0) break;
            applicationType = *d++;
            if ((l -= 2) < 0) break;
            applicationManufacturer = ntohs(get_unaligned((uint16_t *)d));
            d += 2;
            if ((l -= 2) < 0) break;
            manufacturerCode = ntohs(get_unaligned((uint16_t *)d));
            d += 2;
            free(menuString);
            menuString = GetString(l, &d);
            isyslog("CAM: %s, %02X, %04X, %04X", menuString, applicationType, applicationManufacturer, manufacturerCode);//XXX make externally accessible!
            }
            state = 2;
            break;
       default: esyslog("ERROR: CI application information: unknown tag %06X", Tag);
                return false;
       }
     }
  else if (state == 0) {
     dbgprotocol("%d: ==> Application Info Enq\n", SessionId());
     SendData(AOT_APPLICATION_INFO_ENQ);
     state = 1;
     }
  return true;
}

bool cCiApplicationInformation::EnterMenu(void)
{
  if (state == 2 && time(NULL) - creationTime > WRKRND_TIME_BEFORE_ENTER_MENU) {
     dbgprotocol("%d: ==> Enter Menu\n", SessionId());
     SendData(AOT_ENTER_MENU);
     return true;//XXX
     }
  return false;
}

// --- cCiCaPmt --------------------------------------------------------------

// Ca Pmt List Management:

#define CPLM_MORE    0x00
#define CPLM_FIRST   0x01
#define CPLM_LAST    0x02
#define CPLM_ONLY    0x03
#define CPLM_ADD     0x04
#define CPLM_UPDATE  0x05

// Ca Pmt Cmd Ids:

#define CPCI_OK_DESCRAMBLING  0x01
#define CPCI_OK_MMI           0x02
#define CPCI_QUERY            0x03
#define CPCI_NOT_SELECTED     0x04

class cCiCaPmt : public cListObject {
  friend class cCiConditionalAccessSupport;
private:
  uint8_t cmdId;
  int length;
  int esInfoLengthPos;
  uint8_t capmt[2048]; ///< XXX is there a specified maximum?
  int caDescriptorsLength;
  uint8_t caDescriptors[2048];
  bool streamFlag;
  void AddCaDescriptors(int Length, const uint8_t *Data);
public:
  cCiCaPmt(uint8_t CmdId, int Source, int Transponder, int ProgramNumber, const unsigned short *CaSystemIds);
  void SetListManagement(uint8_t ListManagement);
  bool Valid(void);
  void AddPid(int Pid, uint8_t StreamType);
  };

cCiCaPmt::cCiCaPmt(uint8_t CmdId, int Source, int Transponder, int ProgramNumber, const unsigned short *CaSystemIds)
{
  cmdId = CmdId;
  caDescriptorsLength = GetCaDescriptors(Source, Transponder, ProgramNumber, CaSystemIds, sizeof(caDescriptors), caDescriptors, streamFlag);
  length = 0;
  capmt[length++] = CPLM_ONLY;
  capmt[length++] = (ProgramNumber >> 8) & 0xFF;
  capmt[length++] =  ProgramNumber       & 0xFF;
  capmt[length++] = 0x01; // version_number, current_next_indicator - apparently vn doesn't matter, but cni must be 1
  esInfoLengthPos = length;
  capmt[length++] = 0x00; // program_info_length H (at program level)
  capmt[length++] = 0x00; // program_info_length L
  if (!streamFlag)
     AddCaDescriptors(caDescriptorsLength, caDescriptors);
}

void cCiCaPmt::SetListManagement(uint8_t ListManagement)
{
  capmt[0] = ListManagement;
}

bool cCiCaPmt::Valid(void)
{
  return caDescriptorsLength > 0;
}

void cCiCaPmt::AddPid(int Pid, uint8_t StreamType)
{
  if (Pid) {
     //XXX buffer overflow check???
     capmt[length++] = StreamType;
     capmt[length++] = (Pid >> 8) & 0xFF;
     capmt[length++] =  Pid       & 0xFF;
     esInfoLengthPos = length;
     capmt[length++] = 0x00; // ES_info_length H (at ES level)
     capmt[length++] = 0x00; // ES_info_length L
     if (streamFlag)
        AddCaDescriptors(caDescriptorsLength, caDescriptors);
     }
}

void cCiCaPmt::AddCaDescriptors(int Length, const uint8_t *Data)
{
  if (esInfoLengthPos) {
     if (length + Length < int(sizeof(capmt))) {
        capmt[length++] = cmdId;
        memcpy(capmt + length, Data, Length);
        length += Length;
        int l = length - esInfoLengthPos - 2;
        capmt[esInfoLengthPos]     = (l >> 8) & 0xFF;
        capmt[esInfoLengthPos + 1] =  l       & 0xFF;
        }
     else
        esyslog("ERROR: buffer overflow in CA descriptor");
     esInfoLengthPos = 0;
     }
  else
     esyslog("ERROR: adding CA descriptor without Pid!");
}

// --- cCiConditionalAccessSupport -------------------------------------------

#define MAXCASYSTEMIDS 64

// CA Enable Ids:

#define CAEI_POSSIBLE                  0x01
#define CAEI_POSSIBLE_COND_PURCHASE    0x02
#define CAEI_POSSIBLE_COND_TECHNICAL   0x03
#define CAEI_NOT_POSSIBLE_ENTITLEMENT  0x71
#define CAEI_NOT_POSSIBLE_TECHNICAL    0x73

#define CA_ENABLE_FLAG                 0x80

#define CA_ENABLE(x) (((x) & CA_ENABLE_FLAG) ? (x) & ~CA_ENABLE_FLAG : 0)

class cCiConditionalAccessSupport : public cCiSession {
private:
  int state;
  int numCaSystemIds;
  unsigned short caSystemIds[MAXCASYSTEMIDS + 1]; // list is zero terminated!
public:
  cCiConditionalAccessSupport(uint16_t SessionId, cCiTransportConnection *Tc);
  virtual bool Process(int Length = 0, const uint8_t *Data = NULL);
  const unsigned short *GetCaSystemIds(void) { return caSystemIds; }
  bool SendPMT(cCiCaPmt *CaPmt);
  bool ReceivedReply(bool CanDescramble = false);
  };

cCiConditionalAccessSupport::cCiConditionalAccessSupport(uint16_t SessionId, cCiTransportConnection *Tc)
:cCiSession(SessionId, RI_CONDITIONAL_ACCESS_SUPPORT, Tc)
{
  dbgprotocol("New Conditional Access Support (session id %d)\n", SessionId);
  state = 0; // inactive
  caSystemIds[numCaSystemIds = 0] = 0;
}

bool cCiConditionalAccessSupport::Process(int Length, const uint8_t *Data)
{
  if (Data) {
     int Tag = GetTag(Length, &Data);
     switch (Tag) {
       case AOT_CA_INFO: {
            dbgprotocol("%d: <== Ca Info", SessionId());
            numCaSystemIds = 0;
            int l = 0;
            const uint8_t *d = GetData(Data, l);
            while (l > 1) {
                  unsigned short id = ((unsigned short)(*d) << 8) | *(d + 1);
                  dbgprotocol(" %04X", id);
                  d += 2;
                  l -= 2;
                  if (numCaSystemIds < MAXCASYSTEMIDS)
                     caSystemIds[numCaSystemIds++] = id;
                  else {
                     esyslog("ERROR: too many CA system IDs!");
                     break;
                     }
                  }
            caSystemIds[numCaSystemIds] = 0;
            dbgprotocol("\n");
            }
            state = 2; // got ca info
            break;
       case AOT_CA_PMT_REPLY: {
            dbgprotocol("%d: <== Ca Pmt Reply", SessionId());
            state = 4; // got ca pmt reply
            int l = 0;
            const uint8_t *d = GetData(Data, l);
            if (l > 1) {
               unsigned short pnr = ((unsigned short)(*d) << 8) | *(d + 1);
               dbgprotocol(" %d", pnr);
               d += 2;
               l -= 2;
               if (l > 0) {
                  dbgprotocol(" %02X", *d);
                  d += 1;
                  l -= 1;
                  if (l > 0) {
                     if (l % 3 == 0 && l > 1) {
                        // The EN50221 standard defines that the next byte is supposed
                        // to be the CA_enable value at programme level. However, there are
                        // CAMs (for instance the AlphaCrypt with firmware <= 3.05) that
                        // insert a two byte length field here.
                        // This is a workaround to skip this length field:
                        unsigned short len = ((unsigned short)(*d) << 8) | *(d + 1);
                        if (len == l - 2) {
                           d += 2;
                           l -= 2;
                           }
                        }
                     unsigned char caepl = *d;
                     dbgprotocol(" %02X", caepl);
                     d += 1;
                     l -= 1;
                     bool ok = true;
                     if (l <= 2)
                        ok = CA_ENABLE(caepl) == CAEI_POSSIBLE;
                     while (l > 2) {
                           unsigned short pid = ((unsigned short)(*d) << 8) | *(d + 1);
                           unsigned char caees = *(d + 2);
                           dbgprotocol(" %d=%02X", pid, caees);
                           d += 3;
                           l -= 3;
                           if (CA_ENABLE(caees) != CAEI_POSSIBLE)
                              ok = false;
                           }
                     if (ok)
                        state = 5; // descrambling possible
                     }
                  }
               }
            dbgprotocol("\n");
            }
            break;
       default: esyslog("ERROR: CI conditional access support: unknown tag %06X", Tag);
                return false;
       }
     }
  else if (state == 0) {
     dbgprotocol("%d: ==> Ca Info Enq\n", SessionId());
     SendData(AOT_CA_INFO_ENQ);
     state = 1; // enquired ca info
     }
  return true;
}

bool cCiConditionalAccessSupport::SendPMT(cCiCaPmt *CaPmt)
{
  if (CaPmt && state >= 2) {
     dbgprotocol("%d: ==> Ca Pmt\n", SessionId());
     SendData(AOT_CA_PMT, CaPmt->length, CaPmt->capmt);
     state = 3; // sent ca pmt
     return true;
     }
  return false;
}

bool cCiConditionalAccessSupport::ReceivedReply(bool CanDescramble)
{
  return state >= (CanDescramble ? 5 : 4);
}

// --- cCiDateTime -----------------------------------------------------------

class cCiDateTime : public cCiSession {
private:
  int interval;
  time_t lastTime;
  bool SendDateTime(void);
public:
  cCiDateTime(uint16_t SessionId, cCiTransportConnection *Tc);
  virtual bool Process(int Length = 0, const uint8_t *Data = NULL);
  };

cCiDateTime::cCiDateTime(uint16_t SessionId, cCiTransportConnection *Tc)
:cCiSession(SessionId, RI_DATE_TIME, Tc)
{
  interval = 0;
  lastTime = 0;
  dbgprotocol("New Date Time (session id %d)\n", SessionId);
}

bool cCiDateTime::SendDateTime(void)
{
  time_t t = time(NULL);
  struct tm tm_gmt;
  struct tm tm_loc;
  if (gmtime_r(&t, &tm_gmt) && localtime_r(&t, &tm_loc)) {
     int Y = tm_gmt.tm_year;
     int M = tm_gmt.tm_mon + 1;
     int D = tm_gmt.tm_mday;
     int L = (M == 1 || M == 2) ? 1 : 0;
     int MJD = 14956 + D + int((Y - L) * 365.25) + int((M + 1 + L * 12) * 30.6001);
#define DEC2BCD(d) (((d / 10) << 4) + (d % 10))
     struct tTime { unsigned short mjd; uint8_t h, m, s; short offset; };
     tTime T = { mjd : htons(MJD), h : DEC2BCD(tm_gmt.tm_hour), m : DEC2BCD(tm_gmt.tm_min), s : DEC2BCD(tm_gmt.tm_sec), offset : htons(tm_loc.tm_gmtoff / 60) };
     dbgprotocol("%d: ==> Date Time\n", SessionId());
     SendData(AOT_DATE_TIME, 7, (uint8_t*)&T);
     //XXX return value of all SendData() calls???
     return true;
     }
  return false;
}

bool cCiDateTime::Process(int Length, const uint8_t *Data)
{
  if (Data) {
     int Tag = GetTag(Length, &Data);
     switch (Tag) {
       case AOT_DATE_TIME_ENQ: {
            interval = 0;
            int l = 0;
            const uint8_t *d = GetData(Data, l);
            if (l > 0)
               interval = *d;
            dbgprotocol("%d: <== Date Time Enq, interval = %d\n", SessionId(), interval);
            lastTime = time(NULL);
            return SendDateTime();
            }
            break;
       default: esyslog("ERROR: CI date time: unknown tag %06X", Tag);
                return false;
       }
     }
  else if (interval && time(NULL) - lastTime > interval) {
     lastTime = time(NULL);
     return SendDateTime();
     }
  return true;
}

// --- cCiMMI ----------------------------------------------------------------

// Display Control Commands:

#define DCC_SET_MMI_MODE                          0x01
#define DCC_DISPLAY_CHARACTER_TABLE_LIST          0x02
#define DCC_INPUT_CHARACTER_TABLE_LIST            0x03
#define DCC_OVERLAY_GRAPHICS_CHARACTERISTICS      0x04
#define DCC_FULL_SCREEN_GRAPHICS_CHARACTERISTICS  0x05

// MMI Modes:

#define MM_HIGH_LEVEL                      0x01
#define MM_LOW_LEVEL_OVERLAY_GRAPHICS      0x02
#define MM_LOW_LEVEL_FULL_SCREEN_GRAPHICS  0x03

// Display Reply IDs:

#define DRI_MMI_MODE_ACK                              0x01
#define DRI_LIST_DISPLAY_CHARACTER_TABLES             0x02
#define DRI_LIST_INPUT_CHARACTER_TABLES               0x03
#define DRI_LIST_GRAPHIC_OVERLAY_CHARACTERISTICS      0x04
#define DRI_LIST_FULL_SCREEN_GRAPHIC_CHARACTERISTICS  0x05
#define DRI_UNKNOWN_DISPLAY_CONTROL_CMD               0xF0
#define DRI_UNKNOWN_MMI_MODE                          0xF1
#define DRI_UNKNOWN_CHARACTER_TABLE                   0xF2

// Enquiry Flags:

#define EF_BLIND  0x01

// Answer IDs:

#define AI_CANCEL  0x00
#define AI_ANSWER  0x01

class cCiMMI : public cCiSession {
private:
  char *GetText(int &Length, const uint8_t **Data);
  cCiMenu *menu, *fetchedMenu;
  cCiEnquiry *enquiry, *fetchedEnquiry;
public:
  cCiMMI(uint16_t SessionId, cCiTransportConnection *Tc);
  virtual ~cCiMMI();
  virtual bool Process(int Length = 0, const uint8_t *Data = NULL);
  virtual bool HasUserIO(void) { return menu || enquiry; }
  cCiMenu *Menu(bool Clear = false);
  cCiEnquiry *Enquiry(bool Clear = false);
  bool SendMenuAnswer(uint8_t Selection);
  bool SendAnswer(const char *Text);
  bool SendCloseMMI(void);
  };

cCiMMI::cCiMMI(uint16_t SessionId, cCiTransportConnection *Tc)
:cCiSession(SessionId, RI_MMI, Tc)
{
  dbgprotocol("New MMI (session id %d)\n", SessionId);
  menu = fetchedMenu = NULL;
  enquiry = fetchedEnquiry = NULL;
}

cCiMMI::~cCiMMI()
{
  if (fetchedMenu) {
     cMutexLock MutexLock(fetchedMenu->mutex);
     fetchedMenu->mmi = NULL;
     }
  delete menu;
  if (fetchedEnquiry) {
     cMutexLock MutexLock(fetchedEnquiry->mutex);
     fetchedEnquiry->mmi = NULL;
     }
  delete enquiry;
}

char *cCiMMI::GetText(int &Length, const uint8_t **Data)
///< Gets the text at Data.
///< \return Returns a pointer to a newly allocated string, or NULL in case of error.
///< Upon return Length and Data represent the remaining data after the text has been skipped.
{
  int Tag = GetTag(Length, Data);
  if (Tag == AOT_TEXT_LAST) {
     char *s = GetString(Length, Data);
     dbgprotocol("%d: <== Text Last '%s'\n", SessionId(), s);
     return s;
     }
  else
     esyslog("CI MMI: unexpected text tag: %06X", Tag);
  return NULL;
}

bool cCiMMI::Process(int Length, const uint8_t *Data)
{
  if (Data) {
     int Tag = GetTag(Length, &Data);
     switch (Tag) {
       case AOT_DISPLAY_CONTROL: {
            dbgprotocol("%d: <== Display Control\n", SessionId());
            int l = 0;
            const uint8_t *d = GetData(Data, l);
            if (l > 0) {
               switch (*d) {
                 case DCC_SET_MMI_MODE:
                      if (l == 2 && *++d == MM_HIGH_LEVEL) {
                         struct tDisplayReply { uint8_t id; uint8_t mode; };
                         tDisplayReply dr = { id : DRI_MMI_MODE_ACK, mode : MM_HIGH_LEVEL };
                         dbgprotocol("%d: ==> Display Reply\n", SessionId());
                         SendData(AOT_DISPLAY_REPLY, 2, (uint8_t *)&dr);
                         }
                      break;
                 default: esyslog("CI MMI: unsupported display control command %02X", *d);
                          return false;
                 }
               }
            }
            break;
       case AOT_LIST_LAST:
       case AOT_MENU_LAST: {
            dbgprotocol("%d: <== Menu Last\n", SessionId());
            delete menu;
            menu = new cCiMenu(this, Tag == AOT_MENU_LAST);
            int l = 0;
            const uint8_t *d = GetData(Data, l);
            if (l > 0) {
               // since the specification allows choiceNb to be undefined it is useless, so let's just skip it:
               d++;
               l--;
               if (l > 0) menu->titleText = GetText(l, &d);
               if (l > 0) menu->subTitleText = GetText(l, &d);
               if (l > 0) menu->bottomText = GetText(l, &d);
               while (l > 0) {
                     char *s = GetText(l, &d);
                     if (s) {
                        if (!menu->AddEntry(s))
                           free(s);
                        }
                     else
                        break;
                     }
               }
            }
            break;
       case AOT_ENQ: {
            dbgprotocol("%d: <== Enq\n", SessionId());
            delete enquiry;
            enquiry = new cCiEnquiry(this);
            int l = 0;
            const uint8_t *d = GetData(Data, l);
            if (l > 0) {
               uint8_t blind = *d++;
               //XXX GetByte()???
               l--;
               enquiry->blind = blind & EF_BLIND;
               enquiry->expectedLength = *d++;
               l--;
               // I really wonder why there is no text length field here...
               enquiry->text = CopyString(l, d);
               }
            }
            break;
       case AOT_CLOSE_MMI: {
            int id = -1;
            int delay = -1;
            int l = 0;
            const uint8_t *d = GetData(Data, l);
            if (l > 0) {
               id = *d++;
               if (l > 1)
                  delay = *d;
               }
            dbgprotocol("%d: <== Close MMI  id = %02X  delay = %d\n", SessionId(), id, delay);
            }
            break;
       default: esyslog("ERROR: CI MMI: unknown tag %06X", Tag);
                return false;
       }
     }
  return true;
}

cCiMenu *cCiMMI::Menu(bool Clear)
{
  if (Clear)
     fetchedMenu = NULL;
  else if (menu) {
     fetchedMenu = menu;
     menu = NULL;
     }
  return fetchedMenu;
}

cCiEnquiry *cCiMMI::Enquiry(bool Clear)
{
  if (Clear)
     fetchedEnquiry = NULL;
  else if (enquiry) {
     fetchedEnquiry = enquiry;
     enquiry = NULL;
     }
  return fetchedEnquiry;
}

bool cCiMMI::SendMenuAnswer(uint8_t Selection)
{
  dbgprotocol("%d: ==> Menu Answ\n", SessionId());
  SendData(AOT_MENU_ANSW, 1, &Selection);
  //XXX return value of all SendData() calls???
  return true;
}

bool cCiMMI::SendAnswer(const char *Text)
{
  dbgprotocol("%d: ==> Answ\n", SessionId());
  struct tAnswer { uint8_t id; char text[256]; };//XXX
  tAnswer answer;
  answer.id = Text ? AI_ANSWER : AI_CANCEL;
  if (Text)
     strncpy(answer.text, Text, sizeof(answer.text));
  SendData(AOT_ANSW, Text ? strlen(Text) + 1 : 1, (uint8_t *)&answer);
  //XXX return value of all SendData() calls???
  return true;
}

bool cCiMMI::SendCloseMMI(void)
{
  dbgprotocol("%d: ==> Close MMI\n", SessionId());
  SendData(AOT_CLOSE_MMI, 0);
  //XXX return value of all SendData() calls???
  return true;
}

// --- cCiMenu ---------------------------------------------------------------

cCiMenu::cCiMenu(cCiMMI *MMI, bool Selectable)
{
  mmi = MMI;
  mutex = NULL;
  selectable = Selectable;
  titleText = subTitleText = bottomText = NULL;
  numEntries = 0;
}

cCiMenu::~cCiMenu()
{
  cMutexLock MutexLock(mutex);
  if (mmi)
     mmi->Menu(true);
  free(titleText);
  free(subTitleText);
  free(bottomText);
  for (int i = 0; i < numEntries; i++)
      free(entries[i]);
}

bool cCiMenu::AddEntry(char *s)
{
  if (numEntries < MAX_CIMENU_ENTRIES) {
     entries[numEntries++] = s;
     return true;
     }
  return false;
}

bool cCiMenu::HasUpdate(void)
{
  // If the mmi is gone, the menu shall be closed, which also qualifies as 'update'.
  return !mmi || mmi->HasUserIO();
}

bool cCiMenu::Select(int Index)
{
  cMutexLock MutexLock(mutex);
  dbgprotocol("%d: ==> Select %d\n", mmi ? mmi->SessionId() : -1, Index);
  if (mmi && -1 <= Index && Index < numEntries)
     return mmi->SendMenuAnswer(Index + 1);
  return false;
}

bool cCiMenu::Cancel(void)
{
  return Select(-1);
}

bool cCiMenu::Abort(void)
{
  cMutexLock MutexLock(mutex);
  return mmi && mmi->SendCloseMMI();
}

// --- cCiEnquiry ------------------------------------------------------------

cCiEnquiry::cCiEnquiry(cCiMMI *MMI)
{
  mmi = MMI;
  text = NULL;
  blind = false;
  expectedLength = 0;
}

cCiEnquiry::~cCiEnquiry()
{
  cMutexLock MutexLock(mutex);
  if (mmi)
     mmi->Enquiry(true);
  free(text);
}

bool cCiEnquiry::Reply(const char *s)
{
  cMutexLock MutexLock(mutex);
  return mmi ? mmi->SendAnswer(s) : false;
}

bool cCiEnquiry::Cancel(void)
{
  return Reply(NULL);
}

bool cCiEnquiry::Abort(void)
{
  cMutexLock MutexLock(mutex);
  return mmi && mmi->SendCloseMMI();
}

// -- cCiHandler -------------------------------------------------------------

cCiHandler::cCiHandler(int Fd, int NumSlots)
{
  fd = Fd;
  numSlots = NumSlots;
  newCaSupport = false;
  hasUserIO = false;
  for (int i = 0; i < MAX_CI_SESSION; i++)
      sessions[i] = NULL;
  for (int i = 0; i < MAX_CI_SLOT; i++)
      moduleReady[i] = false;
  tpl = new cCiTransportLayer(Fd, numSlots);
  tc = NULL;
  source = transponder = 0;
}

cCiHandler::~cCiHandler()
{
  for (int i = 0; i < MAX_CI_SESSION; i++)
      delete sessions[i];
  delete tpl;
  close(fd);
}

cCiHandler *cCiHandler::CreateCiHandler(const char *FileName)
{
  int fd_ca = open(FileName, O_RDWR);
  if (fd_ca >= 0) {
     ca_caps_t Caps;
     if (ioctl(fd_ca, CA_GET_CAP, &Caps) == 0) {
        int NumSlots = Caps.slot_num;
        if (NumSlots > 0) {
           //XXX dsyslog("CAM: found %d CAM slots", NumSlots); // TODO let's do this only once we can be sure that there _really_ is a CAM adapter!
           if ((Caps.slot_type & CA_CI_LINK) != 0)
              return new cCiHandler(fd_ca, NumSlots);
           else
              isyslog("CAM doesn't support link layer interface");
           }
        else
           esyslog("ERROR: no CAM slots found");
        }
     else
        LOG_ERROR_STR(FileName);
     close(fd_ca);
     }
  return NULL;
}

uint32_t cCiHandler::ResourceIdToInt(const uint8_t *Data)
{
  return (ntohl(get_unaligned((uint32_t *)Data)));
}

bool cCiHandler::Send(uint8_t Tag, uint16_t SessionId, uint32_t ResourceId, int Status)
{
  uint8_t buffer[16];
  uint8_t *p = buffer;
  *p++ = Tag;
  *p++ = 0x00; // will contain length
  if (Status >= 0)
     *p++ = Status;
  if (ResourceId) {
     put_unaligned(htonl(ResourceId), (uint32_t *)p);
     p += 4;
     }
  put_unaligned(htons(SessionId), (uint16_t *)p);
  p += 2;
  buffer[1] = p - buffer - 2; // length
  return tc && tc->SendData(p - buffer, buffer) == OK;
}

cCiSession *cCiHandler::GetSessionBySessionId(uint16_t SessionId)
{
  for (int i = 0; i < MAX_CI_SESSION; i++) {
      if (sessions[i] && sessions[i]->SessionId() == SessionId)
         return sessions[i];
      }
  return NULL;
}

cCiSession *cCiHandler::GetSessionByResourceId(uint32_t ResourceId, int Slot)
{
  for (int i = 0; i < MAX_CI_SESSION; i++) {
      if (sessions[i] && sessions[i]->Tc()->Slot() == Slot && sessions[i]->ResourceId() == ResourceId)
         return sessions[i];
      }
  return NULL;
}

cCiSession *cCiHandler::CreateSession(uint32_t ResourceId)
{
  if (!GetSessionByResourceId(ResourceId, tc->Slot())) {
     for (int i = 0; i < MAX_CI_SESSION; i++) {
         if (!sessions[i]) {
            switch (ResourceId) {
              case RI_RESOURCE_MANAGER:           return sessions[i] = new cCiResourceManager(i + 1, tc);
              case RI_APPLICATION_INFORMATION:    return sessions[i] = new cCiApplicationInformation(i + 1, tc);
              case RI_CONDITIONAL_ACCESS_SUPPORT: newCaSupport = true;
                                                  return sessions[i] = new cCiConditionalAccessSupport(i + 1, tc);
              case RI_HOST_CONTROL:               break; //XXX
              case RI_DATE_TIME:                  return sessions[i] = new cCiDateTime(i + 1, tc);
              case RI_MMI:                        return sessions[i] = new cCiMMI(i + 1, tc);
              }
            }
         }
     }
  return NULL;
}

bool cCiHandler::OpenSession(int Length, const uint8_t *Data)
{
  if (Length == 6 && *(Data + 1) == 0x04) {
     uint32_t ResourceId = ResourceIdToInt(Data + 2);
     dbgprotocol("OpenSession %08X\n", ResourceId);
     switch (ResourceId) {
       case RI_RESOURCE_MANAGER:
       case RI_APPLICATION_INFORMATION:
       case RI_CONDITIONAL_ACCESS_SUPPORT:
       case RI_HOST_CONTROL:
       case RI_DATE_TIME:
       case RI_MMI:
            {
              cCiSession *Session = CreateSession(ResourceId);
              if (Session) {
                 Send(ST_OPEN_SESSION_RESPONSE, Session->SessionId(), Session->ResourceId(), SS_OK);
                 return true;
                 }
              esyslog("ERROR: can't create session for resource identifier: %08X", ResourceId);
            }
            break;
       default: esyslog("ERROR: unknown resource identifier: %08X", ResourceId);
       }
     }
  return false;
}

bool cCiHandler::CloseSession(uint16_t SessionId)
{
  dbgprotocol("CloseSession %d\n", SessionId);
  cCiSession *Session = GetSessionBySessionId(SessionId);
  if (Session && sessions[SessionId - 1] == Session) {
     delete Session;
     sessions[SessionId - 1] = NULL;
     Send(ST_CLOSE_SESSION_RESPONSE, SessionId, 0, SS_OK);
     return true;
     }
  else {
     esyslog("ERROR: unknown session id: %d", SessionId);
     Send(ST_CLOSE_SESSION_RESPONSE, SessionId, 0, SS_NOT_ALLOCATED);
     }
  return false;
}

int cCiHandler::CloseAllSessions(int Slot)
{
  int result = 0;
  for (int i = 0; i < MAX_CI_SESSION; i++) {
      if (sessions[i] && sessions[i]->Tc()->Slot() == Slot) {
         CloseSession(sessions[i]->SessionId());
         result++;
         }
      }
  return result;
}

int cCiHandler::NumCams(void)
{
  int result = 0;
  for (int i = 0; i < MAX_CI_SLOT; i++)
      if (moduleReady[i])
         result++;
  return result;
}

bool cCiHandler::Ready(void)
{
  cMutexLock MutexLock(&mutex);
  for (int Slot = 0; Slot < numSlots; Slot++) {
      if (moduleReady[Slot]) {
         cCiConditionalAccessSupport *cas = (cCiConditionalAccessSupport *)GetSessionByResourceId(RI_CONDITIONAL_ACCESS_SUPPORT, Slot);
         if (!cas || !*cas->GetCaSystemIds())
            return false;
         }
      }
  return true;
}

bool cCiHandler::Process(int Slot)
{
  bool result = true;
  cMutexLock MutexLock(&mutex);
  for (int slot = 0; slot < numSlots; slot++) {
      if (Slot < 0 || slot == Slot) {
         tc = tpl->Process(slot);
         if (tc) {
            int Length;
            const uint8_t *Data = tc->Data(Length);
            if (Data && Length > 1) {
               switch (*Data) {
                 case ST_SESSION_NUMBER:          if (Length > 4) {
                                                     uint16_t SessionId = ntohs(get_unaligned((uint16_t *)&Data[2]));
                                                     cCiSession *Session = GetSessionBySessionId(SessionId);
                                                     if (Session)
                                                        Session->Process(Length - 4, Data + 4);
                                                     else
                                                        esyslog("ERROR: unknown session id: %d", SessionId);
                                                     }
                                                  break;
                 case ST_OPEN_SESSION_REQUEST:    OpenSession(Length, Data);
                                                  break;
                 case ST_CLOSE_SESSION_REQUEST:   if (Length == 4)
                                                     CloseSession(ntohs(get_unaligned((uint16_t *)&Data[2])));
                                                  break;
                 case ST_CREATE_SESSION_RESPONSE: //XXX fall through to default
                 case ST_CLOSE_SESSION_RESPONSE:  //XXX fall through to default
                 default: esyslog("ERROR: unknown session tag: %02X", *Data);
                 }
               }
            }
         else if (CloseAllSessions(slot)) {
            tpl->ResetSlot(slot);
            result = false;
            }
         else if (tpl->ModuleReady(slot)) {
            dbgprotocol("Module ready in slot %d\n", slot);
            moduleReady[slot] = true;
            tpl->NewConnection(slot);
            }
         }
      }
  SendCaPmt();
  bool UserIO = false;
  for (int i = 0; i < MAX_CI_SESSION; i++) {
      if (sessions[i] && sessions[i]->Process())
         UserIO |= sessions[i]->HasUserIO();
      }
  hasUserIO = UserIO;
  return result;
}

void cCiHandler::SendCaPmt(void)
{
  cMutexLock MutexLock(&mutex);
  if (newCaSupport) {
     newCaSupport = false;
     for (int Slot = 0; Slot < numSlots; Slot++) {
         cCiConditionalAccessSupport *cas = (cCiConditionalAccessSupport *)GetSessionByResourceId(RI_CONDITIONAL_ACCESS_SUPPORT, Slot);
         if (cas) {
            // build the list of CA_PMT data:
            cList<cCiCaPmt> CaPmtList;
            for (cCiCaProgramData *p = caProgramList.First(); p; p = caProgramList.Next(p)) {
                bool Active = false;
                cCiCaPmt *CaPmt = new cCiCaPmt(CPCI_OK_DESCRAMBLING, source, transponder, p->programNumber, GetCaSystemIds(Slot));
                if (CaPmt->Valid()) {
                   for (cCiCaPidData *q = p->pidList.First(); q; q = p->pidList.Next(q)) {
                       if (q->active) {
                          CaPmt->AddPid(q->pid, q->streamType);
                          Active = true;
                          }
                       }
                   }
                if (Active)
                   CaPmtList.Add(CaPmt);
                else
                   delete CaPmt;
                }
            // send the CA_PMT data:
            uint8_t ListManagement = CaPmtList.Count() > 1 ? CPLM_FIRST : CPLM_ONLY;
            for (cCiCaPmt *CaPmt = CaPmtList.First(); CaPmt; CaPmt = CaPmtList.Next(CaPmt)) {
                CaPmt->SetListManagement(ListManagement);
                if (!cas->SendPMT(CaPmt))
                   newCaSupport = true;
                ListManagement = CaPmt->Next() && CaPmt->Next()->Next() ? CPLM_MORE : CPLM_LAST;
                }
            }
         }
     }
}

bool cCiHandler::EnterMenu(int Slot)
{
  cMutexLock MutexLock(&mutex);
  cCiApplicationInformation *api = (cCiApplicationInformation *)GetSessionByResourceId(RI_APPLICATION_INFORMATION, Slot);
  return api ? api->EnterMenu() : false;
}

cCiMenu *cCiHandler::GetMenu(void)
{
  cMutexLock MutexLock(&mutex);
  for (int Slot = 0; Slot < numSlots; Slot++) {
      cCiMMI *mmi = (cCiMMI *)GetSessionByResourceId(RI_MMI, Slot);
      if (mmi) {
         cCiMenu *Menu = mmi->Menu();
         if (Menu)
            Menu->mutex = &mutex;
         return Menu;
         }
      }
  return NULL;
}

cCiEnquiry *cCiHandler::GetEnquiry(void)
{
  cMutexLock MutexLock(&mutex);
  for (int Slot = 0; Slot < numSlots; Slot++) {
      cCiMMI *mmi = (cCiMMI *)GetSessionByResourceId(RI_MMI, Slot);
      if (mmi) {
         cCiEnquiry *Enquiry = mmi->Enquiry();
         if (Enquiry)
            Enquiry->mutex = &mutex;
         return Enquiry;
         }
      }
  return NULL;
}

const char *cCiHandler::GetCamName(int Slot)
{
  cMutexLock MutexLock(&mutex);
  cCiApplicationInformation *ai = (cCiApplicationInformation *)GetSessionByResourceId(RI_APPLICATION_INFORMATION, Slot);
  return ai ? ai->GetMenuString() : NULL;
}

const unsigned short *cCiHandler::GetCaSystemIds(int Slot)
{
  cMutexLock MutexLock(&mutex);
  cCiConditionalAccessSupport *cas = (cCiConditionalAccessSupport *)GetSessionByResourceId(RI_CONDITIONAL_ACCESS_SUPPORT, Slot);
  return cas ? cas->GetCaSystemIds() : NULL;
}

bool cCiHandler::ProvidesCa(const unsigned short *CaSystemIds)
{
  cMutexLock MutexLock(&mutex);
  for (int Slot = 0; Slot < numSlots; Slot++) {
      cCiConditionalAccessSupport *cas = (cCiConditionalAccessSupport *)GetSessionByResourceId(RI_CONDITIONAL_ACCESS_SUPPORT, Slot);
      if (cas) {
         for (const unsigned short *ids = cas->GetCaSystemIds(); ids && *ids; ids++) {
             for (const unsigned short *id = CaSystemIds; *id; id++) {
                 if (*id == *ids)
                    return true;
                 }
             }
         }
      }
  return false;
}

void cCiHandler::SetSource(int Source, int Transponder)
{
  cMutexLock MutexLock(&mutex);
  if (source != Source || transponder != Transponder) {
     //XXX if there are active entries, send an empty CA_PMT
     caProgramList.Clear();
     }
  source = Source;
  transponder = Transponder;
}

void cCiHandler::AddPid(int ProgramNumber, int Pid, int StreamType)
{
  cMutexLock MutexLock(&mutex);
  cCiCaProgramData *ProgramData = NULL;
  for (cCiCaProgramData *p = caProgramList.First(); p; p = caProgramList.Next(p)) {
      if (p->programNumber == ProgramNumber) {
         ProgramData = p;
         for (cCiCaPidData *q = p->pidList.First(); q; q = p->pidList.Next(q)) {
             if (q->pid == Pid)
                return;
             }
         }
      }
  if (!ProgramData)
     caProgramList.Add(ProgramData = new cCiCaProgramData(ProgramNumber));
  ProgramData->pidList.Add(new cCiCaPidData(Pid, StreamType));
}

void cCiHandler::SetPid(int Pid, bool Active)
{
  cMutexLock MutexLock(&mutex);
  for (cCiCaProgramData *p = caProgramList.First(); p; p = caProgramList.Next(p)) {
      for (cCiCaPidData *q = p->pidList.First(); q; q = p->pidList.Next(q)) {
          if (q->pid == Pid) {
             q->active = Active;
             return;
             }
         }
      }
}

bool cCiHandler::CanDecrypt(int ProgramNumber)
{
  cMutexLock MutexLock(&mutex);
  for (int Slot = 0; Slot < numSlots; Slot++) {
      cCiConditionalAccessSupport *cas = (cCiConditionalAccessSupport *)GetSessionByResourceId(RI_CONDITIONAL_ACCESS_SUPPORT, Slot);
      if (cas) {
         for (cCiCaProgramData *p = caProgramList.First(); p; p = caProgramList.Next(p)) {
             if (p->programNumber == ProgramNumber) {
                cCiCaPmt CaPmt(CPCI_QUERY, source, transponder, p->programNumber, GetCaSystemIds(Slot));//XXX???
                if (CaPmt.Valid()) {
                   for (cCiCaPidData *q = p->pidList.First(); q; q = p->pidList.Next(q)) {
//XXX                       if (q->active)
                          CaPmt.AddPid(q->pid, q->streamType);
                       }
                   }
                if (!cas->SendPMT(&CaPmt))
                   return false;//XXX
                //XXX
                time_t timeout = time(NULL) + 3;//XXX
                while (time(NULL) <= timeout) {
                      Process(Slot);
                      cas = (cCiConditionalAccessSupport *)GetSessionByResourceId(RI_CONDITIONAL_ACCESS_SUPPORT, Slot);
                      if (!cas)
                         return false;//XXX
                      if (cas->ReceivedReply(true))
                         return true;
                      //XXX remember if a slot doesn't receive a reply
                      }
                break;
                }
             }
         }
      }
  return false;
}

void cCiHandler::StartDecrypting(void)
{
  cMutexLock MutexLock(&mutex);
  newCaSupport = true;
  SendCaPmt();
}

bool cCiHandler::Reset(int Slot)
{
  cMutexLock MutexLock(&mutex);
  CloseAllSessions(Slot);
  return tpl->ResetSlot(Slot, true);
}

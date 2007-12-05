/*
 * stack.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE



#include <malloc.h>
#include <string.h>
#include <sys/socket.h>

#include <vdr/remote.h>
#include <vdr/tools.h>

#include "global.h"
#include "stack.h"



class cCtrlHighestThier
: public cCtrlThier {

public:

  cCtrlHighestThier();
  ~cCtrlHighestThier();

// cCtrlThier
  virtual int dataFromLowerThier(const uchar*, int size);

  int read(uchar* pData, int maxSize);

private:
  uchar* _buffer;
  int    _size;
};



cCtrlHighestThier::cCtrlHighestThier()
: _buffer (NULL),
  _size   (0)
{
}



cCtrlHighestThier::~cCtrlHighestThier() {
  free(_buffer);
}



int cCtrlHighestThier::dataFromLowerThier(const uchar* pData, int size) {

conLOG_STACK("cCtrlHighestThier::dataFromLowerThier: receiving data (%d)", size);

  int oldSize = _size;

  _buffer = (uchar*)realloc((void*)_buffer, sizeof(uchar) * (oldSize + size));

conLOG_STACK("cCtrlHighestThier::dataFromLowerThier: buffer allocated (0x%p)", _buffer);

  if (! _buffer) {
    conESYSLOG("couldn't allocate memory for receiving buffer");
    _size = 0;
    return -1;
  }

conLOG_STACK("cCtrlHighestThier::dataFromLowerThier: before memcpy");

  memcpy(_buffer + oldSize, pData, size);
  _size = oldSize + size;

conLOG_STACK("cCtrlHighestThier::dataFromLowerThier: data received (%d)", size);

  return size;
}



int cCtrlHighestThier::read(uchar* pData, int maxSize) {

  if (! pData)
    return -1;

  int retSize = min(maxSize, _size);

conLOG_STACK("cCtrlHighestThier::read: returning data (%d, %d -> %d)", maxSize, _size, retSize);

  memcpy(pData, _buffer, retSize);

conLOG_STACK("cCtrlHighestThier::read: after memcpy");

  _size -= retSize;
  memmove(_buffer, _buffer + retSize, _size);

conLOG_STACK("cCtrlHighestThier::read: data returned");

  return retSize;
}





cCtrlStack::cCtrlStack()
: _top    (NULL),
  _base   (NULL),
  _helper (NULL)
{
  _helper = new cCtrlHighestThier();
}



cCtrlStack::~cCtrlStack() {
  clear();
  delete _helper;
}



void cCtrlStack::push(cCtrlThier* newThier) {

  if (! newThier)
    return;

  if (_top) {
    newThier->setLowerThier(_top);
    _top->setHigherThier(newThier);
  } else {
    _base = newThier;
  }

  newThier->setHigherThier(_helper);
  _top = newThier;
}



int cCtrlStack::start() {

  cCtrlThier* pThier = _base;
  while (pThier) {

    pThier->start();
    pThier = pThier->higherThier();
  }

  return 0;
}



void cCtrlStack::stop() {

  cCtrlThier* pThier = _top;
  while (pThier) {

    pThier->stop();
    pThier = pThier->lowerThier();
  }
}



void cCtrlStack::clear() {

conLOG_STACK("cCtrlStack::clear: clearing stack");

  cCtrlThier* pThier = _top;
  while (pThier) {

    cCtrlThier* p = pThier;
    pThier = p->lowerThier();

    delete p;
  }

conLOG_STACK("cCtrlStack::clear: stack cleared");
}



int cCtrlStack::sendData(const uchar* pData, int size) {
  return _top ? _top->dataFromHigherThier(pData, size) : -1;
}



int cCtrlStack::receiveData(uchar* pData, int maxSize) {

  if (! _base || ! _helper)
    return -1;

conLOG_STACK("cCtrlStack::receiveData: try to read incoming data from buffer");

  // Let's look if there is something in the buffer.
  int r = _helper->read(pData, maxSize);
  if (r != 0)
    return r;

conLOG_STACK("cCtrlStack::receiveData: no data in buffer - try to read from lowest thier");

  // Make that the lowest thier reads its data (and pushes it into our helper).
  r = ((cCtrlLowestThier*)_base)->read(maxSize);
  if (r < 0)
    return r;

conLOG_STACK("cCtrlStack::receiveData: retry to read from buffer");

  // Now - there should be something in the buffer.
  return _helper->read(pData, maxSize);
}



bool cCtrlStack::poll(int timems) {
  return _base ? ((cCtrlLowestThier*)_base)->poll(timems) : false;
}





cCtrlSocket::cCtrlSocket()
: _socket (0)
{
}



cCtrlSocket::cCtrlSocket(int Socket)
: _socket (Socket)
{
}



cCtrlSocket::~cCtrlSocket() {
  close();
}



int cCtrlSocket::open(int socket) {

  close();
  _socket = socket;

  return 0;
}



void cCtrlSocket::close() {

  if (_socket)
    ::close(_socket);

  _socket = 0;
}



int cCtrlSocket::dataFromHigherThier(const uchar* pData, int size) {

  if (_socket)
    return write(_socket, pData, size);

  return -1;
}



int cCtrlSocket::dataFromLowerThier(const uchar* pData, int size) {
  return sendDataUp(pData, size);
}



bool cCtrlSocket::poll(int timems) {
  cPoller poller(_socket);
  return poller.Poll(timems);
}



int cCtrlSocket::read(int maxSize) {

#define STACK_BUFFER_SIZE 255

  uchar stack_buffer[STACK_BUFFER_SIZE];

conLOG_STACK("cCtrlSocket::read: allocating memory (%d)", maxSize);

  uchar* pBuf = stack_buffer;
  if (maxSize > STACK_BUFFER_SIZE)
    pBuf = (uchar*)malloc(sizeof(uchar) * maxSize);

conLOG_STACK("cCtrlSocket::read: memory (0x%p)", pBuf);

  if (! pBuf) {
    conESYSLOG("couldn't allocate memory for receiving buffer");
    return -1;
  }

conLOG_STACK("cCtrlSocket::read: reading from socket (%d)", maxSize);

  int r = ::read(_socket, pBuf, maxSize);
  if (r > 0) {

conLOG_STACK("cCtrlSocket::read: sending data (%d)", r);
    sendDataUp(pBuf, r);
  }

  if (maxSize > STACK_BUFFER_SIZE)
    free(pBuf);

conLOG_STACK("cCtrlSocket::read: freeing memory");

  return r;
}





cCtrlTty::cCtrlTty()
: _prepared (false)
{
}



cCtrlTty::cCtrlTty(int Socket)
: cCtrlSocket(Socket),
  _prepared (false)
{
}



cCtrlTty::~cCtrlTty() {
  // If prepared then unprepare now
  UnprepareSocket();
}



void cCtrlTty::PrepareSocket() {

  if (_prepared)
    return;

  // prepare STDIN for Console
  termios t;
  int ret = tcgetattr(_socket, &t);
  if (ret < 0) {
    conESYSLOG("couldn't get terminal mode");
    return;
  }

  // save settings to reset on exit
  _oldTerminalSettings = t;

  // change the settings for virtual console mode
  t.c_lflag &= ~(ICANON | ISIG | ECHO | ECHOCTL | ECHOE | ECHOK | ECHOKE | ECHONL | ECHOPRT);
  t.c_iflag |= IGNBRK;
  t.c_iflag &= ~(ICRNL | INLCR | ISTRIP | BRKINT);

  t.c_cflag &= ~(CSIZE);
  t.c_cflag |= (CS8 );

  t.c_cc[VMIN] = 0;
  t.c_cc[VTIME] = 0;

  ret = tcsetattr(_socket, TCSANOW, &t);
  if (ret < 0) {
    conESYSLOG("couldn't set terminal mode");
    return;
  }

  _prepared = true;
}



void cCtrlTty::UnprepareSocket() {

  if (! _prepared)
    return;

  tcsetattr(_socket, TCSANOW, &_oldTerminalSettings);

  _prepared = false;
}



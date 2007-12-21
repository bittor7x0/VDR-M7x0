/*
 * stack.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */



#ifndef _CTRL_STACK_
#define _CTRL_STACK_



//#include <sys/socket.h>





class cCtrlThier
{
public:

  cCtrlThier()
  : _higherThier (NULL),
    _lowerThier  (NULL)
  {}

  virtual ~cCtrlThier() {}


  // These are to be implemented from derived classes
  // ------------------------------------------------
  // Send data up to higher thier
  virtual int dataFromLowerThier(const uchar*, int count) {return 0;}
  // Send data down to lower thier
  virtual int dataFromHigherThier(const uchar*, int count) {return 0;}
  virtual int start() {return 0;}
  virtual void stop() {}


  void setHigherThier(cCtrlThier* higherThier) {_higherThier = higherThier;}
  cCtrlThier* higherThier() { return _higherThier; }

  void setLowerThier(cCtrlThier* lowerThier) {_lowerThier = lowerThier;}
  cCtrlThier* lowerThier() { return _lowerThier; }


protected:

  int sendDataUp(const uchar* pData, int size) {
    return _higherThier ? _higherThier->dataFromLowerThier(pData, size) : -1;
  }

  int sendDataDown(const uchar* pData, int size) {
    return _lowerThier ? _lowerThier->dataFromHigherThier(pData, size) : -1;
  }


protected:
  cCtrlThier* _higherThier;
  cCtrlThier* _lowerThier;

};





class cCtrlLowestThier
: public cCtrlThier {

public:

  // Tests if there are data to receive
  virtual bool poll(int timems) = 0;
  // Reads count bytes and forwards them through the protocol stack.
  virtual int read(int count) = 0;

};





class cCtrlHighestThier;



class cCtrlStack
{
public:

  cCtrlStack();
  virtual ~cCtrlStack();

  // Puts a new thier in top of the stack
  void push(cCtrlThier* newThier);

  int start();
  void stop();
  void clear();

  int sendData(const uchar* pData, int size);
  int receiveData(uchar* pData, int maxSize);

  bool poll(int timems = 10);


protected:
  cCtrlThier*        _top;
  cCtrlThier*        _base;
  cCtrlHighestThier* _helper;
};





class cCtrlSocket
: public cCtrlLowestThier
{
public:

  cCtrlSocket();
  cCtrlSocket(int Socket);
  virtual ~cCtrlSocket();

  int open(int socket);
  void close();


// cCtrlThier
  virtual int dataFromHigherThier(const uchar* pData, int size);
  virtual int dataFromLowerThier(const uchar* pData, int size);
  virtual void stop() {close();}


// cCtrlLowestThier
  virtual bool poll(int timems);
  virtual int read(int count);

protected:
  int _socket;
};





class cCtrlTty
: public cCtrlSocket
{
public:

  cCtrlTty();
  cCtrlTty(int Socket);
  ~cCtrlTty();

  void PrepareSocket();
  void UnprepareSocket();


protected:

  termios _oldTerminalSettings;
  bool    _prepared;
};



#endif



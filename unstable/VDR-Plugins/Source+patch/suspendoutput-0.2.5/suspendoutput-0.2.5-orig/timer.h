/*
 * timer.h: Threaded timer class
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __XINELIBOUTPUT_TIMER_H
#define __XINELIBOUTPUT_TIMER_H

//
// cTimerCallback : timer callback handler interface
//
class cTimerCallback {
  protected:
    virtual bool TimerEvent() = 0; // return false to cancel timer

    virtual void *TargetId() { return (void*)this; }
    virtual int   size()     { return sizeof(*this); }
    virtual bool  is(void *data, int len) 
    { 
      return len==sizeof(*this) && TargetId()==data; 
    }

    friend class cTimerThread;

  public:
    static void Set(cTimerCallback *, unsigned int TimeoutMs);
    static void Cancel(cTimerCallback *);

    virtual ~cTimerCallback();
};

//
// cTimerEvent : base class for timer events
//
class cTimerEvent : protected cTimerCallback {
  private:
    cTimerEvent(cTimerEvent&);

  protected:
    cTimerEvent() {};

    virtual void AddEvent(unsigned int TimeoutMs);

    static void CancelAll(void *Target);

    template<class TCLASS> friend void CancelTimerEvents(TCLASS*);
    friend class cTimerThread;

  public:
    static void Cancel(cTimerEvent *&);
};

//
// make gcc 3.4.5 happy
//
template<class TCLASS, class TRESULT>
cTimerEvent *CreateTimerEvent(TCLASS *c, TRESULT (TCLASS::*fp)(void), 
			      unsigned int TimeoutMs);
template<class TCLASS, class TRESULT, class TARG1>
cTimerEvent *CreateTimerEvent(TCLASS *c, TRESULT (TCLASS::*fp)(TARG1), 
			      TARG1 arg1, 
			      unsigned int TimeoutMs);
template<class TCLASS>
cTimerEvent *CreateTimerEvent(TCLASS *c, void (TCLASS::*fp)(void), 
			      unsigned int TimeoutMs, bool runOnce = true);
template<class TCLASS, class TARG1>
cTimerEvent *CreateTimerEvent(TCLASS *c, void (TCLASS::*fp)(TARG1), 
			      TARG1 arg1, 
			      unsigned int TimeoutMs, bool runOnce = true);

//
// Timer event templates
//

template <class TCLASS, class TRESULT>
class cTimerFunctorR0 : public cTimerEvent {

  public:

  protected:
    typedef TRESULT (TCLASS::*TFUNC)(void);

    cTimerFunctorR0(TCLASS *obj, TFUNC f, unsigned int TimeoutMs) :
      m_obj(obj), m_f(f) 
    { 
      AddEvent(TimeoutMs); 
    }

    virtual ~cTimerFunctorR0() {};

    virtual bool TimerEvent(void)
    { 
      return (*m_obj.*m_f)(); 
    }

    virtual void *TargetId() { return (void*)m_obj; }
    virtual int   size()     { return sizeof(*this); }
    virtual bool  is(void *data, int len) 
    { 
      return sizeof(*this)==len && !memcmp(this,data,len); 
    }

  private:
    TCLASS *m_obj;
    TFUNC   m_f;

    friend cTimerEvent *CreateTimerEvent<TCLASS,TRESULT>(TCLASS*,TFUNC,unsigned int);
};

template <class TCLASS, class TRESULT, class TARG1>
class cTimerFunctorR1 : public cTimerEvent {

  public:

  protected:
    typedef TRESULT (TCLASS::*TFUNC)(TARG1);

    cTimerFunctorR1(TCLASS *obj, TFUNC f, TARG1 arg1, unsigned int TimeoutMs) :
      m_obj(obj), m_f(f), m_arg1(arg1) 
    { 
      AddEvent(TimeoutMs); 
    }

    virtual ~cTimerFunctorR1() {};

    virtual bool TimerEvent(void)
    {
      return (*m_obj.*m_f)(m_arg1); 
    }

    virtual void *TargetId() { return (void*)m_obj; }
    virtual int   size()     { return sizeof(*this); }
    virtual bool  is(void *data, int len) 
    { 
      return sizeof(*this)==len && !memcmp(this,data,len); 
    }

  private:
    TCLASS *m_obj;
    TFUNC   m_f;
    TARG1   m_arg1;

    friend cTimerEvent *CreateTimerEvent<TCLASS,TRESULT,TARG1>(TCLASS*,TFUNC,TARG1,unsigned int);
};

template <class TCLASS>
class cTimerFunctor0 : public cTimerEvent {

  public:

  protected:
    typedef void (TCLASS::*TFUNC)(void);

    cTimerFunctor0(TCLASS *obj, TFUNC f, 
		   unsigned int TimeoutMs, bool runOnce) :
      m_obj(obj), m_f(f), m_runAgain(!runOnce) 
    { 
      AddEvent(TimeoutMs); 
    }

    virtual ~cTimerFunctor0() {};

    virtual bool TimerEvent(void)
    { 
      (*m_obj.*m_f)(); 
      return m_runAgain; 
    }

    virtual void *TargetId() { return (void*)m_obj; }
    virtual int   size()     { return sizeof(*this); }
    virtual bool  is(void *data, int len) 
    { 
      return sizeof(*this)==len && !memcmp(this,data,len); 
    }

  private:
    TCLASS *m_obj;
    TFUNC   m_f;
    bool    m_runAgain;

    friend cTimerEvent *CreateTimerEvent<TCLASS>(TCLASS*,TFUNC,unsigned int,bool);
};

template <class TCLASS, class TARG1>
class cTimerFunctor1 : public cTimerEvent {

  public:

  protected:
    typedef void (TCLASS::*TFUNC)(TARG1);

    cTimerFunctor1(TCLASS *obj, TFUNC f, TARG1 arg1, 
		   unsigned int TimeoutMs, bool runOnce) :
      m_obj(obj), m_f(f), m_arg1(arg1), m_runAgain(!runOnce) 
    { 
      AddEvent(TimeoutMs); 
    }

    virtual ~cTimerFunctor1() {};

    virtual bool TimerEvent(void)
    { 
      (*m_obj.*m_f)(m_arg1); 
      return m_runAgain; 
    }

    virtual void *TargetId() { return (void*)m_obj; }
    virtual int   size()     { return sizeof(*this); }
    virtual bool  is(void *data, int len) 
    { 
      return sizeof(*this)==len && !memcmp(this,data,len); 
    }

  private:
    TCLASS *m_obj;
    TFUNC   m_f;
    TARG1   m_arg1;
    bool    m_runAgain;

    friend cTimerEvent *CreateTimerEvent<TCLASS,TARG1>(TCLASS*,TFUNC,TARG1,unsigned int,bool);
};

//
// Function templates for timer event creation and cancellation
//

template<class TCLASS, class TRESULT>
cTimerEvent *CreateTimerEvent(TCLASS *c, TRESULT (TCLASS::*fp)(void), 
			      unsigned int TimeoutMs)
{ 
  return new cTimerFunctorR0<TCLASS,TRESULT>(c,fp,TimeoutMs); 
}

template<class TCLASS, class TRESULT, class TARG1>
cTimerEvent *CreateTimerEvent(TCLASS *c, TRESULT (TCLASS::*fp)(TARG1), 
			      TARG1 arg1, 
			      unsigned int TimeoutMs)
{ 
  return new cTimerFunctorR1<TCLASS,TRESULT,TARG1>(c,fp,arg1,TimeoutMs); 
}

template<class TCLASS>
cTimerEvent *CreateTimerEvent(TCLASS *c, void (TCLASS::*fp)(void), 
			      unsigned int TimeoutMs, bool runOnce = true)
{ 
  return new cTimerFunctor0<TCLASS>(c,fp,TimeoutMs,runOnce); 
}

template<class TCLASS, class TARG1>
cTimerEvent *CreateTimerEvent(TCLASS *c, void (TCLASS::*fp)(TARG1), 
			      TARG1 arg1, 
			      unsigned int TimeoutMs, bool runOnce = true)
{ 
  return new cTimerFunctor1<TCLASS,TARG1>(c,fp,arg1,TimeoutMs,runOnce); 
}

template<class TCLASS>
void CancelTimerEvents(TCLASS *c)
{ 
  cTimerEvent::CancelAll((void*)c); 
}


// usage:
//
// 'this' derived from cTimerHandler:
//   Set timer:
//     cTimerCallback::Set(this, TimeoutMs);
//   Cancel timer:
//     - return false from handler or
//     - call cTimerCallback::Cancel(this); or
//     - delete 'this' object
//
// any function of any class:
//   Set timer:
//     - cTimerEvent *event = CreateTimerEvent(...);
//       example:
//       CreateTimerEvent(this, &cXinelibDevice::TimerEvent, 1, 1000);
//       -> calls this->cXinelibDevice::TimerEvent(1) every second until stopped.
//   Cancel timer:
//     - if handler returns bool: return false from handler
//     - handler is type of void: timer runs only once
//     - call cTimerEvent::Cancel(event)
//   Cancel all timers for object:
//     - Call CancelTimerEvents(object)
//     - Call CancelTimerEvents(this)


#endif // __XINELIBOUTPUT_TIMER_H



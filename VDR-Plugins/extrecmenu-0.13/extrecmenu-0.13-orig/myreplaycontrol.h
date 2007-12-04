class myReplayControl:public cReplayControl
{
 public:
  static char *Cut;
  static char *Cutted;
  eOSState ProcessKey(eKeys Key);
  ~myReplayControl();
};

class myReplayControl:public cReplayControl
{
 private:
   bool timesearchactive;
 public:
   myReplayControl();
   ~myReplayControl();
   eOSState ProcessKey(eKeys Key);
};

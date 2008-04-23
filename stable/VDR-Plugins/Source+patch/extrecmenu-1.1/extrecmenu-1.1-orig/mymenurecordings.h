// --- myMenuRecordingsItem ---------------------------------------------------
class myMenuRecordingsItem:public cOsdItem
{
  private:
    bool dirismoving;
    bool isdvd;
    int level,isdirectory;
    int totalentries,newentries;
    char *title;
    char *name;
    const char *filename;
    std::string uniqid; // this is the unique name that identifies a recording
  public:
    myMenuRecordingsItem(cRecording *Recording,int Level);
    ~myMenuRecordingsItem();
    const char *FileName(){return filename;}
    const char *Name(){return name;}
    bool IsDirectory(){return name!=NULL;}
    void IncrementCounter(bool IsNew);
    bool IsDVD(){return isdvd;}
    void SetDirIsMoving(bool moving){dirismoving=moving;}
    bool GetDirIsMoving(){return dirismoving;}
    const char *UniqID(){return uniqid.length()?uniqid.c_str():"";}
};

// --- myMenuRecordings -------------------------------------------------------
class myMenuRecordings:public cOsdMenu
{
 private:
  bool edit;
  static bool wasdvd;
  static bool golastreplayed;
  static dev_t fsid;
  static int freediskspace;
  int level,helpkeys;
  int recordingsstate;
  char *base;
  bool Open();
  void SetHelpKeys();
  void Title();
  cRecording *GetRecording(myMenuRecordingsItem *Item);
  eOSState Play();
  eOSState Rewind();
  eOSState Delete();
  eOSState Rename();
  eOSState MoveRec();
  eOSState Info();
  eOSState Details();
  eOSState Commands(eKeys Key=kNone);
  eOSState ChangeSorting();
  int FreeMB();
 public:
  myMenuRecordings(const char *Base=NULL,int Level=0);
  ~myMenuRecordings();
  void Set(bool Refresh=false,char *current=NULL);
  virtual eOSState ProcessKey(eKeys Key);
};

// --- myMenuRenameRecording --------------------------------------------------
class myMenuRenameRecording:public cOsdMenu
{
 private:
  bool isdir;
  char *dirbase,*dirname;
  char name[MaxFileName];
  char path[MaxFileName];
  cRecording *recording;
  myMenuRecordings *menurecordings;
 public:
  myMenuRenameRecording(cRecording *Recording,const char *DirBase,const char *DirName);
  ~myMenuRenameRecording();
  virtual eOSState ProcessKey(eKeys Key);
};

// --- myMenuMoveRecording ----------------------------------------------------
class myMenuMoveRecording:public cOsdMenu
{
 private:
  int level;
  char *base;
  char *dirbase,*dirname;
  cRecording *recording;
  myMenuRecordings *menurecordings;
  void Set();
  eOSState Open();
  eOSState MoveRec();
  eOSState Create();
 public:
  myMenuMoveRecording(cRecording *Recording,const char *DirBase,const char *DirName,const char *Base=NULL,int Level=0);
  ~myMenuMoveRecording();
  virtual eOSState ProcessKey(eKeys Key);
  static bool clearall;
};

// --- myMenuRecordingDetails -------------------------------------------------
class myMenuRecordingDetails:public cOsdMenu
{
 private:
  int priority,lifetime;
  cRecording *recording;
  myMenuRecordings *menurecordings;
 public:
  myMenuRecordingDetails(cRecording *Recording);
  virtual eOSState ProcessKey(eKeys Key);
};

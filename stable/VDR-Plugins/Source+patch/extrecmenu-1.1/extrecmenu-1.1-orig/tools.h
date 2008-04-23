std::string myStrReplace(std::string S,char C1,char C2);

class SortListItem:public cListObject
{
  private:
    std::string path;
  public:
    SortListItem(std::string Path){path=Path;};
    std::string Path(){return path;}
};

class SortList:public cList<SortListItem>
{
  public:
    void ReadConfigFile();
    void WriteConfigFile();
    bool Find(std::string Path);
};

extern SortList *mySortList;

bool MoveRename(const char *OldName,const char *NewName,cRecording *Recording,bool Move);

class myRecListItem:public cListObject
{
  friend class myRecList;
  private:
    static bool SortByName;
    char *filename;
    static char *StripEpisodeName(char *s);
  public:
    myRecListItem(cRecording *Recording);
    ~myRecListItem();
    virtual int Compare(const cListObject &ListObject)const;
    cRecording *recording;
};

class myRecList:public cList<myRecListItem>
{
  public:
    void Sort(bool SortByName);
};

// --- MoveListItem -----------------------------------------------------------
class MoveListItem:public cListObject
{
  private:
    bool moveinprogress;
    bool movecanceled;
    std::string from;
    std::string to;
  public:
    MoveListItem(std::string From,std::string To){from=From;to=To;moveinprogress=false;movecanceled=false;}
    std::string From(){return from;}
    std::string To(){return to;}
    void SetMoveInProgress(){moveinprogress=true;}
    bool GetMoveInProgress(){return moveinprogress;}
    void SetMoveCanceled(){movecanceled=true;}
    bool GetMoveCanceled(){return movecanceled;}
};

// --- MoveList ---------------------------------------------------------------
class MoveList:public cList<MoveListItem>
{
};

// --- CutterListItem ---------------------------------------------------------
class CutterListItem:public cListObject
{
  private:
    bool cutinprogress;
    std::string filename;
    std::string newfilename;
  public:
    CutterListItem(std::string FileName){filename=FileName;cutinprogress=false;};
    void SetNewFileName(std::string NewFileName){newfilename=NewFileName;}
    std::string FileName(){return filename;}
    std::string NewFileName(){return newfilename;}
    void SetCutInProgress(){cutinprogress=true;}
    bool GetCutInProgress(){return cutinprogress;}
};

// --- CutterList -------------------------------------------------------------
class CutterList:public cList<CutterListItem>
{
};

// --- WorkerThread -----------------------------------------------------------
class WorkerThread:public cThread
{
  private:
    bool cancelmove,cancelcut;
    MoveList *MoveBetweenFileSystemsList;
    CutterList *CutterQueue;
    void Cut(std::string From,std::string To);
    bool Move(std::string From,std::string To);
  protected:
    virtual void Action();
  public:
    WorkerThread();
    ~WorkerThread();
    const char *Working();
    bool IsCutting(std::string Path);
    bool IsMoving(std::string Path);
    void CancelCut(std::string Path);
    void CancelMove(std::string Path);
    void AddToCutterQueue(std::string Path);
    void AddToMoveList(std::string From,std::string To);
    bool IsCutterQueueEmpty(){return CutterQueue->First();}
    bool IsMoveListEmpty(){return MoveBetweenFileSystemsList->First();}
};

extern WorkerThread *MoveCutterThread;


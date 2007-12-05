std::string myStrReplace(std::string S,char C1,char C2);

class SortListItem:public cListObject
{
  private:
    std::string path;
  public:
    SortListItem(const char *Path){path=Path;};
    const char *Path(){return path.c_str();}
};

class SortList:public cList<SortListItem>
{
  public:
    void ReadConfigFile();
    void WriteConfigFile();
    bool Find(char *Path);
};

class MoveBetweenFileSystems:public cThread
{
  private:
    std::string oldname;
    std::string newname;
    cRecording *recording;
    bool Move(std::string From,std::string To,cRecording *Recording);
  protected:
    virtual void Action();
  public:
    MoveBetweenFileSystems();
    bool Start(std::string OldName,std::string NewName,cRecording *Recording);
    bool IsMoving(std::string Path);
};

extern MoveBetweenFileSystems MoveThread;

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

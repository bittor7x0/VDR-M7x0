#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <fstream>

using namespace std;

bool CreateDirectory(string dir) {
    mkdir(dir.c_str(), 0775);
    //check if successfull
    DIR *pDir;
    bool exists = false;
    pDir = opendir(dir.c_str());
    if (pDir != NULL) {
        exists = true;    
        closedir(pDir);
    }
    return exists;
}

bool FileExists(string filename) {
    ifstream ifile(filename.c_str());
    if (ifile) {
        //a valid image should be larger then 500 bytes
        ifile.seekg (0, ifile.end);
        int length = ifile.tellg();
        if (length > 500)
            return true;
    }
    return false;
}

bool CheckDirExists(const char* dirName) {
    struct statfs statfsbuf;
    if (statfs(dirName,&statfsbuf)==-1) return false;
    if ((statfsbuf.f_type!=0x01021994) && (statfsbuf.f_type!=0x28cd3d45)) return false;
    if (access(dirName,R_OK|W_OK)==-1) return false;
    return true;

}

void DeleteFile(string filename) {
    remove(filename.c_str());
}
#include <iostream>
#include <string>

#include "Common.h"

using namespace std;

class DSMgr
{
public:
    DSMgr();
    ~DSMgr();
    
    int OpenFile(string filename);
    int CloseFile();
    bFrame ReadPage(int page_id);   // called by the FixPage function in the buffer manager
    int WritePage(int frame_id, bFrame frm);    // called whenever a page is taken out of the buffer
    int Seek(int offset, int pos);  // Seek function moves the file pointer.
    FILE * GetFile();
    void IncNumPages(); // increments the page counter.
    int GetNumPages();
    void SetUse(int page_id, int use_bit);
    int GetUse(int page_id);

private:
    FILE *currFile;
    int numPages;   // 当前page数量
    int pages[MAXPAGES];    // 各page的use_bit,为0则表示可以重用(reusable)
};
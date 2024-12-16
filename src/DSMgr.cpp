#include "DSMgr.h"

using namespace std;


DSMgr::DSMgr()
{
    numPages = 0;
    for (int i = 0; i < MAXPAGES; i++)  pages[i]=0;
}

DSMgr::~DSMgr()
{
    // 自动关闭文件，以免出现忘记close File的情况
    CloseFile();
}

int DSMgr::OpenFile(string filename)
{
    currFile = fopen(filename.c_str(), "r+");
    if (currFile == NULL)
    {
        currFile = fopen(filename.c_str(), "w+");
        currFile = freopen(filename.c_str(), "r+", currFile);
    }
    else
    {
        fseek(currFile, 0L, SEEK_END);
        long len = ftell(currFile);
        numPages = len / PAGESIZE;
        // 文件中已经使用的页的use_bit设为1
        for (int i = 0; i < numPages; i++)  pages[i] = 1;
    }
    return currFile != NULL;
}

int DSMgr::CloseFile()
{
    fclose(currFile);
}

bFrame DSMgr::ReadPage(int page_id)
{
    bFrame frm;
    if(fseek(currFile, page_id * PAGESIZE, SEEK_SET))
    {
        fprintf(stderr, "Error: can't find page %d\n", page_id);
        exit(1);
    }

    fread(frm.field, PAGESIZE, 1, currFile);
    SetUse(page_id, 1);
    fseek(currFile, 0, SEEK_SET);
    return frm;
}

int DSMgr::WritePage(int page_id, bFrame frm)
{
    if(fseek(currFile, page_id * PAGESIZE, SEEK_SET))
    {
        fprintf(stderr, "Error: can't find page %d\n", page_id);
        exit(1);
    }
    
    int result = fwrite(frm.field, PAGESIZE, 1, currFile);
    SetUse(page_id, 1);
    fseek(currFile, 0, SEEK_SET);
    return result;
}

int DSMgr::Seek(int offset, int pos)
{
    // 成功则返回0
    return fseek(currFile, offset, pos);
}

FILE * DSMgr::GetFile()
{
    return currFile;
}

void DSMgr::IncNumPages()
{
    numPages++;
}

int DSMgr::GetNumPages()
{
    return numPages;
}

void DSMgr::SetUse(int page_id, int use_bit)
{
    pages[page_id] = use_bit;
}

int DSMgr::GetUse(int page_id)
{
    return pages[page_id];
}
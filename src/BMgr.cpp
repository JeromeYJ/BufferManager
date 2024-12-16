#include "BMgr.h"

using namespace std;


BMgr::BMgr(string filename)
{
    if(!dsmgr.OpenFile(filename))
    {
        fprintf(stderr, "Error: can't open database file\n");
        exit(1);
    }
    for (int i = 0; i < DEFBUFSIZE; i++)
    {
        ftop[i] = -1;
        ptof[i] = new BCB();
    }
    numFreeFrames = DEFBUFSIZE;
}

BMgr::~BMgr()
{
    // 写脏页并进行一些指针的释放
    WriteDirtys();

}

int BMgr::FixPage(int page_id, int prot)
{

}

void BMgr::FixNewPage()
{
    // The fixNewPagefunction firsts checks this array for a use_bit of zero. If one is found, the page is reused.
    // If not, a new page is allocated.
    
}

int BMgr::UnfixPage(int page_id)
{

}

int BMgr::NumFreeFrames()
{
    return numFreeFrames;
}

int BMgr::SelectVictim()
{

}

BCB* BMgr::Hash(int page_id)
{
    // 所使用的哈希函数.使用拉链法哈希
    int index = page_id % DEFBUFSIZE;
    BCB* bcb = ptof[index];
    while(bcb != NULL)
    {
        if(bcb->page_id == page_id)
            return bcb;
        bcb = bcb->next;
    }
    return NULL;
}

void BMgr::RemoveBCB(int page_id) 
{
    int index = page_id % DEFBUFSIZE;
    BCB* bcb = ptof[index];
    if(bcb->page_id == page_id)
    {
        ptof[index] = bcb->next;
        ftop[bcb->frame_id] = -1;
        delete bcb;
    }
    else
    {
        while(bcb->next != NULL)
        {
            BCB* tmp = bcb->next;
            if(tmp->page_id == page_id)
            {
                bcb->next = tmp->next;
                ftop[tmp->frame_id] = -1;
                delete tmp;
                return;
            }
            bcb = bcb->next;
        }
    }
}

void BMgr::RemoveLRUEle(int frid)    
{

}

BCB* BMgr::FrameToBCB(int frame_id)
{
    int page_id = ftop[frame_id];
    if(page_id == -1)
    {
        fprintf(stderr, "Error: can't find corresponding page\n");
        exit(1);
    }

    return Hash(page_id);
}

void BMgr::SetDirty(int frame_id)    
{
    BCB* bcb = FrameToBCB(frame_id);
    if(bcb == NULL)
    {
        fprintf(stderr, "Error: can't find corresponding BCB\n");
        exit(1);
    }

    bcb->dirty = 1;
}

void BMgr::UnsetDirty(int frame_id)
{
    BCB* bcb = FrameToBCB(frame_id);
    if(bcb == NULL)
    {
        fprintf(stderr, "Error: can't find corresponding BCB\n");
        exit(1);
    }

    bcb->dirty = 0;
}

void BMgr::WriteDirtys()
{
    // 检查所有frame，需要写脏的进行写脏
    // 由于只在析构函数中调用，所以同时需要delete各BCB(delete对应构造函数里的new)
    for (int i = 0; i < DEFBUFSIZE; i++)
    {
        BCB* bcb = ptof[i];
        BCB* tmp;
        while(bcb->next != NULL)
        {
            tmp = bcb->next;
            if(tmp->dirty == 1)
                dsmgr.WritePage(tmp->page_id, buffer[tmp->frame_id]);
            bcb->next = tmp->next;
            delete tmp;
        }
        delete bcb;
    }
}

void BMgr::PrintFrame(int frame_id)
{
    BCB* bcb = FrameToBCB(frame_id);
    printf("frame_id: %d\n", bcb->frame_id);
    printf("page_id: %d\n", bcb->page_id);
}
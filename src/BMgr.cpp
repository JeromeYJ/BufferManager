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
    replacer = new LRU();
    numDiskIO = 0;
    numHit = 0;
}

BMgr::~BMgr()
{
    // 写脏页并进行一些指针的释放
    WriteDirtys();
    delete replacer;
}

int BMgr::FixPage(int page_id, int op)
{
    BCB* bcb = Hash(page_id);
    int frame_id;

    // 当对应page在buffer中
    if(bcb != NULL)
    {
        frame_id = bcb->frame_id;
        if(bcb->count == 0)
            replacer->Fix(frame_id);
        bcb->count++;
        if(op == 1)
            bcb->dirty = 1;

        numHit++;
    }
    // 当对应page不在buffer中
    else
    {
        if(NumFreeFrames())
        {
            frame_id = DEFBUFSIZE - NumFreeFrames();
            numFreeFrames--;
        }
        else
            frame_id = SelectVictim();
        
        // 更新buffer
        buffer[frame_id] = dsmgr.ReadPage(page_id);
        numDiskIO++;
        // 更新ftop
        ftop[frame_id] = page_id;

        // 更新ptof
        BCB* newBcb = new BCB();
        if(op == 1)
            newBcb->dirty = 1;
        newBcb->page_id = page_id;
        newBcb->frame_id = frame_id;
        newBcb->count = 1;

        int index = page_id % DEFBUFSIZE;
        BCB* head = ptof[index];
        if(head == NULL)
            ptof[index] = newBcb;
        else
        {
            newBcb->next = head->next;
            head->next = newBcb;
        }

        replacer->Fix(frame_id);
    }

    return frame_id;
}

void BMgr::FixNewPage()
{
    // The fixNewPagefunction firsts checks this array for a use_bit of zero. If one is found, the page is reused.
    // If not, a new page is allocated.
    for (int i = 0; i < MAXPAGES; i++)
    {
        if(!dsmgr.GetUse(i))
        {
            dsmgr.SetUse(i, 1);
            return;
        }
    }

    if(dsmgr.GetNumPages() == MAXPAGES)
    {
        fprintf(stderr, "Error: too many pages\n");
        exit(1);
    }
    int page_id = dsmgr.GetNumPages();
    dsmgr.IncNumPages();
    char data[PAGESIZE];
    dsmgr.Seek(0, SEEK_END);
    fwrite(data, PAGESIZE, 1, dsmgr.GetFile());
    dsmgr.SetUse(page_id, 1);
}

int BMgr::UnfixPage(int page_id)
{
    BCB* bcb = Hash(page_id);
    if(bcb == NULL)
        return -1;
    
    bcb->count--;
    if(bcb->count == 0)
        replacer->UnFix(bcb->frame_id);
    return bcb->frame_id;
}

int BMgr::NumFreeFrames()
{
    return numFreeFrames;
}

int BMgr::SelectVictim()
{
    int frame_id = replacer->SelectVictim();

    // 更新ptof，删除对应BCB
    RemoveBCB(ftop[frame_id]);

    // 更新ftop
    ftop[frame_id] = -1;

    return frame_id;
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
        if(bcb->dirty == 1)
        {
            dsmgr.WritePage(page_id, buffer[bcb->frame_id]);
            numDiskIO++;
        }
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
                if(tmp->dirty == 1)
                {
                    dsmgr.WritePage(page_id, buffer[tmp->frame_id]);
                    numDiskIO++;
                }
                bcb->next = tmp->next;
                ftop[tmp->frame_id] = -1;
                delete tmp;
                return;
            }
            bcb = bcb->next;
        }
    }
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
            {
                dsmgr.WritePage(tmp->page_id, buffer[tmp->frame_id]);
                numDiskIO++;
            }
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

long long BMgr::GetNumHit()
{
    return numHit;
}

long long BMgr::GetNumDiskIO()
{
    return numDiskIO;
}
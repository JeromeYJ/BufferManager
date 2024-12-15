#include <iostream>
#include "Common.h"

class BMgr
{
public:
    BMgr();
    ~BMgr();

    // Interface functions
    int FixPage(int page_id, int prot);
    void FixNewPage();
    int UnfixPage(int page_id);
    int NumFreeFrames();
    // Internal Functions
    int SelectVictim();
    int Hash(int page_id);
    void RemoveBCB(BCB * ptr, int page_id); // removes the Buffer Control Block for the page_id from the array
    void RemoveLRUEle(int frid);    // removes the LRU element from the list
    void SetDirty(int frame_id);    // if the bit is 1, it will be written. If this bit is zero, it will not be written
    void UnsetDirty(int frame_id);  // set bit 0
    void WriteDirtys(); // WriteDirtys function must be called when the system is shut down.
    void PrintFrame(int frame_id);  //  prints out the contents of the frame described by the frame_id.

private:
    // Hash Table
    int ftop[DEFBUFSIZE];
    BCB* ptof[DEFBUFSIZE];
};
#include <stdio.h>
#include <string>

using namespace std;

#define DEFBUFSIZE 1024   // buffer大小（页帧数量）
#define FRAMESIZE 4096    // 页帧大小
#define PAGESIZE 4096     // 页大小
#define MAXPAGES 50000    // 磁盘最大页数


// 表示页帧frame的结构体
struct bFrame
{
    char field[FRAMESIZE];
};


// Buffer Control Blocks
struct BCB
{
    int page_id;
    int frame_id;
    int latch;
    int count;
    int dirty;
    BCB * next;
    BCB()
    {
        page_id = -1;
        frame_id = -1;
        count = 0;
        dirty = 0;
        next = NULL;
    }
};
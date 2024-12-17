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


// 替换算法中会用到的双向链表结点类型
struct Node
{
    Node *pre, *next;
    int frame_id;
    Node(int id)
    {
        pre = NULL;
        next = NULL;
        frame_id = id;
    }
};


// 缓存替换算法父类
class Replacer
{
public:
    Replacer() = default;
    virtual ~Replacer() = default;
    // 表示正在访问某一页帧
    virtual void Fix(int frame_id) = 0;
    // 表示结束访问某一页帧
    virtual void UnFix(int frame_id) = 0;
    // 选择替换的页帧
    virtual int SelectVictim() = 0;
};
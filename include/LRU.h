#include "Common.h"

using namespace std;


class LRU : public Replacer
{
public:
    LRU();
    ~LRU();
    void Fix(int frame_id);
    void UnFix(int frame_id);
    int SelectVictim();

private:
    Node *head, *tail;
    
    // 查找对应frame_id的Node结点的函数
    Node* SearchNode(int frame_id);
    // 删除LRU链表末尾结点，用于victim
    void DeleteNode();
    // 在LRU链表头部插入结点，用于加入新结点时
    void InsertNode(int frame_id);
};
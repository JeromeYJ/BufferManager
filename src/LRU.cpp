#include "LRU.h"

using namespace std;


LRU::LRU()
{
    head = new Node(-1);
    tail = new Node(-1);
    head->next = tail;
    tail->pre = head;
}

LRU::~LRU()
{
    Node* node;
    while(head->next != NULL)
    {
        node = head->next;
        head->next = node->next;
        delete node;
    }
    delete head;
}

void LRU::Fix(int frame_id)
{
    Node* node = SearchNode(frame_id);
    if(node != NULL)
    {
        Node* nextNode = node->next;
        Node* preNode = node->pre;
        preNode->next = nextNode;
        nextNode->pre = preNode;
        delete node;
        return;
    }
}

void LRU::UnFix(int frame_id)
{
    InsertNode(frame_id);
}

int LRU::SelectVictim()
{
    Node* node = tail->pre;
    if(node == head)
        return -1;
    int frame_id = node->frame_id;
    DeleteNode();
    return frame_id;
}

Node* LRU::SearchNode(int frame_id)
{
    Node* node = head->next;
    while(node != tail)
    {
        if(node->frame_id == frame_id)
            return node;
        node = node->next;
    }
    return NULL;
}

void LRU::DeleteNode()
{
    Node* node = tail->pre;
    if(node == head)
        return;
    
    Node* newEnd = node->pre;
    newEnd->next = tail;
    tail->pre = newEnd;
    delete node;
}

void LRU::InsertNode(int frame_id)
{
    Node* node = new Node(frame_id);
    node->next = head->next;
    node->next->pre = node;
    head->next = node;
    node->pre = head;
}
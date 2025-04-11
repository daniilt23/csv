#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct Node
{
    void* data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct
{
    Node* head;
    Node* tail;
    size_t size;
    size_t dataSize;
} LinkedList;

void initLinkedList(LinkedList* list, size_t dataSize);
void disposeLinkedList(LinkedList* list);
bool pushBack(LinkedList* list, const void* data);

#endif // LINKEDLIST_H

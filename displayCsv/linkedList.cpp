#include "linkedList.h"
#include <string.h>
#include <stdlib.h>

Node* allocateNode(const void* data, size_t size)
{
    Node* node = (Node*)malloc(sizeof(Node) + size);
    if (node != 0)
    {
        node->data = (void*)((char*)node + sizeof(Node));
        node->next = 0;
        node->prev = 0;
        memcpy(node->data, data, size);
    }

    return node;
}

void freeNode(Node* node)
{
    node->next = 0;
    node->prev = 0;
    free(node);
}

void initLinkedList(LinkedList* list, size_t dataSize)
{
    list->head = 0;
    list->tail = 0;
    list->size = 0;
    list->dataSize = dataSize;
}

void disposeLinkedList(LinkedList* list)
{
    Node* current = list->head;
    while (current)
    {
        Node* next = current->next;
        freeNode(current);
        current = next;
    }
    list->head = 0;
    list->tail = 0;
    list->size = 0;
}

bool pushBack(LinkedList* list, const void* data)
{
    Node* node = allocateNode(data, list->dataSize);
    if (!node)
        return false;

    node->prev = list->tail;
    if (list->tail)
        list->tail->next = node;
    else
        list->head = node;
    list->tail = node;
    list->size++;

    return true;
}

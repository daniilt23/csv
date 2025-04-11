#ifndef ITERATOR_H
#define ITERATOR_H

#include "linkedList.h"

typedef struct
{
    Node* current;
} Iterator;

Iterator begin(const LinkedList* list);
Iterator end(const LinkedList* list);
bool hasNext(Iterator* it);
bool next(Iterator* it);
bool prev(Iterator* it);
void* get(Iterator* it);
bool set(Iterator* it, const void* data, size_t data_size);
bool insertAfter(LinkedList* list, Iterator* it, const void* data);
bool removeAt(LinkedList* list, Iterator* it);

#endif // ITERATOR_H

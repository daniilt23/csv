#include "iterator.h"

Iterator begin(const LinkedList* list)
{
    return Iterator { .current = list->head };
}

bool next(Iterator* it)
{
    return it->current && (it->current = it->current->next);
}

bool hasNext(Iterator* it)
{
    return it->current != 0;
}

void* get(Iterator* it)
{
    return it->current ? it->current->data : 0;
}

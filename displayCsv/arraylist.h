#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdlib.h>
#include <string.h>
#include "arrayiterator.h"
#include "appcontext.h"
#include "demographicData.h"

typedef struct
{
    float* data;
    size_t size;
    size_t capacity;
} ArrayList;

void arrayListInit(ArrayList* list);
void arrayListDispose(ArrayList* list);
void arrayListAdd(ArrayList* list, float value);
ArrayIterator arrayListBegin(ArrayList* list);
ArrayIterator arrayListEnd(ArrayList* list);
size_t arrayListSize(const ArrayList* list);

#endif // ARRAYLIST_H

#ifndef ARRAYITERATOR_H
#define ARRAYITERATOR_H
#include <stdio.h>

typedef struct
{
    float* current;
} ArrayIterator;

ArrayIterator arrayBegin(float* array);
ArrayIterator arrayEnd(float* array, size_t size);
bool arrayNext(ArrayIterator* it);
bool arrayPrev(ArrayIterator* it);
float arrayGet(const ArrayIterator* it);
bool arrayItEquals(const ArrayIterator* a, const ArrayIterator* b);

#endif // ARRAYITERATOR_H

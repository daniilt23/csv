#include "arrayiterator.h"

ArrayIterator arrayBegin(float* array)
{
    return (ArrayIterator){array};
}

ArrayIterator arrayEnd(float* array, size_t size)
{
    return (ArrayIterator){array + size};
}

bool arrayNext(ArrayIterator* it)
{
    bool status = true;
    if (!it->current)
        status = false;
    it->current++;

    return status;
}

bool arrayPrev(ArrayIterator* it)
{
    bool status = true;
    if (!it->current)
        status = false;
    it->current--;

    return status;
}

float arrayGet(const ArrayIterator* it)
{
    return it->current ? *(it->current) : 0;
}

bool arrayItEquals(const ArrayIterator* a, const ArrayIterator* b)
{
    return a->current == b->current;
}

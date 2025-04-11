#include "arraylist.h"

void arrayListInit(ArrayList* list)
{
    list->data = 0;
    list->size = 0;
    list->capacity = 0;
}

void arrayListDispose(ArrayList* list)
{
    free(list->data);
    list->data = 0;
    list->size = list->capacity = 0;
}

void arrayListAdd(ArrayList* list, float value)
{
    if (list->size >= list->capacity)
    {
        size_t newCapacity = list->capacity == 0 ? SIZE : list->capacity * TWO;
        void* newData = realloc(list->data, newCapacity * sizeof(float));
        if (!newData)
            return;

        list->data = (float*)newData;
        list->capacity = newCapacity;
    }

    list->data[list->size++] = value;
}

ArrayIterator arrayListBegin(ArrayList* list)
{
    return arrayBegin(list->data);
}

ArrayIterator arrayListEnd(ArrayList* list)
{
    return arrayEnd(list->data, list->size);
}

size_t arrayListSize(const ArrayList* list)
{
    return list->size;
}

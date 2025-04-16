#include "sort.h"

void arraySwap(ArrayIterator* a, ArrayIterator* b);
ArrayIterator arrayPartition(ArrayIterator* low, ArrayIterator* high);

void sortArrayWithIterators(float* array, size_t size)
{
    ArrayIterator begin = arrayBegin(array);
    ArrayIterator end = arrayEnd(array, size);
    arrayPrev(&end);

    arrayQuickSort(begin, end);
}

void arraySwap(ArrayIterator* a, ArrayIterator* b)
{
    float tmp = arrayGet(a);
    *(a->current) = arrayGet(b);
    *(b->current) = tmp;
}

ArrayIterator arrayPartition(ArrayIterator low, ArrayIterator high)
{
    ArrayIterator pivot = high;
    ArrayIterator i = low;

    for (ArrayIterator j = low; !arrayItEquals(&j, &high); arrayNext(&j))
    {
        if (arrayGet(&j) <= arrayGet(&pivot))
        {
            arraySwap(&i, &j);
            arrayNext(&i);
        }
    }
    arraySwap(&i, &high);

    return i;
}

void arrayQuickSort(ArrayIterator low, ArrayIterator high)
{
    if (arrayItEquals(&low, &high))
        return;
    if (low.current >= high.current)
        return;

    ArrayIterator pi = arrayPartition(low, high);

    ArrayIterator piPrev = pi;
    arrayPrev(&piPrev);
    arrayQuickSort(low, piPrev);

    ArrayIterator piNext = pi;
    arrayNext(&piNext);
    arrayQuickSort(piNext, high);
}

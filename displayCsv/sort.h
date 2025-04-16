#ifndef SORT_H
#define SORT_H
#include <stdlib.h>
#include "arrayiterator.h"

void sortArrayWithIterators(float* array, size_t size);
void arrayQuickSort(ArrayIterator low, ArrayIterator high);

#endif // SORT_H

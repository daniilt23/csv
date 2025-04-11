#ifndef LOGIC_H
#define LOGIC_H

#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <ctype.h>
#include "mystring.h"
#include "appcontext.h"
#include "dataStorage.h"
#include "iterator.h"
#include "arrayiterator.h"
#include <arraylist.h>
#include "param.h"

void initContext(AppContext* context);
void clearContext(AppContext* context);
bool checkLine(const char* line);
void readCSVRows(AppContext* context, AppParam* param);
void regionCheck(AppContext* context);
void readDataForRegion(AppContext* context);
float getColumnValue(void* data, int column);
void calculateMetrics(AppContext* context, AppParam* param);
ErrorStatus initCSVReading(AppContext* context, FILE* file);
void processCSVData(AppContext* context, FILE* file);
float calculateMedian(const LinkedList* list, int column);
void sortArrayWithIterators(float* array, size_t size);
ArrayIterator arrayPartition(ArrayIterator low, ArrayIterator high);
void arrayQuickSort(ArrayIterator low, ArrayIterator high);

#endif // LOGIC_H

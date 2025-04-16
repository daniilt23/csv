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
#include "linkedList.h"
#include "arrayiterator.h"
#include <arraylist.h>
#include "param.h"
#include "sort.h"

void initContext(AppContext* context);
void initLineInfo(AppContext* context);
void initMetrics(AppContext* context);
bool checkLine(const char* line);
void readCSVRows(AppContext* context, AppParam* param);
void readDataForRegion(AppContext* context);
float getColumnValue(void* data, int column);
void calculateMetrics(AppContext* context, AppParam* param);
void processCSVData(AppContext* context, FILE* file);
float calculateMedian(const LinkedList* list, int column);
double getValueForCurrentColumn(AppParam* param, const DemographicData* data);
void parseSpecificLine(DemographicData* data, char** tokens);
void doCalculateMetrics(AppContext* context, ArrayList& values);

#endif // LOGIC_H

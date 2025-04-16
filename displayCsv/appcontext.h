#ifndef APPCONTEXT_H
#define APPCONTEXT_H
#include "programstatus.h"
#include "linkedList.h"
#include "lineinfo.h"
#include "metrics.h"
#include "regionData.h"
#include <stdio.h>

enum
{
    ONE = 1,
    TWO,
};

struct AppContext
{
    char region[SIZE];
    LinkedList* dataList;
    LinkedList* regionList;
    LineInfo* lineInfo;
    Metrics* metrics;
    ProgramStatus* error;
};

#endif // APPCONTEXT_H

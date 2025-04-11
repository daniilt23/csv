#ifndef APPCONTEXT_H
#define APPCONTEXT_H
#include "errorName.h"
#include "linkedList.h"
#include "lineinfo.h"
#include "metrics.h"
#include "regionData.h"
#include <stdio.h>

enum
{
    ONE = 1,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
};

struct AppContext
{
    char region[SIZE];
    LinkedList dataList;
    LinkedList regionList;
    LineInfo lineInfo;
    Metrics metrics;
    ErrorStatus error;
};

#endif // APPCONTEXT_H

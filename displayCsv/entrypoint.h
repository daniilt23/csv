#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H
#include <stdlib.h>
#include "appcontext.h"
#include "dataStorage.h"
#include "param.h"
#include "logic.h"


enum Operation
{
    READ_CSV_FILE,
    READ_DATA_FOR_REGION,
    CALCULATE,
    INIT_CONTEXT
};

void doOperation(Operation operation, AppContext* context, AppParam* param);

#endif // ENTRYPOINT_H

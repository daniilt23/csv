#include "entrypoint.h"

void doOperation(Operation operation, AppContext* context, AppParam* param)
{
    switch(operation)
    {
    case READ_CSV_FILE:
        readCSVRows(context, param);
        break;
    case READ_DATA_FOR_REGION:
        readDataForRegion(context);
        break;
    case CALCULATE:
        calculateMetrics(context, param);
        break;
    }
}

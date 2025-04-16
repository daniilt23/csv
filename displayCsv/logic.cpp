#include "logic.h"

void initContext(AppContext* context)
{
    context->lineInfo = (LineInfo*)malloc(sizeof(LineInfo));
    context->dataList = (LinkedList*)malloc(sizeof(LinkedList));
    initLinkedList(context->dataList, sizeof(DemographicData));
    context->regionList = (LinkedList*)malloc(sizeof(LinkedList));
    initLinkedList(context->regionList, sizeof(DemographicData));
    context->metrics = (Metrics*)malloc(sizeof(Metrics));
    context->error = (ProgramStatus*)malloc(sizeof(ProgramStatus));
    *context->error = OK;
    initLineInfo(context);
    initMetrics(context);
}

void initLineInfo(AppContext* context)
{
    context->lineInfo->errorLines = 0;
}

void initMetrics(AppContext* context)
{
    context->metrics->min = 0;
    context->metrics->max = 0;
    context->metrics->med = 0;
}

void parseSpecificLine(DemographicData* data, char** tokens)
{
    data->year = atoi(tokens[0]);
    strncpy(data->region, tokens[REGION], SIZE - ONE);
    data->npg = atof(tokens[NPG]);
    data->birthRate = atof(tokens[BIRTH_RATE]);
    data->deathRate = atof(tokens[DEATH_RATE]);
    data->gdw = atof(tokens[GDW]);
    data->urbanization = atof(tokens[URBANIZATION]);
}

void processCSVData(AppContext* context, FILE* file)
{
    char line_buf[SIZE];
    fgets(line_buf, SIZE, file);
    while (fgets(line_buf, SIZE, file))
    {
        char** words = nullptr;
        int tokenCount = parseWords(&words, line_buf, ",");
        if (tokenCount == MAX_FIELD_COUNT)
        {
            DemographicData data;
            parseSpecificLine(&data, words);
            pushBack(context->dataList, &data);
        }
        else
            context->lineInfo->errorLines++;
        for (int i = 0; i < tokenCount; i++)
            free(words[i]);
        free(words);
    }
}

void readCSVRows(AppContext* context, AppParam* param)
{
    FILE* file = fopen(param->fileName, "r");
    if (file)
        processCSVData(context, file);
    fclose(file);
}

void regionCheck(AppContext* context)
{
    bool regionExists = false;
    Iterator it = begin(context->dataList);

    while (it.current != 0)
    {
        DemographicData* data = (DemographicData*)get(&it);
        if (strcmp(data->region, context->region) == 0)
        {
            regionExists = true;
            break;
        }
        next(&it);
    }

    *context->error = regionExists ? OK : INCORRECT_REGION;
}

void readDataForRegion(AppContext* context)
{
    disposeLinkedList(context->regionList);
    initLinkedList(context->regionList, sizeof(DemographicData));

    Iterator it = begin(context->dataList);
    while (hasNext(&it))
    {
        DemographicData* data = (DemographicData*)get(&it);
        if (strcmp(data->region, context->region) == 0)
            pushBack(context->regionList, data);
        next(&it);
    }
}

float getColumnValue(void* data, int column)
{
    DemographicData* d = (DemographicData*)data;
    float temp = NAN;
    switch (column)
    {
    case YEAR:
        temp = (float)d->year;
        break;
    case NPG:
        temp = d->npg;
        break;
    case BIRTH_RATE:
        temp = d->birthRate;
        break;
    case DEATH_RATE:
        temp = d->deathRate;
        break;
    case GDW:
        temp = d->gdw;
        break;
    case URBANIZATION:
        temp = d->urbanization;
        break;
    }

    return temp;
}

void calculateMetrics(AppContext* context, AppParam* param) {
    ArrayList values;
    arrayListInit(&values);
    Iterator it = begin(context->regionList);
    while (it.current != 0)
    {
        arrayListAdd(&values, getColumnValue(get(&it), param->column));
        next(&it);
    }
    doCalculateMetrics(context, values);
    arrayListDispose(&values);
}

void doCalculateMetrics(AppContext* context, ArrayList& values)
{
    ArrayIterator begin = arrayListBegin(&values);
    ArrayIterator end = arrayListEnd(&values);
    if (!isEmpty(&values))
    {
        arrayPrev(&end);
        arrayQuickSort(begin, end);
        context->metrics->min = arrayGet(&begin);
        ArrayIterator last = begin;
        for (size_t i = ONE; i < arrayListSize(&values); i++)
            arrayNext(&last);
        context->metrics->max = arrayGet(&last);
        if (arrayListSize(&values) % TWO == 0)
        {
            ArrayIterator it1 = begin, it2 = begin;
            for (size_t i = 0; i < arrayListSize(&values) / TWO - ONE; i++) arrayNext(&it1);
            it2 = it1;
            arrayNext(&it2);
            context->metrics->med = (arrayGet(&it1) + arrayGet(&it2)) * 0.5;
        }
        else
        {
            ArrayIterator it = begin;
            for (size_t i = 0; i < arrayListSize(&values) / TWO; i++) arrayNext(&it);
            context->metrics->med = arrayGet(&it);
        }
    }
}

double getValueForCurrentColumn(AppParam* param, const DemographicData* data)
{
    double value = NAN;
    switch (param->column)
    {
    case YEAR:
        value = (float)data->year;
        break;
    case NPG:
        value = data->npg;
        break;
    case BIRTH_RATE:
        value = data->birthRate;
        break;
    case DEATH_RATE:
        value = data->deathRate;
        break;
    case GDW:
        value = data->gdw;
        break;
    case URBANIZATION:
        value = data->urbanization;
        break;
    }

    return value;
}


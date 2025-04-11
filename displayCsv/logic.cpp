#include "logic.h"

void initContext(AppContext* context)
{
    initLinkedList(&context->dataList, sizeof(DemographicData));
    initLinkedList(&context->regionList, sizeof(DemographicData));
    context->lineInfo.errorLines = 0;
    context->metrics.min = 0;
    context->metrics.max = 0;
    context->metrics.med = 0;
}

void clearContext(AppContext* context)
{
    disposeLinkedList(&context->dataList);
    disposeLinkedList(&context->regionList);
}

static bool parseCSVLine(const char* line, DemographicData* data)
{
    if (!line || !data)
        return false;

    char* copy = strdup(line);
    if (!copy)
        return false;

    char** tokens = 0;
    int tokenCount = parseWords(&tokens, copy, ",");
    bool success = (tokenCount == SEVEN);
    if (tokenCount < SEVEN)
    {
        if (tokens)
        {
            for (int i = 0; i < tokenCount; i++)
                if (tokens[i])
                    free(tokens[i]);
            free(tokens);
        }
        free(copy);
    }
    else
    {
        data->year = atoi(tokens[0]);
        strncpy(data->region, tokens[ONE], SIZE - ONE);
        data->region[SIZE - TWO] = '\0';
        data->npg = atof(tokens[TWO]);
        data->birthRate = atof(tokens[THREE]);
        data->deathRate = atof(tokens[FOUR]);
        data->gdw = atof(tokens[FIVE]);
        data->urbanization = atof(tokens[SIX]);
    }

    for (int i = 0; i < tokenCount; i++)
        free(tokens[i]);
    free(tokens);
    free(copy);

    return success;
}

bool checkLine(const char* line)
{
    char* copy = strdup(line);
    char* token = strtok(copy, ",");
    int count = 0;

    while (token && count < MAX_FIELD_COUNT)
    {
        count++;
        token = strtok(0, ",");
    }

    free(copy);

    return count == MAX_FIELD_COUNT;
}

ErrorStatus initCSVReading(AppContext* context, FILE* file)
{
    ErrorStatus temp = OK;
    if (!file)
        temp = CANNOT_OPEN_FILE;
    char line_buf[SIZE];
    if (!fgets(line_buf, SIZE, file))
    {
        fclose(file);
        temp = ERROR;
    }
    context->lineInfo.errorLines = 0;

    return temp;
}

void processCSVData(AppContext* context, FILE* file)
{
    char line_buf[SIZE];
    while (fgets(line_buf, SIZE, file))
    {
        char* line = strdup(line_buf);
        if (!line)
            continue;
        char** words = nullptr;
        int temp = parseWords(&words, line, ",");
        if (temp == MAX_FIELD_COUNT)
        {
            DemographicData data;
            if (parseCSVLine(line_buf, &data))
                pushBack(&context->dataList, &data);
            else
                context->lineInfo.errorLines++;
        }
        else
        {
            context->lineInfo.errorLines++;
            context->dataList.size++;
        }
        free(line);
    }
    fclose(file);
}

void readCSVRows(AppContext* context, AppParam* param)
{
    FILE* file = fopen(param->fileName, "r");
    context->error = initCSVReading(context, file);
    if (context->error == OK)
        processCSVData(context, file);
}

void regionCheck(AppContext* context)
{
    bool regionExists = false;
    Iterator it = begin(&context->dataList);

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

    context->error = regionExists ? OK : INCORRECT_REGION;
}

void readDataForRegion(AppContext* context)
{
    disposeLinkedList(&context->regionList);
    initLinkedList(&context->regionList, sizeof(DemographicData));

    Iterator it = begin(&context->dataList);
    while (hasNext(&it))
    {
        DemographicData* data = (DemographicData*)get(&it);
        if (strcmp(data->region, context->region) == 0)
            pushBack(&context->regionList, data);
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
    Iterator it = begin(&context->regionList);
    while (it.current != 0)
    {
        arrayListAdd(&values, getColumnValue(get(&it), param->column));
        next(&it);
    }
    ArrayIterator begin = arrayListBegin(&values);
    ArrayIterator end = arrayListEnd(&values);
    if (arrayListSize(&values) > 0)
    {
        arrayPrev(&end);
        arrayQuickSort(begin, end);
    }
    if (arrayListSize(&values) > 0)
    {
        context->metrics.min = arrayGet(&begin);
        ArrayIterator last = begin;
        for (size_t i = ONE; i < arrayListSize(&values); i++)
            arrayNext(&last);
        context->metrics.max = arrayGet(&last);
        if (arrayListSize(&values) % TWO == 0)
        {
            ArrayIterator it1 = begin, it2 = begin;
            for (size_t i = 0; i < arrayListSize(&values) / TWO - ONE; i++) arrayNext(&it1);
            it2 = it1;
            arrayNext(&it2);
            context->metrics.med = (arrayGet(&it1) + arrayGet(&it2)) * 0.5;
        }
        else
        {
            ArrayIterator it = begin;
            for (size_t i = 0; i < arrayListSize(&values) / TWO; i++) arrayNext(&it);
            context->metrics.med = arrayGet(&it);
        }
    }

    arrayListDispose(&values);
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

void sortArrayWithIterators(float* array, size_t size)
{
    ArrayIterator begin = arrayBegin(array);
    ArrayIterator end = arrayEnd(array, size);
    arrayPrev(&end);

    arrayQuickSort(begin, end);
}

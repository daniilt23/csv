#ifndef DEMOGRAPHICDATA_H
#define DEMOGRAPHICDATA_H

enum
{
    MAX_FIELD_COUNT = 7,
    SIZE = 200
};

enum Column
{
    YEAR,
    REGION,
    NPG,
    BIRTH_RATE,
    DEATH_RATE,
    GDW,
    URBANIZATION
};

struct DemographicData
{
    int year;
    char region[SIZE];
    float npg;
    float birthRate;
    float deathRate;
    float gdw;
    float urbanization;
};

#endif // DEMOGRAPHICDATA_H

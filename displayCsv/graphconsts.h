#ifndef GRAPHCONSTS_H
#define GRAPHCONSTS_H

#define PADDING_COEFF 0.1

enum
{
    NEG_TWENTY = -20,
    NEG_TEN = -10,
    ZERO = 0,
    THREE = 3,
    FIVE = 5,
    TEN = 10,
    FOURTY = 40,
    MARGIN = 50,
};


typedef struct
{
    double min;
    double max;
} AxisLimits;

struct GraphBounds
{
    AxisLimits x;
    AxisLimits y;
    double paddingX;
    double paddingY;
};


#endif // GRAPHCONSTS_H

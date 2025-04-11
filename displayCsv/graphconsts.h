#ifndef GRAPHCONSTS_H
#define GRAPHCONSTS_H

#define PADDING_COEFF 0.1

enum
{
    NEG_TWENTY = -20,
    NEG_TEN = -10,
    ZERO = 0,
    TEN = 10,
    FOURTY = 40,
    MARGIN = 50,
};


struct GraphBounds
{
    double minX;
    double maxX;
    double minY;
    double maxY;
    double paddingX;
    double paddingY;
};


#endif // GRAPHCONSTS_H

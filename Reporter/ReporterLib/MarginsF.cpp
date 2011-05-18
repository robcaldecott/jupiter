#include "MarginsF.h"

MarginsF::MarginsF() :
    left_(0),
    top_(0),
    right_(0),
    bottom_(0)
{
}

MarginsF::MarginsF(qreal left, qreal top, qreal right, qreal bottom) :
    left_(left),
    top_(top),
    right_(right),
    bottom_(bottom)
{
}

qreal MarginsF::left() const
{
    return left_;
}

qreal MarginsF::top() const
{
    return top_;
}

qreal MarginsF::right() const
{
    return right_;
}

qreal MarginsF::bottom() const
{
    return bottom_;
}

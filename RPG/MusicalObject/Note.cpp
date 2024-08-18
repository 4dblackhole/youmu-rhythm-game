#include "framework.h"
#include "Note.h"

bool MusicalObject::operator<(const MusicalObject& v) const
{
    return this->mp < v.mp;
}

bool MusicalObject::operator>(const MusicalObject& v) const
{
    return this->mp > v.mp;
}

bool MusicalObject::operator==(const MusicalObject& v) const
{
    return this->mp == v.mp;
}

bool MusicalPosition::operator<(const MusicalPosition& v) const
{
    if (this->measureIdx < v.measureIdx) return true;
    if (this->measureIdx == v.measureIdx)
    {
        if (this->position < v.position) return true;
    }
    return false;
}

bool MusicalPosition::operator>(const MusicalPosition& v) const
{
    return v < *this;
}

bool MusicalPosition::operator<=(const MusicalPosition& v) const
{
    return !(*this > v);
}

bool MusicalPosition::operator>=(const MusicalPosition& v) const
{
    return !(*this < v);
}

bool MusicalPosition::operator==(const MusicalPosition& v) const
{
    return (this->measureIdx == v.measureIdx && this->position == v.position);
}

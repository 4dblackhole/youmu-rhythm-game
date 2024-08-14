#include "framework.h"
#include "Note.h"

bool MusicalObject::operator<(const MusicalObject& v) const
{
    if (this->measureIdx < v.measureIdx) return true;
    if (this->measureIdx == v.measureIdx)
    {
        if (this->position < v.position) return true;
    }
    return false;
}

bool MusicalObject::operator==(const MusicalObject& v) const
{
    return (this->measureIdx == v.measureIdx && this->position == v.position);
}
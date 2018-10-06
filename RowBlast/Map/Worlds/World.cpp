#include "World.hpp"

#include <assert.h>

using namespace RowBlast;

MapPlace::MapPlace(const MapLevel& mapLevel) :
    mKind {Kind::MapLevel},
    mMapLevel {mapLevel} {}

MapPlace::MapPlace(const Portal& portal) :
    mKind {Kind::Portal},
    mPortal {portal} {}

const MapLevel& MapPlace::GetMapLevel() const {
    assert(mKind == Kind::MapLevel);
    return mMapLevel;
}

const Portal& MapPlace::GetPortal() const {
    assert(mKind == Kind::Portal);
    return mPortal;
}

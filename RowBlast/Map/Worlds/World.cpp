#include "World.hpp"

#include <assert.h>

#include "World1.hpp"
#include "World2.hpp"

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

World::World(const std::vector<MapPlace>& mapPlaces,
             const std::vector<BlockPathVolume>& blockPaths,
             const std::vector<CloudPathVolume>& cloudPaths,
             const std::vector<HazeLayer>& hazeLayers) :
    mPlaces {mapPlaces},
    mBlockPaths {blockPaths},
    mCloudPaths {cloudPaths},
    mHazeLayers {hazeLayers} {}

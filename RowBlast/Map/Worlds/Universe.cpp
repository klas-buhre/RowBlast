#include "Universe.hpp"

// Game includes.
#include "World1.hpp"
#include "World2.hpp"

using namespace RowBlast;

Universe::Universe() :
    mWorlds {GetWorld1(), GetWorld2()} {}

const World& Universe::GetWorld(int worldIndex) const {
    assert(worldIndex <= mWorlds.size());
    return mWorlds[worldIndex - 1];
}

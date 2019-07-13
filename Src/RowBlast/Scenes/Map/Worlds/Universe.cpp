#include "Universe.hpp"

// Game includes.
#include "World1.hpp"
#include "World2.hpp"
#include "World3.hpp"
#include "World4.hpp"

using namespace RowBlast;

Universe::Universe() :
    mWorlds {GetWorld1(), GetWorld2(), GetWorld3(), GetWorld4()} {}

const World& Universe::GetWorld(int worldId) const {
    assert(worldId <= mWorlds.size());
    return mWorlds[worldId - 1];
}

int Universe::CalcWorldId(int levelId) const {
    for (auto worldId = 1; worldId <= mWorlds.size(); ++worldId) {
        for (auto& place: GetWorld(worldId).mPlaces) {
            switch (place.GetKind()) {
                case MapPlace::Kind::MapLevel:
                    if (place.GetMapLevel().mLevelId == levelId) {
                        return worldId;
                    }
                    break;
                case MapPlace::Kind::Portal:
                    break;
            }
        }
    }

    return 1;
}

bool Universe::IsLastWorld(int worldId) const {
    assert(worldId <= mWorlds.size());
    return worldId == mWorlds.size();
}

#ifndef World_hpp
#define World_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "FloatingBlocks.hpp"
#include "Clouds.hpp"

namespace RowBlast {
    struct MapLevel {
        int mLevelIndex;
        Pht::Vec3 mPosition;
    };
    
    struct Portal {
        int mToWorldIndex;
        int mLevelIndexEquivalent;
        Pht::Vec3 mPosition;
    };

    struct MapPlace {
        enum class Kind {
            MapLevel,
            Portal
        };
        
        explicit MapPlace(const MapLevel& mapLevel);
        explicit MapPlace(const Portal& portal);
        
        const MapLevel& GetMapLevel() const;
        const Portal& GetPortal() const;
        
        Kind GetKind() const {
            return mKind;
        }
        
    private:
        Kind mKind;
        union {
            MapLevel mMapLevel;
            Portal mPortal;
        };
    };

    class World {
    public:
        World(const std::vector<MapPlace>& mapPlaces,
              const std::vector<BlockPathVolume>& blockPaths,
              const std::vector<CloudPathVolume>& cloudPaths,
              const std::vector<HazeLayer>& hazeLayers);
        
        const std::vector<MapPlace>& mPlaces;
        const std::vector<BlockPathVolume>& mBlockPaths;
        const std::vector<CloudPathVolume>& mCloudPaths;
        const std::vector<HazeLayer>& mHazeLayers;
    };
    
    World GetWorld(int worldIndex);
}

#endif

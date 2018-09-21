#ifndef World_hpp
#define World_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "Material.hpp"

// Game includes.
#include "FloatingBlocks.hpp"
#include "Clouds.hpp"
#include "Planets.hpp"

namespace RowBlast {
    struct MapLevel {
        int mLevelId;
        Pht::Vec3 mPosition;
    };
    
    struct Portal {
        int mDestinationWorldId;
        int mNextLevelId;
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

    struct World {
        const std::vector<MapPlace>& mPlaces;
        const std::vector<BlockPathVolume>& mBlockPaths;
        const std::vector<CloudPathVolume>& mCloudPaths;
        const std::vector<HazeLayer>& mHazeLayers;
        const std::vector<PlanetConfig>& mPlanets;
        float mLightIntensity {1.0f};
        Pht::Color mCloudColor;
    };
}

#endif

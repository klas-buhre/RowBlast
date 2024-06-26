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
#include "SunParticleEffect.hpp"
#include "Terrain.hpp"

namespace RowBlast {
    struct MapLevel {
        int mLevelId;
        Pht::Vec3 mPosition;
        Pht::Vec3 mUfoOffset {0.0f, 0.0f, 0.0f};
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
    
    struct BackgroundLight {
        Pht::Vec3 mDirection {-1.0f, 1.0f, 1.0f};
        float mIntensity {1.2f};
        Pht::Optional<Sun> mSun;
    };

    struct World {
        const std::vector<MapPlace>& mPlaces;
        const std::vector<TerrainSegment>& mTerrainSegments;
        const std::vector<BlockPathVolume>& mBlockPaths;
        const std::vector<CloudPathVolume>& mCloudPaths;
        const std::vector<HazeLayer>& mHazeLayers;
        const std::vector<PlanetConfig>& mPlanets;
        const std::vector<BackgroundLight>& mBackgroundLights;
        std::string mBackgroundTextureFilename;
        float mAmbientLightIntensity {1.0f};
        float mDirectionalLightIntensity {1.0f};
        Pht::Color mCloudColor;
    };
}

#endif

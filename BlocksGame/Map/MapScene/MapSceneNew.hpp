#ifndef MapSceneNew_hpp
#define MapSceneNew_hpp

#include <memory>

// Engine includes.
#include "Font.hpp"

// Game includes.
#include "FloatingCubes.hpp"
#include "Clouds.hpp"
#include "NextLevelParticleEffect.hpp"
#include "MapPin.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    class UserData;
    class Chapter;

    class MapSceneNew {
    public:
        MapSceneNew(Pht::IEngine& engine, const CommonResources& commonResources, UserData& userData);
        
        void Reset();
        void Update();
        void SetCameraPosition(float xPosition);
        
    private:
        void Load(const Chapter& chapter);
        
        Pht::IEngine& mEngine;
        UserData& mUserData;
        Pht::Vec3 mCameraPosition {0.0f, 0.0f, 0.0f};
        float mMapSizeX {0.0f};
        std::unique_ptr<NextLevelParticleEffect> mNextLevelParticleEffect;
        std::unique_ptr<FloatingCubes> mFloatingCubes;
        std::vector<std::unique_ptr<MapPin>> mPins;
        Pht::Font mFont;
    };
}

#endif

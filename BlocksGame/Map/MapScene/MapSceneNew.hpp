#ifndef MapSceneNew_hpp
#define MapSceneNew_hpp

#include <memory>

// Engine includes.
#include "Font.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class CameraComponent;
    class RenderableObject;
}

namespace BlocksGame {
    class CommonResources;
    class UserData;
    class Chapter;
    class Clouds;
    class NextLevelParticleEffect;
    class FloatingCubes;
    class MapPin;

    class MapSceneNew {
    public:
        MapSceneNew(Pht::IEngine& engine, const CommonResources& commonResources, UserData& userData);
        ~MapSceneNew() = default;
        
        void Reset();
        void Update();
        void SetCameraPosition(float xPosition);
        
    private:
        void CreateScene(const Chapter& chapter);
        void CreatePin(int level, const Pht::Vec3& position);
        void SetCameraAtCurrentLevel();
        
        Pht::IEngine& mEngine;
        UserData& mUserData;
        const CommonResources& mCommonResources;
        Pht::Scene* mScene {nullptr};
        Pht::CameraComponent* mCamera {nullptr};
        float mMapSizeX {0.0f};
        std::unique_ptr<NextLevelParticleEffect> mNextLevelParticleEffect;
        std::unique_ptr<Clouds> mClouds;
        std::unique_ptr<FloatingCubes> mFloatingCubes;
        std::vector<std::unique_ptr<MapPin>> mPins;
        MapPin* mPreviousPin {nullptr};
        std::shared_ptr<Pht::RenderableObject> mStarRenderable;
        Pht::Font mFont;
    };
}

#endif

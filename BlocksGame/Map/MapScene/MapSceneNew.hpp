#ifndef MapSceneNew_hpp
#define MapSceneNew_hpp

#include <memory>

// Engine includes.
#include "Font.hpp"

// Game includes.
#include "Clouds.hpp"
#include "FloatingCubes.hpp"
#include "MapPinNew.hpp"
#include "MapHudNew.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class CameraComponent;
    class SceneObject;
}

namespace BlocksGame {
    class CommonResources;
    class UserData;
    class Chapter;

    class MapSceneNew {
    public:
        MapSceneNew(Pht::IEngine& engine, const CommonResources& commonResources, UserData& userData);
        
        void Reset();
        void Unload();
        void Update();
        void SetCameraXPosition(float xPosition);
        float GetCameraXPosition() const;
        
        const std::vector<std::unique_ptr<MapPinNew>>& GetPins() const {
            return mPins;
        }
        
    private:
        void CreateScene(const Chapter& chapter);
        void CreatePins(const Chapter& chapter);
        void CreatePin(Pht::SceneObject& pinContainerObject, int level, const Pht::Vec3& position);
        void SetCameraAtCurrentLevel();
        
        Pht::IEngine& mEngine;
        UserData& mUserData;
        const CommonResources& mCommonResources;
        Pht::Scene* mScene {nullptr};
        Pht::CameraComponent* mCamera {nullptr};
        float mMapSizeX {55.0f};
        std::unique_ptr<Clouds> mClouds;
        std::unique_ptr<FloatingCubes> mFloatingCubes;
        std::vector<std::unique_ptr<MapPinNew>> mPins;
        MapPinNew* mPreviousPin {nullptr};
        std::shared_ptr<Pht::RenderableObject> mStarRenderable;
        Pht::Font mFont;
        Pht::Font mHudFont;
        std::unique_ptr<MapHudNew> mHud;
    };
}

#endif

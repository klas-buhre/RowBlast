#ifndef MapScene_hpp
#define MapScene_hpp

#include <memory>
#include <assert.h>

// Engine includes.
#include "Font.hpp"

// Game includes.
#include "Clouds.hpp"
#include "FloatingBlocks.hpp"
#include "MapPin.hpp"
#include "MapHud.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class CameraComponent;
    class LightComponent;
    class SceneObject;
}

namespace RowBlast {
    class CommonResources;
    class UserData;
    class Chapter;

    class MapScene {
    public:
        MapScene(Pht::IEngine& engine, const CommonResources& commonResources, UserData& userData);
        
        void Init();
        void Update();
        void SetCameraXPosition(float xPosition);
        float GetCameraXPosition() const;
        
        const std::vector<std::unique_ptr<MapPin>>& GetPins() const {
            return mPins;
        }

        Pht::SceneObject& GetPinsContainer() {
            assert(mPinsContainer);
            return *mPinsContainer;
        }

        Pht::SceneObject& GetUiViewsContainer() {
            assert(mUiViewsContainer);
            return *mUiViewsContainer;
        }
        
    private:
        void CreateScene(const Chapter& chapter);
        void CreatePins(const Chapter& chapter);
        void CreatePin(Pht::SceneObject& pinContainerObject, int level, const Pht::Vec3& position);
        void SetCameraAtCurrentLevel();
        void UpdateUiLightAnimation();
        
        Pht::IEngine& mEngine;
        UserData& mUserData;
        const CommonResources& mCommonResources;
        Pht::Scene* mScene {nullptr};
        Pht::CameraComponent* mCamera {nullptr};
        Pht::LightComponent* mUiLight {nullptr};
        float mLightAnimationTime {0.0f};
        std::unique_ptr<Clouds> mClouds;
        std::unique_ptr<FloatingBlocks> mFloatingBlocks;
        std::vector<std::unique_ptr<MapPin>> mPins;
        MapPin* mPreviousPin {nullptr};
        std::unique_ptr<Pht::RenderableObject> mStarRenderable;
        Pht::Font mFont;
        std::unique_ptr<MapHud> mHud;
        Pht::SceneObject* mPinsContainer {nullptr};
        Pht::SceneObject* mUiViewsContainer {nullptr};
    };
}

#endif

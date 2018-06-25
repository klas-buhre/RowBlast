#ifndef MapScene_hpp
#define MapScene_hpp

#include <memory>
#include <assert.h>

// Engine includes.
#include "Font.hpp"
#include "Optional.hpp"

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
    class World;
    class MapPlace;
    class Universe;

    class MapScene {
    public:
        MapScene(Pht::IEngine& engine,
                 const CommonResources& commonResources,
                 UserData& userData,
                 const Universe& universe);
        
        void Init();
        void Update();
        void SetCameraXPosition(float xPosition);
        float GetCameraXPosition() const;
        void SetCameraAtLevel(int levelId);
        void SetCameraBetweenLevels(int levelIdA, int levelIdB);
        const MapPin* GetPin(int id) const;
        const MapPin* GetLevelPin(int levelId) const;
        
        const std::vector<std::unique_ptr<MapPin>>& GetPins() const {
            return mPins;
        }

        Pht::SceneObject& GetAvatarContainer() {
            assert(mAvatarContainer);
            return *mAvatarContainer;
        }

        Pht::SceneObject& GetUiViewsContainer() {
            assert(mUiViewsContainer);
            return *mUiViewsContainer;
        }
        
        void SetWorldId(int worldId) {
            mWorldId = worldId;
        }
        
        void SetClickedPortalNextLevelId(int clickedPortalNextLevelId) {
            mClickedPortalNextLevelId = clickedPortalNextLevelId;
        }
        
        int GetWorldId() const {
            return mWorldId;
        }

    private:
        void CreatePins(const World& world);
        void CreatePin(Pht::SceneObject& pinContainerObject, const MapPlace& place);
        void UpdateUiLightAnimation();
        void SetCameraAtPortal(int portalNextLevelId);
        const MapPin* GetPortalPin(int portalNextLevelId) const;
        
        Pht::IEngine& mEngine;
        UserData& mUserData;
        const CommonResources& mCommonResources;
        const Universe& mUniverse;
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
        Pht::SceneObject* mAvatarContainer {nullptr};
        Pht::SceneObject* mUiViewsContainer {nullptr};
        int mWorldId {1};
        Pht::Optional<int> mClickedPortalNextLevelId;
    };
}

#endif

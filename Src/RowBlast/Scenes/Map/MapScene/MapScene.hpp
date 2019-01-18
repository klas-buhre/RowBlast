#ifndef MapScene_hpp
#define MapScene_hpp

#include <memory>
#include <assert.h>

// Engine includes.
#include "Font.hpp"
#include "Optional.hpp"

// Game includes.
#include "Clouds.hpp"
#include "Planets.hpp"
#include "FloatingBlocks.hpp"
#include "MapPin.hpp"
#include "MapHud.hpp"
#include "IGuiLightProvider.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class CameraComponent;
    class LightComponent;
    class SceneObject;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class World;
    class MapPlace;
    class Universe;
    class BackgroundLight;

    class MapScene: public IGuiLightProvider {
    public:
        MapScene(Pht::IEngine& engine,
                 const CommonResources& commonResources,
                 UserServices& userServices,
                 const Universe& universe);

        void SetGuiLightDirections(const Pht::Vec3& directionA,
                                   const Pht::Vec3& directionB) override;
        void SetDefaultGuiLightDirections() override;
        
        void Init();
        void Update();
        void SetCameraXPosition(float xPosition);
        float GetCameraXPosition() const;
        void SetCameraAtLevel(int levelId);
        void SetCameraBetweenLevels(int levelIdA, int levelIdB);
        const MapPin* GetPin(int id) const;
        const MapPin* GetLevelPin(int levelId) const;
        void SaveCameraXPosition();
        
        const std::vector<std::unique_ptr<MapPin>>& GetPins() const {
            return mPins;
        }

        Pht::SceneObject& GetUfoContainer() {
            assert(mUfoContainer);
            return *mUfoContainer;
        }
        
        Pht::SceneObject& GetTutorialContainer() {
            assert(mTutorialContainer);
            return *mTutorialContainer;
        }

        Pht::SceneObject& GetUiViewsContainer() {
            assert(mUiViewsContainer);
            return *mUiViewsContainer;
        }
        
        Pht::Scene& GetScene() {
            assert(mScene);
            return *mScene;
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
        void CreateWorld(const World& world, const BackgroundLight& backgroundLight);
        void CreatePins(const World& world);
        void CreatePin(Pht::SceneObject& pinContainerObject, const MapPlace& place);
        void CreateEffects(const World& world, const BackgroundLight& backgroundLight);
        void UpdateUiLightAnimation();
        void SetCameraAtPortal(int portalNextLevelId);
        const MapPin* GetPortalPin(int portalNextLevelId) const;
        
        Pht::IEngine& mEngine;
        UserServices& mUserServices;
        const CommonResources& mCommonResources;
        const Universe& mUniverse;
        Pht::Scene* mScene {nullptr};
        Pht::CameraComponent* mCamera {nullptr};
        Pht::LightComponent* mUiLight {nullptr};
        Pht::Vec3 mUiLightDirectionA;
        Pht::Vec3 mUiLightDirectionB;
        float mLightAnimationTime {0.0f};
        std::unique_ptr<Planets> mPlanets;
        std::unique_ptr<Clouds> mClouds;
        std::unique_ptr<FloatingBlocks> mFloatingBlocks;
        std::vector<std::unique_ptr<MapPin>> mPins;
        MapPin* mPreviousPin {nullptr};
        std::unique_ptr<Pht::RenderableObject> mStarRenderable;
        Pht::Font mFont;
        std::unique_ptr<MapHud> mHud;
        Pht::SceneObject* mUfoContainer {nullptr};
        Pht::SceneObject* mTutorialContainer {nullptr};
        Pht::SceneObject* mUiViewsContainer {nullptr};
        int mWorldId {1};
        Pht::Optional<int> mClickedPortalNextLevelId;
        Pht::Optional<float> mSavedCameraXPosition;
    };
}

#endif

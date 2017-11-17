#ifndef MapScene_hpp
#define MapScene_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"

// Game includes.
#include "MapPin.hpp"
#include "FloatingCubes.hpp"
#include "NextLevelParticleEffect.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    class UserData;
    
    class MapScene {
    public:
        MapScene(Pht::IEngine& engine, const CommonResources& commonResources, UserData& userData);
        
        void Reset();
        void Update();
        void SetCameraPosition(Pht::Vec3 position);
        const std::vector<FloatingCube>& GetFloatingCubes() const;
        const Pht::ParticleEffect_& GetNextLevelParticleEffect() const;
        
        const Pht::Vec3& GetCameraPosition() const {
            return mCameraPosition;
        }
        
        const Pht::SceneObject& GetBackground() const {
            return *mBackground;
        }
        
        const std::vector<std::unique_ptr<MapPin>>& GetPins() const {
            return mPins;
        }
        
        const std::vector<std::unique_ptr<Pht::SceneObject>>& GetConnections() const {
            return mConnections;
        }
        
    private:
        void CreateBackground(const Pht::Material& backgroundMaterial);
        void CreateFloatingCubes(const CommonResources& commonResources);
        void LoadStar(const CommonResources& commonResources);
        void CreatePins(const CommonResources& commonResources);
        void CreatePin(int level, const Pht::Vec3& position);
        void ReflectProgressInPinsAndConnections();
        void StartParticleEffect();
        void SetCameraAtCurrentLevel();

        Pht::IEngine& mEngine;
        UserData& mUserData;
        Pht::Vec3 mCameraPosition {0.0f, 0.0f, 0.0f};
        Pht::Vec2 mMapSize {55.0f, 53.0f};
        NextLevelParticleEffect mNextLevelParticleEffect;
        std::unique_ptr<FloatingCubes> mFloatingCubes;
        std::unique_ptr<Pht::SceneObject> mBackground;
        std::vector<std::unique_ptr<MapPin>> mPins;
        std::vector<std::unique_ptr<Pht::SceneObject>> mConnections;
        std::shared_ptr<Pht::RenderableObject> mStarRenderable;
        std::shared_ptr<Pht::RenderableObject> mBluePinRenderable;
        std::shared_ptr<Pht::RenderableObject> mBlueConnectionRenderable;
        std::shared_ptr<Pht::RenderableObject> mGrayPinRenderable;
        std::shared_ptr<Pht::RenderableObject> mGrayConnectionRenderable;
        MapPin* mPreviousPin {nullptr};
        Pht::Font mFont;
    };
}

#endif

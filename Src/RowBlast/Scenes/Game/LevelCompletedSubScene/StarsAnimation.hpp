#ifndef StarsAnimation_hpp
#define StarsAnimation_hpp

#include <memory>
#include <array>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class CameraShake;
}

namespace RowBlast {
    class GameScene;
    class CommonResources;

    class StarsAnimation {
    public:
        enum class State {
            Ongoing,
            Rotating
        };

        StarsAnimation(Pht::IEngine& engine,
                       GameScene& scene,
                       const CommonResources& commonResources,
                       Pht::CameraShake& cameraShake);

        void Init();
        void Start(int numStars);
        State Update();
        void MoveToFront();
        
    private:
        class StarAnimation {
        public:
            enum class State {
                Waiting,
                ScalingIn,
                Flashing,
                Rotating
            };

            StarAnimation(Pht::IEngine& engine,
                          const CommonResources& commonResources,
                          Pht::CameraShake& cameraShake);
            
            void Init(Pht::SceneObject& starsContainer);
            void Start(const Pht::Vec3& position, float waitTime);
            State Update(float dt);
            void MoveToFront();
            
        private:
            void CreateGlowParticleEffect(Pht::IEngine& engine);
            void UpdateInWaitingState(float dt);
            void UpdateInScalingInState(float dt);
            void UpdateInFlashingState(float dt);
            void UpdateInRotatingState(float dt);
            void SetIsFlashing(bool isFlashing);
            
            Pht::IEngine& mEngine;
            Pht::CameraShake& mCameraShake;
            const Pht::Material& mGoldStarMaterial;
            State mState {State::Waiting};
            std::unique_ptr<Pht::RenderableObject> mStarRenderable;
            std::unique_ptr<Pht::SceneObject> mStar;
            std::unique_ptr<Pht::SceneObject> mGlowEffect;
            float mElapsedTime {0.0f};
            float mWaitTime {0.0f};
            float mStarZAngle {0.0f};
        };

        Pht::IEngine& mEngine;
        GameScene& mScene;
        int mNumStars {0};
        std::array<std::unique_ptr<StarAnimation>, 3> mStarAnimations;
    };
}

#endif

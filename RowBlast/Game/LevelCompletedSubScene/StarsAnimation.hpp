#ifndef StarsAnimation_hpp
#define StarsAnimation_hpp

#include <memory>
#include <array>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
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
                       const CommonResources& commonResources);

        void Init();
        void Start(int numStars);
        State Update();
        void ShowStarShadows();
        
    private:
        class StarAnimation {
        public:
            enum class State {
                Waiting,
                ScalingIn,
                Flashing,
                RotatingAndGlowing,
                Rotating
            };

            StarAnimation(Pht::IEngine& engine,
                          Pht::RenderableObject& shadowRenderable,
                          const CommonResources& commonResources);
            
            void Init(Pht::SceneObject& starsContainer, Pht::SceneObject& shadowsContainer);
            void Start(const Pht::Vec3& position, float waitTime);
            State Update(float dt);
            void ShowShadow();
            
        private:
            void CreateGlowParticleEffect(Pht::IEngine& engine);
            void UpdateInWaitingState(float dt);
            void UpdateInScalingInState(float dt);
            void UpdateInFlashingState(float dt);
            void UpdateInRotatingAndGlowingState(float dt);
            void UpdateInRotatingState(float dt);
            void UpdateRotation(float dt);
            void SetIsFlashing(bool isFlashing);
            
            State mState {State::Waiting};
            const Pht::Material& mGoldStarMaterial;
            std::unique_ptr<Pht::RenderableObject> mStarRenderable;
            std::unique_ptr<Pht::SceneObject> mStar;
            std::unique_ptr<Pht::SceneObject> mGlowEffect;
            std::unique_ptr<Pht::SceneObject> mShadow;
            float mElapsedTime {0.0f};
            float mWaitTime {0.0f};
            float mStarZAngle {0.0f};
        };

        Pht::IEngine& mEngine;
        GameScene& mScene;
        int mNumStars {0};
        std::array<std::unique_ptr<StarAnimation>, 3> mStarAnimations;
        std::unique_ptr<Pht::RenderableObject> mShadowRenderable;
    };
}

#endif

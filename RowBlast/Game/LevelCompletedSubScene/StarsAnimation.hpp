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
                          Pht::SceneObject& parentObject,
                          Pht::RenderableObject& shadowRenderable,
                          const CommonResources& commonResources);
            
            void Reset();
            void Start(const Pht::Vec3& position, float waitTime);
            State Update(float dt);
            
        private:
            void UpdateInWaitingState(float dt);
            void UpdateInRotatingState(float dt);
            
            State mState {State::Waiting};
            std::unique_ptr<Pht::RenderableObject> mStarRenderable;
            std::unique_ptr<Pht::SceneObject> mStar;
            std::unique_ptr<Pht::SceneObject> mGlow;
            std::unique_ptr<Pht::SceneObject> mShadow;
            float mElapsedTime {0.0f};
            float mWaitTime {0.0f};
            float mStarZAngle {0.0f};
        };

        Pht::IEngine& mEngine;
        GameScene& mScene;
        int mNumStars {0};
        std::unique_ptr<Pht::SceneObject> mContainer;
        std::array<std::unique_ptr<StarAnimation>, 3> mStarAnimations;
        std::unique_ptr<Pht::RenderableObject> mShadowRenderable;
        std::unique_ptr<Pht::RenderableObject> mGlowRenderable;
    };
}

#endif

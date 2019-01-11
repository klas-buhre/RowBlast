#ifndef FireworksParticleEffect_hpp
#define FireworksParticleEffect_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class FireworksParticleEffect {
    public:
        enum class State {
            Ongoing,
            Inactive
        };

        FireworksParticleEffect(Pht::IEngine& engine);

        void Init(Pht::SceneObject& parentObject, const Pht::Vec3& effectsVolume);
        void Start();
        State Update();
        
    private:
        class Firework {
        public:
            Firework(Pht::IEngine& engine);
            
            void Init(Pht::SceneObject& parentObject,
                      const Pht::Vec3& position,
                      float waitTime,
                      const Pht::Vec4& color);
            void Update(float dt);
            
        private:
            enum class State {
                Waiting,
                Exploding,
                Inactive
            };
            
            Pht::IEngine& mEngine;
            State mState {State::Waiting};
            std::unique_ptr<Pht::SceneObject> mExplosion;
            float mElapsedTime {0.0f};
            float mWaitTime {0.0f};
        };
        
        State mState {State::Inactive};
        Pht::IEngine& mEngine;
        float mElapsedTime {0.0f};
        std::vector<std::unique_ptr<Firework>> mFireworks;
    };
}

#endif

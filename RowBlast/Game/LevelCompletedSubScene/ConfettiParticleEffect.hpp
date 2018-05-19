#ifndef ConfettiParticleEffect_hpp
#define ConfettiParticleEffect_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class ConfettiParticleEffect {
    public:
        enum class State {
            Ongoing,
            Inactive
        };

        ConfettiParticleEffect(Pht::IEngine& engine);

        void Init(Pht::SceneObject& parentObject, const Pht::Vec3& effectsVolume);
        void Start();
        State Update();
        
    private:
        State mState {State::Inactive};
        Pht::IEngine& mEngine;
        std::unique_ptr<Pht::SceneObject> mLeftEffect;
        std::unique_ptr<Pht::SceneObject> mRightEffect;
    };
}

#endif

#ifndef BlastRadiusAnimation_hpp
#define BlastRadiusAnimation_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class BlastRadiusAnimation {
    public:
        BlastRadiusAnimation(Pht::IEngine& engine, const GameScene& scene);
    
        void Start();
        void Stop();
        void SetPosition(const Pht::Vec2& position);
        void Update(float dt);
        const Pht::SceneObject* GetSceneObject() const;
        
        bool IsActive() const {
            return mState == State::Active;
        }
        
    private:
        enum class State {
            Active,
            Inactive
        };
        
        const GameScene& mScene;
        State mState {State::Inactive};
        std::unique_ptr<Pht::SceneObject> mSceneObject;
    };
}

#endif

#ifndef BlastRadiusAnimation_hpp
#define BlastRadiusAnimation_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"
#include "SceneResources.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameScene;
    
    class BlastRadiusAnimation {
    public:
        BlastRadiusAnimation(Pht::IEngine& engine, GameScene& scene);
    
        void Init();
        void Start();
        void Stop();
        void SetPosition(const Pht::Vec2& position);
        void Update(float dt);
        
        bool IsActive() const {
            return mState == State::Active;
        }
        
    private:
        enum class State {
            Active,
            Inactive
        };
        
        GameScene& mScene;
        State mState {State::Inactive};
        std::unique_ptr<Pht::SceneObject> mSceneObject;
        float mTime {0.0f};
        Pht::SceneResources mSceneResources;
    };
}

#endif

#ifndef FieldBottomGlow_hpp
#define FieldBottomGlow_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class ScrollController;

    class FieldBottomGlow {
    public:
        FieldBottomGlow(Pht::IEngine& engine,
                        GameScene& scene,
                        const ScrollController& scrollController);

        void Init();
        void Update(float dt);
    
    private:
        void UpdateInActiveState(float dt);
        void UpdateInInactiveState();
        void GoToActiveState();
        void GoToInactiveState();
        
        enum class State {
            Active,
            Inactive
        };

        GameScene& mScene;
        const ScrollController& mScrollController;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        std::unique_ptr<Pht::SceneObject> mGlowSceneObject;
        std::unique_ptr<Pht::RenderableObject> mGlowRenderable;
    };
}

#endif

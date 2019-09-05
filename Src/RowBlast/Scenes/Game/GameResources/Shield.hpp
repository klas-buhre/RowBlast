#ifndef Shield_hpp
#define Shield_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class Level;
    class ScrollController;

    class Shield {
    public:
        Shield(Pht::IEngine& engine,
               GameScene& scene,
               const ScrollController& scrollController);

        void Init(const Level& level);
        void Start();
        void StartFlash();
        void Update(float dt);
        
        static constexpr auto shieldRelativeYPosition {12};
    
    private:
        void UpdatePosition();
        void UpdateInAppearingState(float dt);
        void UpdateInPulsatingState(float dt);
        void UpdateInFlashingState(float dt);
        void GoToPulsatingState();
        
        enum class State {
            Appearing,
            Pulsating,
            Flashing,
            Inactive
        };

        GameScene& mScene;
        const ScrollController& mScrollController;
        const Level* mLevel {nullptr};
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        float mShieldRelativeYPosition {0.0f};
        std::unique_ptr<Pht::SceneObject> mShieldSceneObject;
        std::unique_ptr<Pht::RenderableObject> mShieldRenderable;
    };
}

#endif

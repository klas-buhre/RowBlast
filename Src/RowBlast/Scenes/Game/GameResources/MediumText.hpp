#ifndef MediumText_hpp
#define MediumText_hpp

#include <vector>
#include <memory>
#include <string>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class Font;
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class CommonResources;
    
    class MediumText {
    public:
        MediumText(Pht::IEngine& engine,
                   GameScene& scene,
                   const CommonResources& commonResources);
        
        void Init();
        void StartDeferredComboMessage(int numCombos);
        void StartAwesomeMessage();
        void StartFantasticMessage();
        void StartWillUndoMessage();
        void StartUndoingMessage();
        void StartNoRoomMessage();
        void OnNewMove();
        void OnNewMoveAfterUndo();
        void Update(float dt);
        bool IsAwesomeTextActive() const;
        bool IsFantasticTextActive() const;
        
    private:
        Pht::SceneObject& CreateText(const Pht::Font& font,
                                     const Pht::Vec2& position,
                                     const std::string& text);
        void CreateTwinkleParticleEffect(Pht::IEngine& engine);
        void TryStartingDeferredMessage();
        bool StartComboMessage();
        void Start(Pht::SceneObject& textSceneObject);
        void UpdateInScalingInState(float dt);
        void UpdateInDisplayingTextState(float dt);
        void UpdateInSlidingOutState(float dt);
        void HideAllTextObjects();
    
        enum class State {
            ScalingIn,
            DisplayingText,
            SlidingOut,
            Inactive
        };

        Pht::IEngine& mEngine;
        GameScene& mScene;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        float mVelocity {0.0f};
        float mTextSlide {0.0f};
        std::vector<std::unique_ptr<Pht::SceneObject>> mTextSceneObjects;
        Pht::SceneObject* mContainerSceneObject {nullptr};
        Pht::SceneObject* mActiveTextSceneObject {nullptr};
        Pht::SceneObject* mDeferredTextSceneObject {nullptr};
        Pht::SceneObject* mComboTextSceneObject {nullptr};
        Pht::SceneObject* mAwesomeTextSceneObject {nullptr};
        Pht::SceneObject* mFantasticTextSceneObject {nullptr};
        Pht::SceneObject* mUndoingTextSceneObject {nullptr};
        Pht::SceneObject* mWillUndoTextSceneObject {nullptr};
        Pht::SceneObject* mNoRoomTextSceneObject {nullptr};
        std::unique_ptr<Pht::SceneObject> mTwinkleParticleEffect;
    };
}

#endif

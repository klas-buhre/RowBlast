#ifndef ComboTextAnimation_hpp
#define ComboTextAnimation_hpp

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
    
    class ComboTextAnimation {
    public:
        ComboTextAnimation(Pht::IEngine& engine,
                           GameScene& scene,
                           const CommonResources& commonResources);
        
        void Init();
        void StartComboMessage(int numCombos);
        void StartAwesomeMessage();
        void StartFantasticMessage();
        void Update(float dt);
        
    private:
        Pht::SceneObject& CreateText(const Pht::Font& font,
                                     const Pht::Vec2& position,
                                     const std::string& text);
        void CreateTwinkleParticleEffect(Pht::IEngine& engine);
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

        GameScene& mScene;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        float mVelocity {0.0f};
        float mTextSlide {0.0f};
        std::vector<std::unique_ptr<Pht::SceneObject>> mTextSceneObjects;
        Pht::SceneObject* mContainerSceneObject {nullptr};
        Pht::SceneObject* mActiveTextSceneObject {nullptr};
        Pht::SceneObject* mComboTextSceneObject {nullptr};
        Pht::SceneObject* mAwesomeTextSceneObject {nullptr};
        Pht::SceneObject* mFantasticTextSceneObject {nullptr};
        std::unique_ptr<Pht::SceneObject> mTwinkleParticleEffect;
    };
}

#endif

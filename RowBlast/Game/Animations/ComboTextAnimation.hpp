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
        enum class Message {
            Combo,
            Awesome
        };
        
        ComboTextAnimation(Pht::IEngine& engine,
                           GameScene& scene,
                           const CommonResources& commonResources);
        
        void Init();
        void Start(Message message);
        void Update(float dt);
        
    private:
        void CreateText(const Pht::Font& font, const Pht::Vec2& position, const std::string& text);
        void CreateTwinkleParticleEffect(Pht::IEngine& engine);
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
        std::unique_ptr<Pht::SceneObject> mTwinkleParticleEffect;
    };
}

#endif

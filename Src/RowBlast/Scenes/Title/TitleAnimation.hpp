#ifndef TitleAnimation_hpp
#define TitleAnimation_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"

namespace Pht {
    class IEngine;
    class Scene;
}

namespace RowBlast {
    class TitleAnimation {
    public:
        TitleAnimation(Pht::IEngine& engine);
        
        void Init(Pht::Scene& scene, Pht::SceneObject& parentObject);
        void Update();
        bool IsDone() const;

    private:
        struct TextLine {
            Pht::Vec3 mTextPosition;
            std::unique_ptr<Pht::SceneObject> mTextSceneObject;
            std::unique_ptr<Pht::SceneObject> mFlareEffect;
        };
        
        void CreateText();
        TextLine CreateTextLine(const std::string& text,
                                const Pht::Vec3& textPosition,
                                Pht::TextProperties textProperties);
        std::unique_ptr<Pht::SceneObject> CreateFlareParticleEffect(float yPosition);
        void CreateTwinkleParticleEffect();
        void UpdateInWaitingState();
        void UpdateInSlidingInState();
        void UpdateTextLineSceneObjectPositions();
        void UpdateInSlidingSlowlyState();
        void UpdateInTwinklesState();
    
        enum class State {
            Waiting,
            SlidingIn,
            SlidingSlowly,
            Twinkles
        };

        Pht::IEngine& mEngine;
        State mState {State::Waiting};
        Pht::Font mFont;
        float mElapsedTime {0.0f};
        std::unique_ptr<Pht::SceneObject> mTwinkleParticleEffect;
        TextLine mUpperTextLine;
        TextLine mLowerTextLine;
        Pht::Vec3 mLeftPosition;
        Pht::Vec3 mRightPosition;
        Pht::Vec3 mTextPosition;
        float mVelocity {0.0f};
        float mInitialVelocity {0.0f};
        float mDisplayVelocity {0.0f};
    };
}

#endif

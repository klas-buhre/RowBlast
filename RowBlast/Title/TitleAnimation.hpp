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
        TitleAnimation(Pht::IEngine& engine, Pht::Scene& scene, Pht::SceneObject& parentObject);
        
        void Update();
        bool IsDone() const;

    private:
        struct Text {
            Pht::Vec3 mUpperTextLinePosition;
            std::unique_ptr<Pht::SceneObject> mUpperTextLineSceneObject;
            Pht::Vec3 mLowerTextLinePosition;
            std::unique_ptr<Pht::SceneObject> mLowerTextLineSceneObject;
        };
        
        void CreateText();
        void CreateTwinkleParticleEffect(Pht::IEngine& engine);
        void CreateGradientRectangles(Pht::Scene& scene, Pht::SceneObject& containerSceneObject);
        void UpdateInWaitingState();
        void UpdateInRectangleAppearingState();
        void UpdateInSlidingInState();
        void UpdateTextLineSceneObjectPositions();
        void UpdateInSlidingSlowlyState();
    
        enum class State {
            Waiting,
            RectangleAppearing,
            SlidingIn,
            SlidingSlowly,
            Inactive
        };

        Pht::IEngine& mEngine;
        State mState {State::Waiting};
        Pht::Font mFont;
        float mElapsedTime {0.0f};
        Pht::SceneObject* mGradientRectanglesSceneObject {nullptr};
        std::unique_ptr<Pht::SceneObject> mTwinkleParticleEffect;
        Text mText;
        Pht::Vec3 mLeftPosition;
        Pht::Vec3 mRightPosition;
        Pht::Vec3 mTextPosition;
        float mVelocity {0.0f};
        float mInitialVelocity {0.0f};
        float mDisplayVelocity {0.0f};
    };
}

#endif

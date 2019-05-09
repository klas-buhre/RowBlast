#ifndef SlidingTextAnimation_hpp
#define SlidingTextAnimation_hpp

#include <vector>
#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"
#include "SceneResources.hpp"

// Game includes.
#include "Ufo.hpp"
#include "UfoAnimation.hpp"

namespace Pht {
    class IEngine;
    class Font;
    class Animation;
}

namespace RowBlast {
    class GameScene;
    class CommonResources;
    
    class SlidingTextAnimation {
    public:
        enum class State {
            RectangleAppearing,
            SlidingIn,
            DisplayingText,
            SlidingOut,
            RectangleDisappearing,
            Inactive
        };
        
        enum class Message {
            ClearBlocks,
            BlocksCleared,
            FillSlots,
            SlotsFilled,
            BringDownTheAsteroid,
            TheAsteroidIsDown,
            OutOfMoves
        };
        
        SlidingTextAnimation(Pht::IEngine& engine,
                             GameScene& scene,
                             const CommonResources& commonResources);
        
        void Init();
        void Start(Message message);
        State Update();
        
    private:
        struct TextLine {
            Pht::Vec2 mPosition;
            const std::string mText;
        };

        struct Text {
            float mDisplayTime {1.0f};
            bool mIsUfoVisible {true};
            Pht::Vec3 mUpperTextLinePosition;
            std::unique_ptr<Pht::SceneObject> mUpperTextLineSceneObject;
            Pht::Vec3 mLowerTextLinePosition;
            std::unique_ptr<Pht::SceneObject> mLowerTextLineSceneObject;
        };
        
        void CreateText(const Pht::Font& font,
                        float displayTime,
                        bool isUfoVisible,
                        const TextLine& upperTextLine,
                        const TextLine& lowerTextLine);
        void CreateClearObjectiveContainer(const CommonResources& commonResources,
                                           const Pht::Font& font);
        void CreateTwinkleParticleEffect();
        void CreateGradientRectangles(Pht::SceneObject& containerSceneObject);
        void UpdateInRectangleAppearingState();
        void UpdateInSlidingInState();
        void UpdateTextLineSceneObjectPositions();
        void UpdateInDisplayingTextState();
        void UpdateInSlidingOutState();
        void UpdateInRectangleDisappearingState();
        void UpdateUfo();
        void FlyInUfo();
        void FlyOutUfo();
        void UpdatePhtAnimation();
        
        enum class UfoState {
            FlyingIn,
            Hovering,
            FlyingOut,
            Inactive
        };
    
        Pht::IEngine& mEngine;
        GameScene& mScene;
        Ufo mUfo;
        UfoAnimation mUfoAnimation;
        UfoState mUfoState {UfoState::Inactive};
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        const Text* mText {nullptr};
        Pht::SceneObject* mContainerSceneObject {nullptr};
        Pht::SceneObject* mGradientRectanglesSceneObject {nullptr};
        std::unique_ptr<Pht::SceneObject> mTwinkleParticleEffect;
        std::unique_ptr<Pht::SceneObject> mClearObjectiveContainer;
        Pht::Animation* mGreyCubeAnimation {nullptr};
        Pht::Animation* mNumObjectsTextAnimation {nullptr};
        std::vector<Text> mTexts;
        Pht::Vec3 mLeftPosition;
        Pht::Vec3 mRightPosition;
        Pht::Vec3 mTextPosition;
        float mVelocity {0.0f};
        float mInitialVelocity {0.0f};
        float mDisplayVelocity {0.0f};
        Pht::SceneResources mSceneResources;
    };
}

#endif

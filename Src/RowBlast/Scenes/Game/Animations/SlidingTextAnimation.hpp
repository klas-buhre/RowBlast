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
    class TextProperties;
    class TextComponent;
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
        
        SlidingTextAnimation(Pht::IEngine& engine,
                             GameScene& scene,
                             const CommonResources& commonResources);
        
        void Init();
        void StartClearBlocksMessage(int numBlocks);
        void StartBlocksClearedMessage();
        void StartFillSlotsMessage(int numSlots);
        void StartSlotsFilledMessage();
        void StartBringDownTheAsteroidMessage();
        void StartTheAsteroidIsDownMessage();
        void StartOutOfMovesMessage();
        State Update();
        
    private:
        struct TextLineConfig {
            Pht::Vec2 mPosition;
            const std::string mText;
            Pht::Vec2 mTwinkleRelativePosition;
        };
        
        struct ExtraAnimations {
            bool mUfo {true};
            bool mGrayCube {false};
            bool mNumObjects {false};
            bool mCheckMark {false};
        };
        
        struct TextLine {
            Pht::Vec3 mPosition;
            std::unique_ptr<Pht::SceneObject> mSceneObject;
            Pht::Vec3 mTwinkleRelativePosition;
        };

        struct TextMessage {
            TextLine mUpperTextLine;
            TextLine mLowerTextLine;
            ExtraAnimations mExtraAnimations;
            float mDisplayTime {1.0f};
        };
        
        TextMessage& CreateText(const Pht::Font& font,
                                float displayTime,
                                const TextLineConfig& upperTextLineConfig,
                                const TextLineConfig& lowerTextLineConfig,
                                const ExtraAnimations& extraAnimations);
        TextLine CreateTextLine(const TextLineConfig& textLineConfig,
                                const Pht::TextProperties textProperties);
        void CreateExtraAnimationsContainer(const CommonResources& commonResources,
                                            const Pht::Font& font);
        void CreateGreyCubeAnimation(const CommonResources& commonResources);
        void CreateNumObjectsTextAnimation(const Pht::Font& font);
        void CreateCheckMarkAnimation();
        void CreateTwinkleParticleEffects();
        void CreateGradientRectangles(Pht::SceneObject& containerSceneObject);
        void Start(const TextMessage& textMessage);
        void UpdateInRectangleAppearingState();
        void UpdateInSlidingInState();
        void UpdateTextLineSceneObjectPositions();
        void UpdateInDisplayingTextState();
        void UpdateInSlidingOutState();
        void UpdateInRectangleDisappearingState();
        void UpdateUfo();
        void FlyInUfo();
        void FlyOutUfo();
        void StartPlayingExtraAnimations();
        void UpdateExtraAnimations();
        void StartScalingDownExtraAnimations();
        
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
        const TextMessage* mTextMessage {nullptr};
        Pht::SceneObject* mContainerSceneObject {nullptr};
        Pht::SceneObject* mGradientRectanglesSceneObject {nullptr};
        std::unique_ptr<Pht::SceneObject> mUpperTwinkleParticleEffect;
        std::unique_ptr<Pht::SceneObject> mLowerTwinkleParticleEffect;
        std::unique_ptr<Pht::SceneObject> mExtraAnimationsContainer;
        Pht::Animation* mGreyCubeAnimation {nullptr};
        Pht::Animation* mNumObjectsTextAnimation {nullptr};
        Pht::TextComponent* mNumObjectsText {nullptr};
        Pht::Animation* mCheckMarkAnimation {nullptr};
        std::vector<TextMessage> mTextMessages;
        Pht::Vec3 mLeftPosition;
        Pht::Vec3 mRightPosition;
        Pht::Vec3 mTextPosition;
        float mVelocity {0.0f};
        float mInitialVelocity {0.0f};
        float mDisplayVelocity {0.0f};
        TextMessage* mClearBlocksMessage;
        TextMessage* mBlocksClearedMessage;
        TextMessage* mFillSlotsMessage;
        TextMessage* mSlotsFilledMessage;
        TextMessage* mBringDownTheAsteroidMessage;
        TextMessage* mTheAsteroidIsDownMessage;
        TextMessage* mOutOfMovesMessage;
        Pht::SceneResources mSceneResources;
    };
}

#endif

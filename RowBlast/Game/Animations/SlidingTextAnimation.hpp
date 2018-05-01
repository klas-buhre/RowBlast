#ifndef SlidingTextAnimation_hpp
#define SlidingTextAnimation_hpp

#include <vector>
#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class Font;
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
            SlotsFilled
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
            Pht::Vec3 mUpperTextLinePosition;
            std::unique_ptr<Pht::SceneObject> mUpperTextLineSceneObject;
            Pht::Vec3 mLowerTextLinePosition;
            std::unique_ptr<Pht::SceneObject> mLowerTextLineSceneObject;
        };
        
        void CreateText(const Pht::Font& font,
                        float displayTime,
                        const TextLine& upperTextLine,
                        const TextLine& lowerTextLine);
        void CreateGradientRectangles(Pht::SceneObject& containerSceneObject);
        void UpdateInRectangleAppearingState();
        void UpdateInSlidingInState();
        void UpdateTextLineSceneObjectPositions();
        void UpdateInDisplayingTextState();
        void UpdateInSlidingOutState();
        void UpdateInRectangleDisappearingState();
    
        Pht::IEngine& mEngine;
        GameScene& mScene;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        const Text* mText {nullptr};
        Pht::SceneObject* mGradientRectanglesSceneObject {nullptr};
        std::vector<Text> mTexts;
        Pht::Vec3 mLeftPosition;
        Pht::Vec3 mRightPosition;
        Pht::Vec3 mTextPosition;
        float mVelocity {0.0f};
        float mInitialVelocity {0.0f};
        float mDisplayVelocity {0.0f};
    };
}

#endif

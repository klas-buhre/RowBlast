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
            SlidingIn,
            SlidingOut,
            DisplayingText,
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
            std::unique_ptr<Pht::SceneObject> mSceneObject;
            std::vector<std::unique_ptr<Pht::SceneObject>> mTextLines;
        };
        
        void CreateText(const Pht::Font& font,
                        float displayTime,
                        const std::vector<TextLine>& textLines);
        void SetAlpha(float newAlhpa);
        void StartSlideOut();
        void UpdateInSlidingInState();
        void UpdateInDisplayingTextState();
        void UpdateInSlidingOutState();
    
        Pht::IEngine& mEngine;
        GameScene& mScene;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        const Text* mText {nullptr};
        std::unique_ptr<Pht::SceneObject> mContainerSceneObject;
        std::vector<Text> mTexts;
        Pht::Vec3 mSlideInStartPosition;
        Pht::Vec3 mSlideOutFinalPosition;
    };
}

#endif

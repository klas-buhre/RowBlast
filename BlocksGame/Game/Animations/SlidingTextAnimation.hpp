#ifndef SlidingTextAnimation_hpp
#define SlidingTextAnimation_hpp

#include <vector>

// Engine includes.
#include "Font.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class SlidingTextAnimation {
    public:
        enum class State {
            SlidingIn,
            SlidingOut,
            DisplayingText,
            Inactive
        };
        
        struct TextLine {
            Pht::Vec2 mPosition;
            const std::string mText;
        };
        
        struct Text {
            float mDisplayTime {1.0f};
            std::vector<TextLine> mTextLines;
        };
        
        explicit SlidingTextAnimation(Pht::IEngine& engine);
        
        void Start(const Text& text);
        State Update();
        
        State GetState() const {
            return mState;
        }
        
        const Text* GetText() const {
            return mText;
        }
        
        const Pht::Vec2& GetPosition() const {
            return mPosition;
        }
        
        const Pht::TextProperties& GetTextProperties() const {
            return mTextProperties;
        }
        
        static const Text mClearBlocks;
        static const Text mBlocksCleared;
        static const Text mFillSlots;
        static const Text mSlotsFilled;
        
    private:
        void StartSlideOut();
        void UpdateInSlidingInState();
        void UpdateInDisplayingTextState();
        void UpdateInSlidingOutState();
    
        Pht::IEngine& mEngine;
        Pht::Font mFont;
        Pht::TextProperties mTextProperties;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        const Text* mText {nullptr};
        Pht::Vec2 mPosition;
        Pht::Vec2 mVelocity;
        Pht::Vec2 mAcceleration;
        Pht::Vec2 mSlideInStartPosition;
        Pht::Vec2 mSlideOutFinalPosition;
    };
}

#endif

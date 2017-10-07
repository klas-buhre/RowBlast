#ifndef NoLivesDialogController_hpp
#define NoLivesDialogController_hpp

// Game includes.
#include "NoLivesDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace BlocksGame {
    class CommonResources;
    class UserData;
    
    class NoLivesDialogController {
    public:
        enum class Result {
            None,
            RefillLives,
            Close
        };
        
        NoLivesDialogController(Pht::IEngine& engine,
                                const CommonResources& commonResources,
                                const UserData& userData);
        
        void Reset(bool shouldSlideOut);
        Result Update();
        
        const Pht::GuiView& GetView() const {
            return mView;
        }
        
        const Pht::FadeEffect& GetFadeEffect() const {
            return mSlidingMenuAnimation.GetFadeEffect();
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        const UserData& mUserData;
        NoLivesDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        bool mShouldSlideOut {true};
        Result mDeferredResult {Result::None};
    };
}

#endif

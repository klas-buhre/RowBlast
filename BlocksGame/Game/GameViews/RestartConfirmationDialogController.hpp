#ifndef RestartConfirmationDialogController_hpp
#define RestartConfirmationDialogController_hpp

// Game includes.
#include "RestartConfirmationDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace BlocksGame {
    class CommonResources;
    
    class RestartConfirmationDialogController {
    public:
        enum class Result {
            None,
            RestartGame,
            DoNotRestartGame
        };
        
        RestartConfirmationDialogController(Pht::IEngine& engine,
                                            const CommonResources& commonResources);
        
        void Reset();
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
        RestartConfirmationDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

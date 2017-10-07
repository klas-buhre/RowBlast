#ifndef NoMovesDialogController_hpp
#define NoMovesDialogController_hpp

// Game includes.
#include "NoMovesDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace BlocksGame {
    class CommonResources;
    
    class NoMovesDialogController {
    public:
        enum class Result {
            None,
            PlayOn,
            BackToMap
        };
        
        NoMovesDialogController(Pht::IEngine& engine, const CommonResources& commonResources);
        
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
        NoMovesDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

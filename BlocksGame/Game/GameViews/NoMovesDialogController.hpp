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
        
        void Init();
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();

        Pht::GuiView& GetView() {
            return mView;
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

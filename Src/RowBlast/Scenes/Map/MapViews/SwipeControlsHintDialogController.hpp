#ifndef SwipeControlsHintDialogController_hpp
#define SwipeControlsHintDialogController_hpp

// Game includes.
#include "SwipeControlsHintDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class SwipeControlsHintDialogController {
    public:
        enum class Result {
            None,
            Close,
            HowToPlay
        };
        
        SwipeControlsHintDialogController(Pht::IEngine& engine,
                                          const CommonResources& commonResources);
        
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        void SetUp();
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }
        
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        SwipeControlsHintDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

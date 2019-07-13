#ifndef NoMoreLevelsDialogController_hpp
#define NoMoreLevelsDialogController_hpp

// Game includes.
#include "NoMoreLevelsDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class NoMoreLevelsDialogController {
    public:
        enum class Result {
            None,
            Close
        };
        
        NoMoreLevelsDialogController(Pht::IEngine& engine,
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
        NoMoreLevelsDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

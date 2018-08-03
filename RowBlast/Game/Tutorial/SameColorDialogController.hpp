#ifndef SameColorDialogController_hpp
#define SameColorDialogController_hpp

// Game includes.
#include "SameColorDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    
    class SameColorDialogController {
    public:
        enum class Result {
            None,
            Play
        };
        
        SameColorDialogController(Pht::IEngine& engine, const CommonResources& commonResources);
        
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
        SameColorDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

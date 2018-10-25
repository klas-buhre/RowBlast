#ifndef CascadingDialogController_hpp
#define CascadingDialogController_hpp

// Game includes.
#include "CascadingDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    
    class CascadingDialogController {
    public:
        enum class Result {
            None,
            Play
        };
        
        CascadingDialogController(Pht::IEngine& engine, const CommonResources& commonResources);
        
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
        CascadingDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

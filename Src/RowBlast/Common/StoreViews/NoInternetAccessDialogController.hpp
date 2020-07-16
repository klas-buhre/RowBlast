#ifndef NoInternetAccessDialogController_hpp
#define NoInternetAccessDialogController_hpp

// Game includes.
#include "NoInternetAccessDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class NoInternetAccessDialogController {
    public:
        enum class Result {
            None,
            Close
        };
        
        NoInternetAccessDialogController(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void SetUp(SlidingMenuAnimation::UpdateFade updateFadeOnShow,
                   SlidingMenuAnimation::UpdateFade updateFadeOnClose);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        NoInternetAccessDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        SlidingMenuAnimation::UpdateFade mUpdateFadeOnClose {SlidingMenuAnimation::UpdateFade::No};
        Result mDeferredResult {Result::None};
    };
}

#endif

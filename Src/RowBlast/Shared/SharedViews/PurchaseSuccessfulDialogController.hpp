#ifndef PurchaseSuccessfulDialogController_hpp
#define PurchaseSuccessfulDialogController_hpp

// Game includes.
#include "PurchaseSuccessfulDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class PurchaseSuccessfulDialogController {
    public:
        enum class Result {
            None,
            Close
        };
        
        enum class ShouldSlideOut {
            Yes,
            No
        };
        
        PurchaseSuccessfulDialogController(Pht::IEngine& engine,
                                           const CommonResources& commonResources,
                                           PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void SetUp(int numCoins,
                   ShouldSlideOut slideOutOnClose,
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
        PurchaseSuccessfulDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
        ShouldSlideOut mSlideOutOnClose;
        SlidingMenuAnimation::UpdateFade mUpdateFadeOnClose;
    };
}

#endif

#ifndef StoreMenuController_hpp
#define StoreMenuController_hpp

// Game includes.
#include "StoreMenuView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
    class FadeEffect;
}

namespace RowBlast {
    class StoreMenuController {
    public:
        enum class Result {
            None,
            Close,
            PurchaseCoins
        };
        
        StoreMenuController(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void SetUp(SlidingMenuAnimation::UpdateFade updateFade,
                   SlidingMenuAnimation::SlideDirection slideDirection);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IEngine& mEngine;
        StoreMenuView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

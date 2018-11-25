#ifndef PurchaseCanceledDialogController_hpp
#define PurchaseCanceledDialogController_hpp

// Game includes.
#include "PurchaseCanceledDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class PurchaseCanceledDialogController {
    public:
        enum class Result {
            None,
            Close
        };
        
        PurchaseCanceledDialogController(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void SetUp();
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        PurchaseCanceledDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

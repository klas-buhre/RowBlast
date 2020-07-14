#ifndef PurchaseFailedDialogController_hpp
#define PurchaseFailedDialogController_hpp

// Game includes.
#include "PurchaseFailedDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class PurchaseFailedDialogController {
    public:
        enum class Result {
            None,
            Close
        };
        
        PurchaseFailedDialogController(Pht::IEngine& engine,
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
        PurchaseFailedDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

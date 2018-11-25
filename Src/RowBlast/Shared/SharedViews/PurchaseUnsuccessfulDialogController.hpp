#ifndef PurchaseUnsuccessfulDialogController_hpp
#define PurchaseUnsuccessfulDialogController_hpp

// Game includes.
#include "PurchaseUnsuccessfulDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class PurchaseUnsuccessfulDialogController {
    public:
        enum class Result {
            None,
            Close
        };
        
        PurchaseUnsuccessfulDialogController(Pht::IEngine& engine,
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
        PurchaseUnsuccessfulDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

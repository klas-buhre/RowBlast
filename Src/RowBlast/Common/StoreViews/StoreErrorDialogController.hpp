#ifndef StoreErrorDialogController_hpp
#define StoreErrorDialogController_hpp

// Game includes.
#include "StoreErrorDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class StoreErrorDialogController {
    public:
        enum class Result {
            None,
            Close
        };
        
        StoreErrorDialogController(Pht::IEngine& engine,
                                   const CommonResources& commonResources,
                                   const std::string& caption,
                                   const std::vector<std::string>& textLines,
                                   PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void SetUp(SlidingMenuAnimation::UpdateFade updateFade);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        StoreErrorDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        SlidingMenuAnimation::UpdateFade mUpdateFade {SlidingMenuAnimation::UpdateFade::No};
        Result mDeferredResult {Result::None};
    };
}

#endif

#ifndef MapConfirmationDialogController_hpp
#define MapConfirmationDialogController_hpp

// Game includes.
#include "MapConfirmationDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace BlocksGame {
    class CommonResources;
    
    class MapConfirmationDialogController {
    public:
        enum class Result {
            None,
            GoToMap,
            DoNotGoToMap
        };
        
        MapConfirmationDialogController(Pht::IEngine& engine,
                                        const CommonResources& commonResources);
        
        void Reset();
        Result Update();
        
        const Pht::GuiView& GetView() const {
            return mView;
        }
        
        const Pht::FadeEffect& GetFadeEffect() const {
            return mSlidingMenuAnimation.GetFadeEffect();
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        MapConfirmationDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

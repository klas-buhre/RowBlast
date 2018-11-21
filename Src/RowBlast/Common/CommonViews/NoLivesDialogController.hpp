#ifndef NoLivesDialogController_hpp
#define NoLivesDialogController_hpp

// Game includes.
#include "NoLivesDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
    class FadeEffect;
}

namespace RowBlast {
    class UserServices;
    
    class NoLivesDialogController {
    public:
        enum class Result {
            None,
            RefillLives,
            Close
        };
        
        NoLivesDialogController(Pht::IEngine& engine,
                                const CommonResources& commonResources,
                                const UserServices& userServices,
                                PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void SetUp(bool shouldSlideOut);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }
        
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        const UserServices& mUserServices;
        NoLivesDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        bool mShouldSlideOut {true};
        Result mDeferredResult {Result::None};
    };
}

#endif

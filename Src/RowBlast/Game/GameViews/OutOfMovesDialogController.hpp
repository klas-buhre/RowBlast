#ifndef OutOfMovesDialogController_hpp
#define OutOfMovesDialogController_hpp

// Game includes.
#include "OutOfMovesDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    
    class OutOfMovesDialogController {
    public:
        enum class Result {
            None,
            PlayOn,
            BackToMap
        };
        
        OutOfMovesDialogController(Pht::IEngine& engine,
                                   const CommonResources& commonResources,
                                   const UserServices& userServices);
        
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        void SetUp(SlidingMenuAnimation::SlideDirection slideDirection,
                   SlidingMenuAnimation::UpdateFade updateFade);
        Result Update();

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        const UserServices& mUserServices;
        OutOfMovesDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

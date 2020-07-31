#ifndef OutOfMovesRetryDialogController_hpp
#define OutOfMovesRetryDialogController_hpp

// Game includes.
#include "OutOfMovesRetryDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    
    class OutOfMovesRetryDialogController {
    public:
        enum class Result {
            None,
            Retry,
            BackToMap
        };
        
        OutOfMovesRetryDialogController(Pht::IEngine& engine,
                                        const CommonResources& commonResources,
                                        const UserServices& userServices);
        
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider);
        void SetUp(GameScene& scene,
                   SlidingMenuAnimation::SlideDirection slideDirection,
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
        OutOfMovesRetryDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

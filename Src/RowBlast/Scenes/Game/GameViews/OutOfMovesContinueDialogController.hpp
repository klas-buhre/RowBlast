#ifndef OutOfMovesContinueDialogController_hpp
#define OutOfMovesContinueDialogController_hpp

// Game includes.
#include "OutOfMovesContinueDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    
    class OutOfMovesContinueDialogController {
    public:
        enum class Result {
            None,
            PlayOn,
            BackToMap
        };
        
        OutOfMovesContinueDialogController(Pht::IEngine& engine,
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
        OutOfMovesContinueDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

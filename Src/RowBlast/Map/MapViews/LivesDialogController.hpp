#ifndef LivesDialogController_hpp
#define LivesDialogController_hpp

// Game includes.
#include "LivesDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
    class FadeEffect;
}

namespace RowBlast {
    class UserServices;
    
    class LivesDialogController {
    public:
        enum class Result {
            None,
            Close
        };
        
        LivesDialogController(Pht::IEngine& engine,
                              const CommonResources& commonResources,
                              const UserServices& userServices,
                              IGuiLightProvider& guiLightProvider);
        
        void SetUp();
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }
        
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        LivesDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

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
        
        enum class ShouldSlideOut {
            Yes,
            No
        };
        
        NoLivesDialogController(Pht::IEngine& engine,
                                const CommonResources& commonResources,
                                const UserServices& userServices,
                                PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        void SetUp(ShouldSlideOut shouldSlideOutOnClose,
                   ShouldSlideOut shouldSlideOutOnRefillLives);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }
        
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        Result ReturnResult(Result result);
        void SetDeferredResult(Result result);
        
        Pht::IInput& mInput;
        const UserServices& mUserServices;
        NoLivesDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        ShouldSlideOut mShouldSlideOutOnClose;
        ShouldSlideOut mShouldSlideOutOnRefillLives;
        Result mDeferredResult {Result::None};
        bool mHasResult {false};
    };
}

#endif

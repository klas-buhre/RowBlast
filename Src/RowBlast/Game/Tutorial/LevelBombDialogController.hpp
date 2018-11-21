#ifndef LevelBombDialogController_hpp
#define LevelBombDialogController_hpp

// Game includes.
#include "LevelBombDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    
    class LevelBombDialogController {
    public:
        enum class Result {
            None,
            Play
        };
        
        LevelBombDialogController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void SetUp(Pht::Scene& scene);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        LevelBombDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

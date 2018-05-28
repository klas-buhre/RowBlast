#ifndef LevelStartDialogController_hpp
#define LevelStartDialogController_hpp

// Game includes.
#include "LevelStartDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    
    class LevelStartDialogController {
    public:
        enum class Result {
            None,
            Play,
            Close
        };
        
        LevelStartDialogController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Init(const LevelInfo& levelInfo);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        LevelStartDialogView& GetView() {
            return mView;
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        LevelStartDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

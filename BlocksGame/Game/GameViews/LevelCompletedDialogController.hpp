#ifndef LevelCompletedDialogController_hpp
#define LevelCompletedDialogController_hpp

// Game includes.
#include "LevelCompletedDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
    class IAudio;
}

namespace RowBlast {
    class CommonResources;
    
    class LevelCompletedDialogController {
    public:
        enum class Result {
            None,
            NextLevel,
            BackToMap
        };
        
        LevelCompletedDialogController(Pht::IEngine& engine,
                                       const CommonResources& commonResources);
        
        void Init();
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        LevelCompletedDialogView& GetView() {
            return mView;
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        LevelCompletedDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
    };
}

#endif

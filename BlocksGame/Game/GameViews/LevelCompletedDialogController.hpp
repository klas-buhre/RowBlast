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

namespace BlocksGame {
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
        Result Update();
        
        const LevelCompletedDialogView& GetView() const {
            return mView;
        }

        LevelCompletedDialogView& GetView() {
            return mView;
        }
        
        const Pht::FadeEffect& GetFadeEffect() const {
            return mSlidingMenuAnimation.GetFadeEffect();
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

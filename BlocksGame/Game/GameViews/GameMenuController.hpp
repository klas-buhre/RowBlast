#ifndef GameMenuController_hpp
#define GameMenuController_hpp

// Game includes.
#include "GameMenuView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace BlocksGame {
    class CommonResources;
    class GameLogic;
    
    class GameMenuController {
    public:
        enum class Result {
            None,
            ResumeGame,
            UndoMove,
            RestartGame,
            GoToSettingsMenu,
            BackToMap
        };
        
        GameMenuController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Reset(SlidingMenuAnimation::UpdateFade updateFade, bool isUndoMovePossible);
        Result Update();
        
        const Pht::GuiView& GetView() const {
            return mView;
        }
        
        const Pht::FadeEffect& GetFadeEffect() const {
            return mSlidingMenuAnimation.GetFadeEffect();
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        GameMenuView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif

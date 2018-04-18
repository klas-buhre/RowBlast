#ifndef GameMenuController_hpp
#define GameMenuController_hpp

// Game includes.
#include "GameMenuView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
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
        
        void Init(SlidingMenuAnimation::UpdateFade updateFade, bool isUndoMovePossible);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
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

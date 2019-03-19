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
            ResumeGameAfterUndo,
            GoToHowToPlayDialog,
            GoToLevelInfoDialog,
            GoToSettingsMenu,
            RestartGame,
            BackToMap
        };
        
        GameMenuController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void SetUp(SlidingMenuAnimation::UpdateFade updateFade,
                   SlidingMenuAnimation::SlideDirection slideDirection,
                   bool isUndoMovePossible);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider);
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

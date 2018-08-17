#ifndef LevelGoalDialogController_hpp
#define LevelGoalDialogController_hpp

// Game includes.
#include "LevelGoalDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    class PieceResources;
    
    class LevelGoalDialogController {
    public:
        enum class Result {
            None,
            Play,
            Close
        };
        
        LevelGoalDialogController(Pht::IEngine& engine,
                                  const CommonResources& commonResources,
                                  const PieceResources& pieceResources,
                                  LevelGoalDialogView::Scene scene);
        
        void Init(const LevelInfo& levelInfo);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        LevelGoalDialogView& GetView() {
            return mView;
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        Result OnTouchInMapScene(const Pht::TouchEvent& touchEvent);
        Result OnTouchInGameScene(const Pht::TouchEvent& touchEvent);

        Pht::IInput& mInput;
        LevelGoalDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
        LevelGoalDialogView::Scene mScene {LevelGoalDialogView::Scene::Map};
    };
}

#endif

#ifndef LevelGoalDialogController_hpp
#define LevelGoalDialogController_hpp

// Game includes.
#include "LevelGoalDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
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
                                  LevelGoalDialogView::SceneId sceneId);
        
        void SetUp(const LevelInfo& levelInfo);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider);
        Result Update();
        
        LevelGoalDialogView& GetView() {
            return mView;
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        Result OnTouchInMapScene(const Pht::TouchEvent& touchEvent);
        Result OnTouchInGameScene(const Pht::TouchEvent& touchEvent);

        Pht::IEngine& mEngine;
        LevelGoalDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
        LevelGoalDialogView::SceneId mSceneId {LevelGoalDialogView::SceneId::Map};
    };
}

#endif

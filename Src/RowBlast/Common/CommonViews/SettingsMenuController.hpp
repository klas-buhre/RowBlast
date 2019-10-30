#ifndef SettingsMenuController_hpp
#define SettingsMenuController_hpp

// Game includes.
#include "SettingsMenuView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
    class FadeEffect;
}

namespace RowBlast {
    class UserServices;
    class CommonResources;
    
    class SettingsMenuController {
    public:
        enum class Result {
            None,
            GoBack
        };
        
        SettingsMenuController(Pht::IEngine& engine,
                               const CommonResources& commonResources,
                               UserServices& userServices,
                               SettingsMenuView::SceneId sceneId);
        
        void SetUp(bool isGestureControlsAllowed);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        void UpdateViewToReflectSettings(bool isGestureControlsAllowed);
        
        Pht::IEngine& mEngine;
        UserServices& mUserServices;
        SettingsMenuView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
        SettingsMenuView::SceneId mSceneId {SettingsMenuView::SceneId::Game};
    };
}

#endif
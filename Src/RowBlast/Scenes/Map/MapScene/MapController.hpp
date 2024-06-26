#ifndef MapController_hpp
#define MapController_hpp

// Game includes.
#include "MapScene.hpp"
#include "MapViewControllers.hpp"
#include "StoreController.hpp"
#include "MapTutorial.hpp"
#include "Ufo.hpp"
#include "UfoAnimation.hpp"
#include "PortalCameraMovement.hpp"
#include "FireworksParticleEffect.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class LevelResources;
    class PieceResources;
    class Universe;
    
    class MapController {
    public:
        class Command {
        public:
            enum Kind {
                StartGame,
                StartMap,
                ViewTermsOfService,
                ViewPrivacyPolicy,
                ViewCredits,
                None
            };
            
            explicit Command(Kind kind, int level = 0);
            
            int GetLevel() const;
            
            Kind GetKind() const {
                return mKind;
            }

        private:
            Kind mKind {None};
            int mLevel;
        };
    
        MapController(Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      UserServices& userServices,
                      const Universe& universe,
                      const LevelResources& levelResources,
                      const PieceResources& pieceResources);
    
        void Init();
        void GoToStartLevelStateLevelGoalDialog(int levelToStart);
        void GoToAboutMenuState(SlidingMenuAnimation::UpdateFade updateFade);
        void GoToNoMoreLevelsDialogState();
        void GoToUfoAnimationState(int levelToStart);
        Command Update();
        
        void SetStartLevelDialogOnAnimationFinished(bool startLevelDialogOnAnimationFinished) {
            mStartLevelDialogOnAnimationFinished = startLevelDialogOnAnimationFinished;
        }
        
        MapScene& GetScene() {
            return mScene;
        }

    private:
        void InitFireworks();
        void UpdateMap();
        void UpdateUfoAnimation();
        Command UpdateInPortalCameraMovementState();
        Command UpdateInStartLevelState();
        Command UpdateInStartLevelStateLevelGoalDialog();
        void UpdateInStartLevelStateNoLivesDialog();
        void UpdateInStartLevelStateStore();
        void UpdateInStartLevelStateSwipeControlsHintDialog();
        void UpdateInStartLevelStateHowToPlayDialog();
        void UpdateLivesDialog();
        void UpdateInAddLivesState();
        void UpdateInAddLivesStateNoLivesDialog();
        void UpdateInAddLivesStateStore();
        void RefillLives();
        void UpdateOptionsMenu();
        void UpdateSettingsMenu();
        void UpdateHowToPlayDialog();
        Command UpdateAboutMenu();
        void UpdateInAddCoinsStoreState();
        void UpdateNoMoreLevelsDialog();
        void HandleInput();
        void UpdateTouchingState(const Pht::TouchEvent& touch);
        void HandleTouch(const Pht::TouchEvent& touch);
        void HandlePinClick(const MapPin& pin);
        void Pan(const Pht::TouchEvent& touch);
        void StartPan(const Pht::TouchEvent& touch);
        void UpdateCamera();
        void HandleLivesButtonClick();
        void GoToPortalCameraMovementState();
        void GoToStartLevelStateNoLivesDialog(int levelToStart);
        void GoToStartLevelStateStore();
        void GoToStartLevelStateSwipeControlsHintDialog(int levelToStart);
        void GoToStartLevelStateHowToPlayDialogState();
        void GoToLivesDailogState();
        void GoToAddLivesStateNoLivesDialog();
        void GoToAddLivesStateStore();
        void GoToOptionsMenuState();
        void GoToSettingsMenuState();
        void GoToHowToPlayDialogState();
        void GoToAddCoinsStoreState();
        void GoToMapState();
        
        enum class State {
            Map,
            UfoAnimation,
            PortalCameraMovement,
            StartLevel,
            LivesDialog,
            AddLives,
            OptionsMenu,
            SettingsMenu,
            HowToPlayDialog,
            AboutMenu,
            AddCoinsStore,
            NoMoreLevelsDialog
        };
        
        enum class StartLevelState {
            LevelGoalDialog,
            NoLivesDialog,
            Store,
            SwipeControlsHintDialog,
            HowToPlayDialog
        };
        
        enum class AddLivesState {
            NoLivesDialog,
            Store
        };
        
        Pht::IEngine& mEngine;
        UserServices& mUserServices;
        const LevelResources& mLevelResources;
        const Universe& mUniverse;
        State mState {State::Map};
        StartLevelState mStartLevelState {StartLevelState::LevelGoalDialog};
        AddLivesState mAddLivesState {AddLivesState::NoLivesDialog};
        MapScene mScene;
        MapTutorial mTutorial;
        Ufo mUfo;
        UfoAnimation mUfoAnimation;
        PortalCameraMovement mPortalCameraMovement;
        FireworksParticleEffect mFireworksParticleEffect;
        MapViewControllers mMapViewControllers;
        StoreController mStoreController;
        float mCameraXPositionAtPanBegin {0.0f};
        Pht::Vec2 mTouchLocationAtPanBegin {0.0f, 0.0f};
        float mCameraXVelocity {0.0f};
        bool mIsTouching {false};
        int mLevelToStart;
        bool mStartLevelDialogOnAnimationFinished {false};
        bool mHideUfoOnAnimationFinished {false};
        bool mCameraShouldFollowUfo {true};
    };
}

#endif

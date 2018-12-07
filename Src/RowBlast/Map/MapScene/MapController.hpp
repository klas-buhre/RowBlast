#ifndef MapController_hpp
#define MapController_hpp

// Game includes.
#include "MapScene.hpp"
#include "MapViewControllers.hpp"
#include "StoreController.hpp"
#include "Ufo.hpp"
#include "UfoAnimation.hpp"

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
                      PieceResources& pieceResources);
    
        void Init();
        void GoToStartLevelStateLevelGoalDialog(int levelToStart);
        void GoToUfoAnimationState(int levelToStart);
        Command Update();
        
        void SetStartLevelDialogOnAnimationFinished(bool startLevelDialogOnAnimationFinished) {
            mStartLevelDialogOnAnimationFinished = startLevelDialogOnAnimationFinished;
        }
        
        MapScene& GetScene() {
            return mScene;
        }

    private:
        Command UpdateMap();
        void UpdateUfoAnimation();
        Command UpdateInStartLevelState();
        Command UpdateInStartLevelStateLevelGoalDialog();
        void UpdateInStartLevelStateNoLivesDialog();
        void UpdateInStartLevelStateStore();
        void UpdateLivesDialog();
        void UpdateInAddLivesState();
        void UpdateInAddLivesStateNoLivesDialog();
        void UpdateInAddLivesStateStore();
        void RefillLives();
        void UpdateSettingsMenu();
        void UpdateInAddCoinsStoreState();
        Command HandleInput();
        void UpdateTouchingState(const Pht::TouchEvent& touch);
        Command HandleTouch(const Pht::TouchEvent& touch);
        Command HandlePinClick(const MapPin& pin);
        void Pan(const Pht::TouchEvent& touch);
        void StartPan(const Pht::TouchEvent& touch);
        void UpdateCamera();
        void HandleLivesButtonClick();
        void GoToStartLevelStateNoLivesDialog(int levelToStart);
        void GoToStartLevelStateStore();
        void GoToLivesDailogState();
        void GoToAddLivesStateNoLivesDialog();
        void GoToAddLivesStateStore();
        void GoToSettingsMenuState();
        void GoToAddCoinsStoreState();
        void GoToMapState();
        
        enum class State {
            Map,
            UfoAnimation,
            StartLevel,
            LivesDialog,
            AddLives,
            SettingsMenu,
            AddCoinsStore
        };
        
        enum class StartLevelState {
            LevelGoalDialog,
            NoLivesDialog,
            Store
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
        Ufo mUfo;
        UfoAnimation mUfoAnimation;
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

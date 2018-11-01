#ifndef MapController_hpp
#define MapController_hpp

// Game includes.
#include "MapScene.hpp"
#include "MapViewControllers.hpp"
#include "Ufo.hpp"
#include "UfoAnimation.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class Settings;
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
            
            Command(Kind kind, int level = 0);
            
            int GetLevel() const;
            
            Kind GetKind() const {
                return mKind;
            }

        private:
            Kind mKind {None};
            int mLevel;
        };
        
        enum class State {
            Map,
            UfoAnimation,
            LevelGoalDialog,
            NoLivesDialog,
            SettingsMenu
        };
    
        MapController(Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      UserServices& userServices,
                      Settings& settings,
                      const Universe& universe,
                      const LevelResources& levelResources,
                      PieceResources& pieceResources);
    
        void Init();
        void GoToLevelGoalDialogState(int levelToStart);
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
        Command UpdateLevelGoalDialog();
        void UpdateNoLivesDialog();
        void UpdateSettingsMenu();
        Command HandleInput();
        void UpdateTouchingState(const Pht::TouchEvent& touch);
        Command HandleTouch(const Pht::TouchEvent& touch);
        Command HandlePinClick(const MapPin& pin);
        void Pan(const Pht::TouchEvent& touch);
        void StartPan(const Pht::TouchEvent& touch);
        void UpdateCamera();
        void GoToSettingsMenuState();
        
        Pht::IEngine& mEngine;
        UserServices& mUserServices;
        const LevelResources& mLevelResources;
        const Universe& mUniverse;
        State mState {State::Map};
        MapScene mScene;
        Ufo mUfo;
        UfoAnimation mUfoAnimation;
        MapViewControllers mMapViewControllers;
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

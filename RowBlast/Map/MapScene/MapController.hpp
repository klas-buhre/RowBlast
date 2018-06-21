#ifndef MapController_hpp
#define MapController_hpp

// Game includes.
#include "MapScene.hpp"
#include "MapViewControllers.hpp"
#include "Avatar.hpp"
#include "AvatarAnimation.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    class UserData;
    class Settings;
    class LevelResources;
    class PieceResources;
    
    class MapController {
    public:
        class Command {
        public:
            enum Kind {
                StartGame,
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
            AvatarAnimation,
            LevelStartDialog,
            NoLivesDialog,
            SettingsMenu
        };
    
        MapController(Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      UserData& userData,
                      Settings& settings,
                      const LevelResources& levelResources,
                      PieceResources& pieceResources);
    
        void Init();
        void GoToLevelStartDialogState(int levelToStart);
        void GoToAvatarAnimationState(int levelToStart);
        Command Update();
        void SetCameraAtLevel(int levelIndex);
        
        void SetStartLevelDialogOnAnimationFinished(bool startLevelDialogOnAnimationFinished) {
            mStartLevelDialogOnAnimationFinished = startLevelDialogOnAnimationFinished;
        }

    private:
        void UpdateMap();
        void UpdateAvatarAnimation();
        Command UpdateLevelStartDialog();
        void UpdateNoLivesDialog();
        void UpdateSettingsMenu();
        void HandleInput();
        void UpdateTouchingState(const Pht::TouchEvent& touch);
        void HandleTouch(const Pht::TouchEvent& touch);
        void HandleLevelClick(int levelIndex);
        void Pan(const Pht::TouchEvent& touch);
        void StartPan(const Pht::TouchEvent& touch);
        void UpdateCamera();
        void GoToSettingsMenuState();
        
        Pht::IEngine& mEngine;
        UserData& mUserData;
        const LevelResources& mLevelResources;
        State mState {State::Map};
        MapScene mScene;
        Avatar mAvatar;
        AvatarAnimation mAvatarAnimation;
        MapViewControllers mMapViewControllers;
        float mCameraXPositionAtPanBegin {0.0f};
        Pht::Vec2 mTouchLocationAtPanBegin {0.0f, 0.0f};
        float mCameraXVelocity {0.0f};
        bool mIsTouching {false};
        int mLevelToStart;
        bool mStartLevelDialogOnAnimationFinished {false};
    };
}

#endif

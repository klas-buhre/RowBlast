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
            AvatarAnimation,
            LevelStartDialog,
            NoLivesDialog,
            SettingsMenu
        };
    
        MapController(Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      UserData& userData,
                      Settings& settings,
                      const Universe& universe,
                      const LevelResources& levelResources,
                      PieceResources& pieceResources);
    
        void Init();
        void GoToLevelStartDialogState(int levelToStart);
        void GoToAvatarAnimationState(int levelToStart);
        Command Update();
        
        void SetStartLevelDialogOnAnimationFinished(bool startLevelDialogOnAnimationFinished) {
            mStartLevelDialogOnAnimationFinished = startLevelDialogOnAnimationFinished;
        }
        
        MapScene& GetScene() {
            return mScene;
        }

    private:
        Command UpdateMap();
        void UpdateAvatarAnimation();
        Command UpdateLevelStartDialog();
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

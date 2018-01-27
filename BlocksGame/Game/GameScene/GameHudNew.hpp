#ifndef GameHudNew_hpp
#define GameHudNew_hpp

// Game includes.
#include "IGameHudEventListener.hpp"
#include "Level.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
    class Font;
    class LightComponent;
}

namespace BlocksGame {
    class GameLogic;
    class LevelResources;
    class GameHudController;
    
    class GameHudNew: public IGameHudEventListener {
    public:
        GameHudNew(Pht::IEngine& engine,
                const GameLogic& gameLogic,
                const LevelResources& levelResources,
                GameHudController& gameHudController,
                const Pht::Font& font,
                Pht::Scene& scene,
                Pht::SceneObject& parentObject,
                int hudLayer);
        
        void OnSwitchButtonDown() override;
        void OnSwitchButtonUp() override;

        void Update();
        
    private:
        void CreateLightAndCamera(Pht::Scene& scene, Pht::SceneObject& parentObject, int hudLayer);
        void UpdateLightAnimation();
        void UpdateProgress();
        void UpdateMovesLeft();

        Pht::IEngine& mEngine;
        const GameLogic& mGameLogic;
        Level::Objective mLevelObjective {Level::Objective::Clear};
        int mMovesLeft {0};
        int mProgress {0};
        Pht::LightComponent* mLight {nullptr};
        float mLightAnimationTime {0.0f};
    };
}

#endif

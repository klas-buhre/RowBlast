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
    class TextProperties;
    class TextComponent;
    class LightComponent;
}

namespace BlocksGame {
    class GameLogic;
    class LevelResources;
    class PieceResources;
    class GameHudController;
    
    class GameHudNew: public IGameHudEventListener {
    public:
        GameHudNew(Pht::IEngine& engine,
                const GameLogic& gameLogic,
                const LevelResources& levelResources,
                const PieceResources& pieceResources,
                GameHudController& gameHudController,
                const Pht::Font& font,
                Pht::Scene& scene,
                Pht::SceneObject& parentObject,
                int hudLayer,
                const Level& level);
        
        void OnSwitchButtonDown() override;
        void OnSwitchButtonUp() override;

        void Update();
        
    private:
        void CreateLightAndCamera(Pht::Scene& scene, Pht::SceneObject& parentObject, int hudLayer);
        void CreateProgressObject(Pht::Scene& scene,
                                  Pht::SceneObject& parentObject,
                                  const Pht::TextProperties& textProperties,
                                  const LevelResources& levelResources);
        void CreateGrayBlock(Pht::Scene& scene,
                             Pht::SceneObject& progressContainer,
                             const LevelResources& levelResources);
        void CreateBlueprintSlot(Pht::Scene& scene,
                                 Pht::SceneObject& progressContainer,
                                 const LevelResources& levelResources);
        void CreateMovesObject(Pht::Scene& scene,
                               Pht::SceneObject& parentObject,
                               const Pht::TextProperties& textProperties,
                               const PieceResources& pieceResources);
        void CreateLPiece(Pht::Scene& scene,
                          Pht::SceneObject& movesContainer,
                          const PieceResources& pieceResources);
        void CreateGreenBlock(const Pht::Vec3& position,
                              Pht::RenderableObject& blockRenderable,
                              Pht::Scene& scene,
                              Pht::SceneObject& lPiece);
        void CreateSmallPiecesRectangle(const Pht::Vec3& position,
                                        Pht::Scene& scene,
                                        Pht::SceneObject& parentObject);
        void CreateTextRectangle(const Pht::Vec3& position,
                                 float length,
                                 bool isBright,
                                 Pht::Scene& scene,
                                 Pht::SceneObject& parentObject);
        void UpdateLightAnimation();
        void UpdateProgress();
        void UpdateMovesLeft();

        Pht::IEngine& mEngine;
        const GameLogic& mGameLogic;
        Level::Objective mLevelObjective {Level::Objective::Clear};
        int mMovesLeft {0};
        int mProgress {0};
        Pht::TextComponent* mProgressText {nullptr};
        Pht::TextComponent* mMovesText {nullptr};
        Pht::LightComponent* mLight {nullptr};
        float mLightAnimationTime {0.0f};
    };
}

#endif

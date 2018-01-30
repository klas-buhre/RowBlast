#ifndef GameHud_hpp
#define GameHud_hpp

#include <memory>
#include <array>

// Game includes.
#include "IGameHudEventListener.hpp"
#include "Level.hpp"
#include "SceneObjectPool.hpp"

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
    
    class GameHud: public IGameHudEventListener {
    public:
        GameHud(Pht::IEngine& engine,
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
        struct PreviewPiece {
            const Piece* mPieceType {nullptr};
            std::unique_ptr<SceneObjectPool> mSceneObjects;
        };
        
        using TwoPreviewPieces = std::array<PreviewPiece, 2>;

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
                               const Pht::TextProperties& textProperties);
        void CreateLPiece(Pht::Scene& scene, Pht::SceneObject& movesContainer);
        void CreateGreenBlock(const Pht::Vec3& position,
                              Pht::RenderableObject& blockRenderable,
                              Pht::Scene& scene,
                              Pht::SceneObject& lPiece);
        void CreateNextPiecesObject(Pht::Scene& scene,
                                    Pht::SceneObject& parentObject,
                                    const Pht::TextProperties& textProperties,
                                    const Level& level);
        void CreateSelectablePiecesObject(Pht::Scene& scene,
                                          Pht::SceneObject& parentObject,
                                          const Pht::TextProperties& textProperties,
                                          const Level& level);
        void CreateSmallPiecesRectangle(const Pht::Vec3& position,
                                        Pht::Scene& scene,
                                        Pht::SceneObject& parentObject);
        Pht::SceneObject& CreateTextRectangle(const Pht::Vec3& position,
                                              float length,
                                              bool isBright,
                                              Pht::Scene& scene,
                                              Pht::SceneObject& parentObject);
        Pht::SceneObject& CreatePiecesRectangle(const Pht::Vec3& position,
                                                bool isBright,
                                                Pht::Scene& scene,
                                                Pht::SceneObject& parentObject);
        void CreateTwoPreviewPieces(TwoPreviewPieces& previewPieces,
                                    Pht::SceneObject& parentObject,
                                    const Level& level);
        void UpdateLightAnimation();
        void UpdateProgress();
        void UpdateMovesLeft();
        void UpdatePreviewPieces();
        void UpdatePreviewPiece(PreviewPiece& previewPiece, const Piece* currentPiece);
        
        Pht::IEngine& mEngine;
        const GameLogic& mGameLogic;
        const PieceResources& mPieceResources;
        Level::Objective mLevelObjective {Level::Objective::Clear};
        int mMovesLeft {0};
        int mProgress {0};
        Pht::TextComponent* mProgressText {nullptr};
        Pht::TextComponent* mMovesText {nullptr};
        Pht::LightComponent* mLight {nullptr};
        float mLightAnimationTime {0.0f};
        Pht::SceneObject* mSwitchTextRectangle {nullptr};
        Pht::SceneObject* mBrightSwitchTextRectangle {nullptr};
        Pht::SceneObject* mSelectablePiecesRectangle {nullptr};
        Pht::SceneObject* mBrightSelectablePiecesRectangle {nullptr};
        TwoPreviewPieces mNextPieces;
        TwoPreviewPieces mSelectablePieces;
    };
}

#endif

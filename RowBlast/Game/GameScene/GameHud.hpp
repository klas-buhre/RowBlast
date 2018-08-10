#ifndef GameHud_hpp
#define GameHud_hpp

#include <memory>
#include <array>

// Game includes.
#include "IGameHudEventListener.hpp"
#include "Level.hpp"
#include "SceneObjectPool.hpp"
#include "NextPieceGenerator.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
    class Font;
    class TextProperties;
    class TextComponent;
    class LightComponent;
}

namespace RowBlast {
    class GameLogic;
    class LevelResources;
    class PieceResources;
    class GameHudRectangles;
    class GameHudController;
    class CommonResources;
    
    struct PreviewPiece {
        std::unique_ptr<SceneObjectPool> mSceneObjects;
        float mScale {0.0f};
        Pht::SceneObject* mBombSceneObject {nullptr};
        Pht::SceneObject* mRowBombSceneObject {nullptr};
    };

    using ThreePreviewPieces = std::array<PreviewPiece, 3>;
    using PreviewPieceRelativePositions = std::array<Pht::Vec3, 4>;
    
    class GameHud: public IGameHudEventListener {
    public:
        GameHud(Pht::IEngine& engine,
                const GameLogic& gameLogic,
                const LevelResources& levelResources,
                const PieceResources& pieceResources,
                const GameHudRectangles& hudRectangles,
                GameHudController& gameHudController,
                const CommonResources& commonResources,
                Pht::Scene& scene,
                Pht::SceneObject& parentObject,
                int hudLayer,
                const Level& level);
        
        void OnSwitchButtonDown() override;
        void OnSwitchButtonUp() override;
        
        void OnSwitchPieceAnimationFinished();
        void OnNextPieceAnimationFinished();
        void Update();
        
        ThreePreviewPieces& GetNextPreviewPieces() {
            return mNextPreviewPieces;
        }
        
        ThreePreviewPieces& GetSelectablePreviewPieces() {
            return mSelectablePreviewPieces;
        }
        
        const PreviewPieceRelativePositions& GetPreviewPieceRelativePositions() const {
            return mPreviewPieceRelativePositions;
        }

        Pht::SceneObject& GetProgressContainer() {
            assert(mProgressContainer);
            return *mProgressContainer;
        }
        
        Pht::SceneObject& GetMovesContainer() {
            assert(mMovesContainer);
            return *mMovesContainer;
        }

        Pht::SceneObject& GetNextPiecesContainer() {
            assert(mNextPiecesContainer);
            return *mNextPiecesContainer;
        }
        
        Pht::SceneObject& GetSelectablePiecesContainer() {
            assert(mSelectablePiecesContainer);
            return *mSelectablePiecesContainer;
        }
        
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
                               const Pht::TextProperties& textProperties);
        void CreateLPiece(Pht::Scene& scene, Pht::SceneObject& movesContainer);
        void CreateGreenBlock(const Pht::Vec3& position,
                              Pht::RenderableObject& blockRenderable,
                              Pht::Scene& scene,
                              Pht::SceneObject& lPiece);
        void CreateNextPiecesObject(Pht::Scene& scene,
                                    Pht::SceneObject& parentObject,
                                    const GameHudRectangles& hudRectangles);
        void CreateSelectablePiecesObject(Pht::Scene& scene,
                                          Pht::SceneObject& parentObject,
                                          const GameHudRectangles& hudRectangles);
        void CreateThreePreviewPieces(ThreePreviewPieces& previewPieces,
                                      Pht::SceneObject& parentObject);
        void UpdateLightAnimation();
        void UpdateProgress();
        void UpdateMovesLeft();
        void UpdatePreviewPieces();
        void UpdatePreviewPieceGroup(ThreePreviewPieces& previewPieces,
                                     const TwoPieces& pieces,
                                     const TwoPieces& piecesPreviousFrame,
                                     bool shouldStartPreviewPieceAnimation,
                                     bool shouldDeactivateSlotZero);
        void UpdatePreviewPiece(PreviewPiece& previewPiece,
                                const Piece* pieceType,
                                const Pht::Vec3& position);
        
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
        Pht::SceneObject* mSelectablePiecesRectangle {nullptr};
        Pht::SceneObject* mPressedSelectablePiecesRectangle {nullptr};
        Pht::SceneObject* mProgressContainer {nullptr};
        Pht::SceneObject* mMovesContainer {nullptr};
        Pht::SceneObject* mNextPiecesContainer {nullptr};
        Pht::SceneObject* mSelectablePiecesContainer {nullptr};
        ThreePreviewPieces mNextPreviewPieces;
        ThreePreviewPieces mSelectablePreviewPieces;
        TwoPieces mNext2PiecesPreviousFrame;
        TwoPieces mSelectablePiecesPreviousFrame;
        const PreviewPieceRelativePositions mPreviewPieceRelativePositions;
    };
}

#endif

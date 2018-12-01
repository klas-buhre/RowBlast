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
    class TextComponent;
    class LightComponent;
}

namespace RowBlast {
    class GameLogic;
    class Field;
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

    using NextPreviewPieces = std::array<PreviewPiece, 3>;
    using SelectablePreviewPieces = std::array<PreviewPiece, 4>;
    using NextPreviewPiecesRelativePositions = std::array<Pht::Vec3, 4>;
    using SelectablePreviewPiecesRelativePositions = std::array<Pht::Vec3, 5>;
    
    class GameHud: public IGameHudEventListener {
    public:
        GameHud(Pht::IEngine& engine,
                const GameLogic& gameLogic,
                const Field& field,
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
        
        NextPreviewPieces& GetNextPreviewPieces() {
            return mNextPreviewPieces;
        }
        
        SelectablePreviewPieces& GetSelectablePreviewPieces() {
            return mSelectablePreviewPieces;
        }
        
        const NextPreviewPiecesRelativePositions& GetNextPreviewPiecesRelativePositions() const {
            return mNextPreviewPiecesRelativePositions;
        }

        const SelectablePreviewPiecesRelativePositions& GetSelectablePreviewPiecesRelativePositions() const {
            return mSelectablePreviewPiecesRelativePositions;
        }
        
        Pht::SceneObject& GetUpperContainer() {
            assert(mUpperContainer);
            return *mUpperContainer;
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
                                  const Field& field,
                                  const CommonResources& commonResources,
                                  const LevelResources& levelResources);
        void CreateGrayBlock(Pht::Scene& scene,
                             Pht::SceneObject& progressContainer,
                             const CommonResources& commonResources);
        void CreateAsteroid(Pht::Scene& scene,
                            Pht::SceneObject& progressContainer,
                            const LevelResources& levelResources);
        void CreateBlueprintSlot(Pht::Scene& scene,
                                 Pht::SceneObject& progressContainer,
                                 const LevelResources& levelResources);
        void CreateMovesObject(Pht::Scene& scene,
                               Pht::SceneObject& parentObject,
                               const CommonResources& commonResources);
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
        void UpdateLightAnimation();
        void UpdateProgress();
        void UpdateMovesLeft();
        void UpdatePreviewPieces();
        void UpdateNextPreviewPieceGroup(bool shouldStartPreviewPieceAnimation);
        void UpdateSelectablePreviewPieceGroup(bool shouldStartPreviewPieceAnimation,
                                               bool shouldDeactivateSlotZero,
                                               bool shouldStartRemoveActivePieceAnimation);
        void UpdatePreviewPiece(PreviewPiece& previewPiece,
                                const Piece* pieceType,
                                const Pht::Vec3& position);
        
        Pht::IEngine& mEngine;
        const GameLogic& mGameLogic;
        const PieceResources& mPieceResources;
        Level::Objective mLevelObjective {Level::Objective::Clear};
        int mMovesLeft {0};
        int mProgress {-1};
        int mProgressGoal {0};
        Pht::TextComponent* mProgressText {nullptr};
        Pht::TextComponent* mMovesText {nullptr};
        Pht::LightComponent* mLight {nullptr};
        float mLightAnimationTime {0.0f};
        Pht::SceneObject* mSelectablePiecesRectangle {nullptr};
        Pht::SceneObject* mPressedSelectablePiecesRectangle {nullptr};
        Pht::SceneObject* mUpperContainer {nullptr};
        Pht::SceneObject* mProgressContainer {nullptr};
        Pht::SceneObject* mMovesContainer {nullptr};
        Pht::SceneObject* mNextPiecesContainer {nullptr};
        Pht::SceneObject* mSelectablePiecesContainer {nullptr};
        NextPreviewPieces mNextPreviewPieces;
        SelectablePreviewPieces mSelectablePreviewPieces;
        TwoPieces mNext2PiecesPreviousFrame;
        TwoPieces mSelectablePiecesPreviousFrame;
        const Piece* mActivePiecePreviousFrame {nullptr};
        const NextPreviewPiecesRelativePositions mNextPreviewPiecesRelativePositions;
        const SelectablePreviewPiecesRelativePositions mSelectablePreviewPiecesRelativePositions;
    };
}

#endif

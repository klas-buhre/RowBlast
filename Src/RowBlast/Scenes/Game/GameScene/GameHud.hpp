#ifndef GameHud_hpp
#define GameHud_hpp

#include <memory>
#include <array>

// Engine includes.
#include "Button.hpp"

// Game includes.
#include "IGameHudEventListener.hpp"
#include "Level.hpp"
#include "SceneObjectPool.hpp"
#include "NextPieceGenerator.hpp"
#include "GameLogic.hpp"

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
    class GameHudResources;
    class GameHudController;
    class CommonResources;
    
    struct PreviewPiece {
        Pht::SceneObject* mSceneObject {nullptr};
        std::unique_ptr<SceneObjectPool> mSceneObjectPool;
        float mScale {0.0f};
        Pht::SceneObject* mBombSceneObject {nullptr};
        Pht::SceneObject* mRowBombSceneObject {nullptr};
    };

    using NextPreviewPieces = std::array<PreviewPiece, 3>;
    using SelectablePreviewPieces = std::array<PreviewPiece, 4>;
    using NextPreviewPiecesRelativePositions = std::array<Pht::Vec3, 3>;
    using SelectablePreviewPiecesRelativePositions = std::array<Pht::Vec3, 5>;
    using StarSceneObjects = std::array<Pht::SceneObject*, 3>;
    
    class GameHud: public IGameHudEventListener {
    public:
        GameHud(Pht::IEngine& engine,
                const GameLogic& gameLogic,
                const Field& field,
                const LevelResources& levelResources,
                const PieceResources& pieceResources,
                const GameHudResources& gameHudResources,
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
        void RemovePreviewPiece(PreviewPieceIndex previewPieceIndex);
        void ShowPreviewPiece(PreviewPieceIndex previewPieceIndex);
        void Update();
        void ShowBlueMovesIcon();
        void ShowYellowMovesIcon();
        void SetNumMovesLeft(int movesLeft);
        
        enum class NumMovesLeftSource {
            GameLogic,
            Self
        };
        
        void SetNumMovesLeftSource(NumMovesLeftSource source) {
            mNumMovesLeftSource = source;
        }
        
        NextPreviewPieces& GetNextPreviewPieces() {
            return mNextPreviewPieces;
        }
        
        SelectablePreviewPieces& GetSelectablePreviewPieces() {
            return mSelectablePreviewPieces;
        }
        
        Pht::SceneObject* GetActivePreviewPieceSceneObject() {
            return mActivePreviewPiece;
        }

        Pht::SceneObject* GetSelectable0PreviewPieceSceneObject() {
            return mSelectable0PreviewPiece;
        }

        Pht::SceneObject* GetSelectable1PreviewPieceSceneObject() {
            return mSelectable1PreviewPiece;
        }
        
        const NextPreviewPiecesRelativePositions& GetNextPreviewPiecesRelativePositions() const {
            return mNextPreviewPiecesRelativePositions;
        }

        const SelectablePreviewPiecesRelativePositions& GetSelectablePreviewPiecesRelativePositions() const {
            return mSelectablePreviewPiecesRelativePositions;
        }
        
        Pht::Button& GetActivePieceButton() {
            return *mActivePieceButton;
        }
        
        Pht::Button& GetSelectable0Button() {
            return *mSelectable0Button;
        }
        
        Pht::Button& GetSelectable1Button() {
            return *mSelectable1Button;
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

        Pht::SceneObject& GetNextPiecesSceneObject() {
            assert(mNextPiecesSceneObject);
            return *mNextPiecesSceneObject;
        }

        Pht::SceneObject& GetSelectablePiecesContainer() {
            assert(mSelectablePiecesContainer);
            return *mSelectablePiecesContainer;
        }

        Pht::SceneObject& GetSelectablePiecesSceneObject() {
            assert(mSelectablePiecesSceneObject);
            return *mSelectablePiecesSceneObject;
        }

        Pht::SceneObject& GetMovesTextSceneObject() {
            assert(mMovesTextSceneObject);
            return *mMovesTextSceneObject;
        }

        Pht::SceneObject& GetMovesTextContainer() {
            assert(mMovesTextContainer);
            return *mMovesTextContainer;
        }

        Pht::SceneObject& GetMovesIconContainer() {
            assert(mMovesIconContainer);
            return *mMovesIconContainer;
        }
        
        float GetMovesTextScaleFactor() const {
            return mMovesTextScaleFactor;
        }

        static constexpr float movesContainerScale {1.1f};
        static constexpr float movesTextStaticScale {1.3f};
        static constexpr float nextPiecesContainerScale {0.75f};
        static constexpr float nextPiecesScale {0.75f};
        static constexpr float selectablePiecesContainerScale {1.1f};
        static constexpr float selectablePiecesScale {1.2f};

    private:
        void CreateLightAndCamera(Pht::Scene& scene, Pht::SceneObject& parentObject, int hudLayer);
        void CreateUpperBarObject(Pht::Scene& scene, Pht::SceneObject& parentObject, const CommonResources& commonResources);
        void CreateProgressObject(Pht::Scene& scene,
                                  Pht::SceneObject& parentObject,
                                  const CommonResources& commonResources,
                                  const LevelResources& levelResources,
                                  const GameHudResources& gameHudResources);
        void CreateGrayBlock(Pht::Scene& scene,
                             Pht::SceneObject& progressContainer,
                             const CommonResources& commonResources);
        void CreateAsteroid(Pht::Scene& scene,
                            Pht::SceneObject& progressContainer,
                            const LevelResources& levelResources);
        void CreateDownArrow(Pht::Scene& scene,
                             Pht::SceneObject& progressContainer,
                             const GameHudResources& gameHudResources);
        void CreateBlueprintSlot(Pht::Scene& scene,
                                 Pht::SceneObject& progressContainer,
                                 const LevelResources& levelResources);
        void CreateStarMeterObject(Pht::Scene& scene,
                                   Pht::SceneObject& parentObject,
                                   const CommonResources& commonResources);
        void SetIsGoldStarVisible(int index, bool isVisible);
        void CreateStar(int index,
                        const Pht::Vec3& position,
                        Pht::Scene& scene,
                        Pht::SceneObject& parentObject,
                        const CommonResources& commonResources);
        void CreateMovesObject(Pht::Scene& scene,
                               Pht::SceneObject& parentObject,
                               const CommonResources& commonResources,
                               const GameHudResources& gameHudResources);
        Pht::SceneObject& CreateMovesIcon(Pht::Scene& scene,
                                          Pht::SceneObject& movesContainer,
                                          Pht::RenderableObject& arrowRenderable);
        void CreateArrow(const Pht::Vec3& position,
                         const Pht::Vec3& rotation,
                         Pht::RenderableObject& renderable,
                         Pht::Scene& scene,
                         Pht::SceneObject& parent);
        void CreateNextPiecesObject(Pht::Scene& scene,
                                    Pht::SceneObject& parentObject,
                                    const CommonResources& commonResources);
        void CreateLowerBarObject(Pht::Scene& scene,
                                  Pht::SceneObject& parentObject,
                                  const CommonResources& commonResources);
        void CreateSelectablePiecesObject(Pht::Scene& scene,
                                          Pht::SceneObject& parentObject,
                                          const GameHudRectangles& hudRectangles);
        void UpdateLightAnimation();
        void UpdateProgress();
        void UpdateStarMeter();
        void UpdateMovesLeft();
        void UpdatePreviewPieces();
        void UpdateNextPreviewPieceGroup();
        bool ShouldStartNextPieceAnimation() const;
        void UpdateSelectablePreviewPieceGroup();
        void UpdateActivePreviewPiece(PreviewPiece& previewPiece,
                                      const Piece* pieceType,
                                      const Pht::Vec3& position);
        void UpdateSelectable0PreviewPiece(PreviewPiece& previewPiece,
                                           const Piece* pieceType,
                                           const Pht::Vec3& position);
        void UpdateSelectable1PreviewPiece(PreviewPiece& previewPiece,
                                           const Piece* pieceType,
                                           const Pht::Vec3& position);
        void UpdateSelectablePreviewPiece(PreviewPiece& previewPiece,
                                          const Piece* pieceType,
                                          const Pht::Vec3& position);
        void UpdateNextPreviewPiece(PreviewPiece& previewPiece,
                                    const Piece* pieceType,
                                    const Pht::Vec3& position);
        void UpdatePreviewPiece(PreviewPiece& previewPiece,
                                const Piece* pieceType,
                                const Pht::Vec3& position,
                                float slotScale);
        int GetMovesLeft() const;
        
        Pht::IEngine& mEngine;
        const GameLogic& mGameLogic;
        const Field& mField;
        const PieceResources& mPieceResources;
        const Level& mLevel;
        int mScore {-1};
        NumMovesLeftSource mNumMovesLeftSource {NumMovesLeftSource::GameLogic};
        int mMovesLeftSelf {0};
        int mMovesLeft {0};
        int mProgress {-1};
        int mProgressGoal {0};
        std::string mProgressGoalString;
        Pht::TextComponent* mProgressText {nullptr};
        Pht::TextComponent* mMovesText {nullptr};
        Pht::SceneObject* mMovesTextSceneObject {nullptr};
        Pht::SceneObject* mMovesTextContainer {nullptr};
        Pht::SceneObject* mMovesIconContainer {nullptr};
        Pht::SceneObject* mBlueMovesIcon {nullptr};
        Pht::SceneObject* mYellowMovesIcon {nullptr};
        Pht::LightComponent* mLight {nullptr};
        float mLightAnimationTime {0.0f};
        Pht::SceneObject* mSelectablePiecesRectangle {nullptr};
        Pht::SceneObject* mPressedSelectablePiecesRectangle {nullptr};
        Pht::SceneObject* mUpperContainer {nullptr};
        Pht::SceneObject* mProgressContainer {nullptr};
        Pht::SceneObject* mMovesContainer {nullptr};
        Pht::SceneObject* mNextPiecesContainer {nullptr};
        Pht::SceneObject* mNextPiecesSceneObject {nullptr};
        Pht::SceneObject* mSelectablePiecesContainer {nullptr};
        Pht::SceneObject* mSelectablePiecesSceneObject {nullptr};
        Pht::SceneObject* mStarMeterFill {nullptr};
        StarSceneObjects mGoldStars;
        StarSceneObjects mGreyStars;
        NextPreviewPieces mNextPreviewPieces;
        SelectablePreviewPieces mSelectablePreviewPieces;
        TwoPieces mNext2PiecesPreviousFrame;
        TwoPieces mSelectablePiecesPreviousFrame;
        const Piece* mActivePiecePreviousFrame {nullptr};
        Pht::SceneObject* mActivePreviewPiece {nullptr};
        Pht::SceneObject* mSelectable0PreviewPiece {nullptr};
        Pht::SceneObject* mSelectable1PreviewPiece {nullptr};
        const NextPreviewPiecesRelativePositions mNextPreviewPiecesRelativePositions;
        const SelectablePreviewPiecesRelativePositions mSelectablePreviewPiecesRelativePositions;
        float mMovesTextScaleFactor {0.0f};
        std::unique_ptr<Pht::Button> mActivePieceButton;
        std::unique_ptr<Pht::Button> mSelectable0Button;
        std::unique_ptr<Pht::Button> mSelectable1Button;
    };
}

#endif

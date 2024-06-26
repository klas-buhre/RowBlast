#ifndef GameScene_hpp
#define GameScene_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "FloatingBlocks.hpp"
#include "ScrollController.hpp"
#include "SceneObjectPool.hpp"
#include "GameHud.hpp"
#include "FieldBorder.hpp"
#include "FieldGrid.hpp"
#include "UiLayer.hpp"
#include "IGuiLightProvider.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class CameraComponent;
    class ScissorBox;
    class CameraShake;
}

namespace RowBlast {
    class Level;
    class ScrollController;
    class Field;
    class CommonResources;
    class LevelResources;
    class PieceResources;
    class GameLogic;
    class GameHudController;
    class GameHudResources;
    
    class GameScene: public IGuiLightProvider {
    public:
        enum class Layer {
            Background,
            FieldQuad,
            FieldBlueprintSlots,
            FieldPieceDropEffects,
            FieldBlocksAndFallingPiece,
            Effects,
            FlyingBlocks,
            ScoreText,
            LevelCompletedFadeEffect,
            LevelCompletedEffects,
            Hud,
            DraggedPiece,
            UiViews,
            Stars,
            SceneSwitchFadeEffect = GlobalLayer::sceneSwitchFadeEffect
        };
    
        GameScene(Pht::IEngine& engine,
                  const ScrollController& scrollController,
                  const CommonResources& commonResources,
                  const LevelResources& levelResources,
                  const PieceResources& pieceResources,
                  const Field& field,
                  GameHudController& gameHudController,
                  const Pht::CameraShake& cameraShake,
                  const GameHudResources& gameHudResources);

        void SetGuiLightDirections(const Pht::Vec3& directionA,
                                   const Pht::Vec3& directionB) override;
        void SetDefaultGuiLightDirections() override;

        void Init(const Level& level, const GameLogic& gameLogic);
        void Update();
        void UpdateLightAnimation();
        void SetScissorBox(const Pht::ScissorBox& scissorBox);
        const Pht::ScissorBox& GetFieldScissorBox() const;
        void SetUiCameraPosition(const Pht::Vec3& position);
        void SetDefaultUiCameraPosition();
        
        const CommonResources& GetCommonResources() const {
            return mCommonResources;
        }
        
        Pht::Scene& GetScene() {
            assert(mScene);
            return *mScene;
        }

        const Pht::CameraComponent& GetCamera() const {
            assert(mCamera);
            return *mCamera;
        }

        GameHud& GetHud() {
            assert(mHud);
            return *mHud;
        }
        
        const FieldGrid& GetFieldGrid() const {
            return mFieldGrid;
        }

        Pht::SceneObject& GetFieldContainer() {
            assert(mFieldContainer);
            return *mFieldContainer;
        }

        Pht::SceneObject& GetPieceDropEffectsContainer() {
            assert(mPieceDropEffectsContainer);
            return *mPieceDropEffectsContainer;
        }

        Pht::SceneObject& GetFieldBlocksContainer() {
            assert(mFieldBlocksContainer);
            return *mFieldBlocksContainer;
        }

        Pht::SceneObject& GetEffectsContainer() {
            assert(mEffectsContainer);
            return *mEffectsContainer;
        }

        Pht::SceneObject& GetFlyingBlocksContainer() {
            assert(mFlyingBlocksContainer);
            return *mFlyingBlocksContainer;
        }

        Pht::SceneObject& GetScoreTextContainer() {
            assert(mScoreTextContainer);
            return *mScoreTextContainer;
        }

        Pht::SceneObject& GetHudContainer() {
            assert(mHudContainer);
            return *mHudContainer;
        }

        Pht::SceneObject& GetUiViewsContainer() {
            assert(mUiViewsContainer);
            return *mUiViewsContainer;
        }

        SceneObjectPool& GetFieldBlocks() {
            assert(mFieldBlocks);
            return *mFieldBlocks;
        }

        SceneObjectPool& GetPieceBlocks() {
            assert(mPieceBlocks);
            return *mPieceBlocks;
        }
        
        SceneObjectPool& GetDraggedPieceBlocks() {
            assert(mDraggedPieceBlocks);
            return *mDraggedPieceBlocks;
        }

        SceneObjectPool& GetGhostPieces() {
            assert(mGhostPieces);
            return *mGhostPieces;
        }

        SceneObjectPool& GetGhostPieceBlocks() {
            assert(mGhostPieceBlocks);
            return *mGhostPieceBlocks;
        }

        Pht::SceneObject& GetFieldQuadContainer() {
            assert(mFieldQuadContainer);
            return *mFieldQuadContainer;
        }
        
        Pht::SceneObject& GetLevelCompletedEffectsContainer() {
            assert(mLevelCompletedEffectsContainer);
            return *mLevelCompletedEffectsContainer;
        }

        Pht::SceneObject& GetStarsContainer() {
            assert(mStarsContainer);
            return *mStarsContainer;
        }

        Pht::SceneObject& GetDraggedPieceSceneObject() {
            assert(mDraggedPiece);
            return *mDraggedPiece;
        }

        Pht::SceneObject& GetDraggedPieceShadowSceneObject() {
            assert(mDraggedPieceShadow);
            return *mDraggedPieceShadow;
        }

        const Pht::Vec3& GetFieldPosition() const {
            return mFieldPosition;
        }
        
        float GetFieldWidth() const {
            return mFieldWidth;
        }
        
        float GetCellSize() const {
            return mCellSize;
        }

        float GetDraggedPieceZ() const {
            return mDraggedPieceZ;
        }

        float GetBlastAreaZ() const {
            return mBlastAreaZ;
        }

        float GetGhostPieceZ() const {
            return mGhostPieceZ;
        }

        float GetDraggedPieceShadowZ() const {
            return mDraggedPieceShadowZ;
        }

        float GetPressedGhostPieceZ() const {
            return mPressedGhostPieceZ;
        }
        
        float GetBlueprintAnimationZ() const {
            return mBlueprintAnimationZ;
        }

        float GetPiecePathZ() const {
            return mPiecePathZ;
        }

        float GetFieldGridZ() const {
            return mFieldGridZ;
        }
        
        float GetBigAsteroidZ() const {
            return mBigAsteroidZ;
        }

        float GetBouncingBlockZ() const {
            return mBouncingBlockZ;
        }
        
        float GetShieldZ() const {
            return mShieldZ;
        }
        
        float GetFieldBottomGlowZ() const {
            return mFieldBottomGlowZ;
        }

        const Pht::Vec2& GetFieldLoweLeft() const {
            return mFieldLoweLeft;
        }

        const Pht::Vec2& GetDraggedPieceShadowOffset() const {
            return mDraggedPieceShadowOffset;
        }
        
        float GetRowBelowSliceHeightInCells() const {
            return mRowBelowSliceHeightInCells;
        }
        
        float GetFieldPadding() const {
            return mFieldPadding;
        }

    private:
        void UpdateCameraPosition();
        void LoadMusic(const Level& level);
        void CreateRenderPasses();
        void CreateLightAndCamera();
        void CreateBackground(const Level& level);
        void CreateBackgroundLayerLight(const Level& level);
        void CreateFloatingBlocks(const Level& level);
        void CreateLevelCompletedEffectsContainer();
        void InitFieldDimensions(const Level& level);
        void CreateFieldQuad();
        void CreateFieldContainer();
        Pht::QuadMesh::Vertices CreateFieldVertices();
        void CreateBlueprintSlots(const Level& level);
        void CreatePieceDropEffectsContainer();
        void CreateFieldBlocksContainer();
        void CreateSceneObjectPools(const Level& level);
        void CreateDraggedPiece();
        void CreateEffectsContainer();
        void CreateFlyingBlocksContainer();
        void CreateScoreTextContainer();
        void CreateHud(const GameLogic& gameLogic, const Level& level);
        void CreateUiViewsContainer();
        void CreateStarsContainer();
        void InitFieldBorder(const Level& level);
        void SetScissorBox(const Pht::ScissorBox& scissorBox, int layer);
        
        const float mCellSize {1.25f};
        Pht::IEngine& mEngine;
        const ScrollController& mScrollController;
        const CommonResources& mCommonResources;
        const LevelResources& mLevelResources;
        const PieceResources& mPieceResources;
        const Field& mField;
        GameHudController& mGameHudController;
        const Pht::CameraShake& mCameraShake;
        const GameHudResources& mGameHudResources;
        FieldBorder mFieldBorder;
        FieldGrid mFieldGrid;
        Pht::Scene* mScene {nullptr};
        Pht::CameraComponent* mCamera {nullptr};
        Pht::LightComponent* mLight {nullptr};
        Pht::CameraComponent* mUiCamera {nullptr};
        Pht::LightComponent* mUiLight {nullptr};
        Pht::Vec3 mUiLightDirectionA;
        Pht::Vec3 mUiLightDirectionB;
        float mLightAnimationTime {0.0f};
        std::unique_ptr<FloatingBlocks> mFloatingBlocks;
        std::unique_ptr<SceneObjectPool> mFieldBlocks;
        std::unique_ptr<SceneObjectPool> mPieceBlocks;
        std::unique_ptr<SceneObjectPool> mDraggedPieceBlocks;
        std::unique_ptr<SceneObjectPool> mGhostPieces;
        std::unique_ptr<SceneObjectPool> mGhostPieceBlocks;
        Pht::SceneObject* mFieldQuadContainer {nullptr};
        Pht::SceneObject* mFieldContainer {nullptr};
        Pht::SceneObject* mPieceDropEffectsContainer {nullptr};
        Pht::SceneObject* mFieldBlocksContainer {nullptr};
        Pht::SceneObject* mEffectsContainer {nullptr};
        Pht::SceneObject* mFlyingBlocksContainer {nullptr};
        Pht::SceneObject* mScoreTextContainer {nullptr};
        Pht::SceneObject* mHudContainer {nullptr};
        Pht::SceneObject* mUiViewsContainer {nullptr};
        Pht::SceneObject* mLevelCompletedEffectsContainer {nullptr};
        Pht::SceneObject* mStarsContainer {nullptr};
        Pht::SceneObject* mDraggedPiece {nullptr};
        Pht::SceneObject* mDraggedPieceShadow {nullptr};
        Pht::SceneObject* mRowBelowQuad {nullptr};
        const Pht::Vec3 mFieldPosition;
        const float mDraggedPieceZ {mCellSize / 2.0f + 0.3f};
        const float mDraggedPieceShadowZ {mCellSize / 2.0f + 0.2f};
        const float mBlastAreaZ {mCellSize / 2.0f + 0.15f};
        const float mShieldZ {mCellSize / 2.0f + 0.1f};
        const float mFieldBottomGlowZ {mCellSize / 2.0f + 0.1f};
        const float mRowBelowQuadZ {mCellSize / 2.0f + 0.09f};
        const float mGhostPieceZ {mCellSize / 2.0f + 0.02f};
        const float mPressedGhostPieceZ {mCellSize / 2.0f + 0.2f};
        const float mBlueprintAnimationZ {-0.6f};
        const float mPiecePathZ {-0.7f};
        const float mFieldGridZ {-1.8f};
        const float mBigAsteroidZ {-1.0f};
        const float mBouncingBlockZ {-0.2f};
        const float mRowBelowSliceHeightInCells {0.25f};
        const float mFieldPadding {0.1f};
        Pht::Vec2 mDraggedPieceShadowOffset {-0.075f, -0.075f};
        float mFieldWidth {0.0f};
        float mFieldHeight {0.0f};
        Pht::Vec2 mFieldLoweLeft;
        std::unique_ptr<GameHud> mHud;
    };
}

#endif

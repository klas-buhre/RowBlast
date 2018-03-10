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

namespace Pht {
    class IEngine;
    class Scene;
    class CameraComponent;
    class ScissorBox;
}

namespace BlocksGame {
    class Level;
    class ScrollController;
    class CommonResources;
    class LevelResources;
    class PieceResources;
    class GameLogic;
    class GameHudController;
    
    class GameScene {
    public:
        GameScene(Pht::IEngine& engine,
                  const ScrollController& scrollController,
                  const CommonResources& commonResources,
                  GameHudController& gameHudController);
        
        void Init(const Level& level,
                  const LevelResources& levelResources,
                  const PieceResources& pieceResources,
                  const GameLogic& gameLogic);
        void Update();
        const Pht::Material& GetGoldMaterial() const;
        const Pht::Material& GetRedMaterial() const;
        const Pht::Material& GetBlueMaterial() const;
        const Pht::Material& GetGreenMaterial() const;
        const Pht::Material& GetGrayMaterial() const;
        const Pht::Material& GetDarkGrayMaterial() const;
        const Pht::Material& GetLightGrayMaterial() const;
        const Pht::Material& GetYellowMaterial() const;

        GameHud& GetHud() {
            assert(mHud);
            return *mHud;
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

        SceneObjectPool& GetGhostPieces() {
            assert(mGhostPieces);
            return *mGhostPieces;
        }
        
        const Pht::Vec3& GetFieldPosition() const {
            return mFieldPosition;
        }
        
        float GetCellSize() const {
            return mCellSize;
        }

        float GetBlastRadiusAnimationZ() const {
            return mBlastRadiusAnimationZ;
        }

        float GetGhostPieceZ() const {
            return mGhostPieceZ;
        }
        
        float GetPressedGhostPieceZ() const {
            return mPressedGhostPieceZ;
        }
        
        float GetBlueprintAnimationZ() const {
            return mBlueprintAnimationZ;
        }

        const Pht::Vec2& GetFieldLoweLeft() const {
            return mFieldLoweLeft;
        }
        
        float GetGhostPieceOpacity() const {
            return mGhostPieceOpacity;
        }
        
    private:
        void UpdateCameraPositionAndScissorBox();
        void CreateRenderPasses();
        void CreateLightAndCamera();
        void CreateBackground();
        void CreateBackgroundLayerLight();
        void CreateFloatingCubes();
        void InitFieldDimensions(const Level& level);
        void CreateFieldQuad(const Level& level);
        void CreateFieldContainer();
        Pht::QuadMesh::Vertices CreateFieldVertices(const Level& level);
        void CreateBlueprintSlots(const Level& level, const LevelResources& levelResources);
        void CreatePieceDropEffectsContainer();
        void CreateFieldBlocksContainer();
        void CreateSceneObjectPools(const Level& level);
        void CreateEffectsContainer();
        void CreateFlyingBlocksContainer();
        void CreateHud(const GameLogic& gameLogic,
                       const LevelResources& levelResources,
                       const PieceResources& pieceResources,
                       const Level& level);
        void CreateUiViewsContainer();
        void SetScissorBox(const Pht::ScissorBox& scissorBox, int layer);
        void UpdateLightAnimation();
        
        Pht::IEngine& mEngine;
        const ScrollController& mScrollController;
        const CommonResources& mCommonResources;
        GameHudController& mGameHudController;
        Pht::Scene* mScene {nullptr};
        Pht::CameraComponent* mCamera {nullptr};
        Pht::LightComponent* mLight {nullptr};
        float mLightAnimationTime {0.0f};
        std::unique_ptr<FloatingBlocks> mFloatingBlocks;
        std::unique_ptr<SceneObjectPool> mFieldBlocks;
        std::unique_ptr<SceneObjectPool> mPieceBlocks;
        std::unique_ptr<SceneObjectPool> mGhostPieces;
        Pht::SceneObject* mFieldContainer {nullptr};
        Pht::SceneObject* mPieceDropEffectsContainer {nullptr};
        Pht::SceneObject* mFieldBlocksContainer {nullptr};
        Pht::SceneObject* mEffectsContainer {nullptr};
        Pht::SceneObject* mFlyingBlocksContainer {nullptr};
        Pht::SceneObject* mHudContainer {nullptr};
        Pht::SceneObject* mUiViewsContainer {nullptr};
        const Pht::Vec3 mFieldPosition;
        const float mCellSize {1.25f};
        const float mBlastRadiusAnimationZ {mCellSize / 2.0f + 0.1f};
        const float mGhostPieceZ {-0.2f};
        const float mPressedGhostPieceZ {mCellSize / 2.0f + 0.2f};
        const float mBlueprintAnimationZ {-0.6f};
        const float mBlueprintZ {-0.7f};
        float mFieldWidth {0.0f};
        float mFieldHeight {0.0f};
        Pht::Vec2 mFieldLoweLeft;
        const float mGhostPieceOpacity {0.5f};
        std::unique_ptr<GameHud> mHud;
    };
}

#endif

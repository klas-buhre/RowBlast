#ifndef GameScene_hpp
#define GameScene_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "FloatingCubes.hpp"
#include "ScrollController.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class CameraComponent;
}

namespace BlocksGame {
    class Level;
    class ScrollController;
    class CommonResources;
    
    class GameScene {
    public:
        GameScene(Pht::IEngine& engine,
                  const ScrollController& scrollController,
                  const CommonResources& commonResources);
        
        void Reset(const Level& level);
        void Update();
        const Pht::Material& GetGoldMaterial() const;
        const Pht::Material& GetRedMaterial() const;
        const Pht::Material& GetBlueMaterial() const;
        const Pht::Material& GetGreenMaterial() const;
        const Pht::Material& GetGrayMaterial() const;
        const Pht::Material& GetDarkGrayMaterial() const;
        const Pht::Material& GetLightGrayMaterial() const;
        const Pht::Material& GetYellowMaterial() const;
        
        const Pht::Vec3& GetLightDirection() const {
            return mLightDirection;
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

        float GetBlueprintZ() const {
            return mBlueprintZ;
        }
        
        const Pht::Vec2& GetFieldLoweLeft() const {
            return mFieldLoweLeft;
        }
        
        const Pht::Vec2& GetScissorBoxLowerLeft() const {
            return mScissorBoxLowerLeft;
        }

        const Pht::Vec2& GetScissorBoxSize() const {
            return mScissorBoxSize;
        }
       
        float GetGhostPieceOpacity() const {
            return mGhostPieceOpacity;
        }
        
    private:
        void UpdateCameraPositionAndScissorBox();
        void CreateBackground();
        void CreateFieldQuad(const Level& level);
        Pht::QuadMesh::Vertices CreateFieldVertices(const Level& level);
        
        Pht::IEngine& mEngine;
        const ScrollController& mScrollController;
        const CommonResources& mCommonResources;
        Pht::Scene* mScene {nullptr};
        Pht::CameraComponent* mCamera {nullptr};
        std::unique_ptr<FloatingCubes> mFloatingCubes;
        Pht::Vec3 mLightDirection;
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
        Pht::Vec2 mScissorBoxLowerLeft;
        Pht::Vec2 mScissorBoxSize;
        const float mGhostPieceOpacity {0.5f};
    };
}

#endif

#ifndef RenderPass_hpp
#define RenderPass_hpp

#include "IRenderer.hpp"
#include "Optional.hpp"

namespace Pht {
    class LightComponent;
    class CameraComponent;

    enum class RenderOrder {
        Optimized,
        BackToFront
    };

    struct ScissorBox {
        Vec2 mLowerLeft;
        Vec2 mSize;
    };
    
    class RenderPass {
    public:
        explicit RenderPass(int layerIndex);
        
        void SetScissorBox(const ScissorBox& scissorBox);
        void AddLayer(int layerIndex);
        
        void SetProjectionMode(ProjectionMode projectionMode) {
            mProjectionMode = projectionMode;
        }
        
        ProjectionMode GetProjectionMode() const {
            return mProjectionMode;
        }
        
        void SetRenderOrder(RenderOrder renderOrder) {
            mRenderOrder = renderOrder;
        }

        RenderOrder GetRenderOrder() const {
            return mRenderOrder;
        }

        void SetHudMode(bool isHudMode) {
            mIsHudMode = isHudMode;
        }
        
        bool IsHudMode() const {
            return mIsHudMode;
        }

        void SetCamera(const CameraComponent* camera) {
            mCamera = camera;
        }

        const CameraComponent* GetCamera() const {
            return mCamera;
        }

        void SetLight(const LightComponent* light) {
            mLight = light;
        }
        
        const LightComponent* GetLight() const {
            return mLight;
        }
        
        const Optional<ScissorBox>& GetScissorBox() const {
            return mScissorBox;
        }

        Optional<ScissorBox>& GetScissorBox() {
            return mScissorBox;
        }

        int GetLayerMask() const {
            return mLayerMask;
        }

        void SetIsDepthTestAllowed(bool isDepthTestAllowed) {
            mIsDepthTestAllowed = isDepthTestAllowed;
        }
        
        bool IsDepthTestAllowed() const {
            return mIsDepthTestAllowed;
        }

    private:
        ProjectionMode mProjectionMode {ProjectionMode::Perspective};
        RenderOrder mRenderOrder {RenderOrder::Optimized};
        bool mIsHudMode {false};
        const CameraComponent* mCamera {nullptr};
        const LightComponent* mLight {nullptr};
        Optional<ScissorBox> mScissorBox;
        int mLayerMask {0};
        bool mIsDepthTestAllowed {true};
    };
}

#endif

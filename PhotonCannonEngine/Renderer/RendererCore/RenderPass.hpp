#ifndef RenderPass_hpp
#define RenderPass_hpp

#include "IRenderer.hpp"
#include "Optional.hpp"

namespace Pht {
    class LightComponent;
    
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
        
        void SetHudMode(bool isHudMode) {
            mIsHudMode = isHudMode;
        }
        
        bool IsHudMode() const {
            return mIsHudMode;
        }
        
        void SetLight(const LightComponent* light) {
            mLight = light;
        }
        
        const LightComponent* GetLight() const {
            return mLight;
        }
        
        const Optional<ScissorBox> GetScissorBox() const {
            return mScissorBox;
        }
        
        int GetLayerMask() const {
            return mLayerMask;
        }
        
    private:
        ProjectionMode mProjectionMode {ProjectionMode::Perspective};
        bool mIsHudMode {false};
        const LightComponent* mLight {nullptr};
        Optional<ScissorBox> mScissorBox;
        int mLayerMask {0};
    };
}

#endif

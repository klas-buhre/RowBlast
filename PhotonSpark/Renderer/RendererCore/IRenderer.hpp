#ifndef IRenderer_hpp
#define IRenderer_hpp

#include <memory>

#include "Matrix.hpp"

namespace Pht {
    class RenderableObject;
    class GuiView;
    class Scene;
    
    enum class ProjectionMode {
        Perspective,
        Orthographic
    };

    class IRenderer {
    public:
        virtual ~IRenderer() {}
        
        virtual void SetLightDirection(const Vec3& lightDirection) = 0;
        virtual void SetClearColorBuffer(bool clearColorBuffer) = 0;
        virtual void SetHudMode(bool hudMode) = 0;
        virtual void SetProjectionMode(ProjectionMode projectionMode) = 0;
        virtual void RenderObject(const RenderableObject& object, const Mat4& transform) = 0;
        virtual void RenderScene(const Scene& scene) = 0;
        virtual void RenderGuiView(const GuiView& view) = 0;
        virtual int GetAdjustedNumPixels(int numPixels) const = 0;
        virtual const Vec2& GetHudFrustumSize() const = 0;
        virtual const Vec2& GetOrthographicFrustumSize() const = 0;
        virtual const IVec2& GetRenderBufferSize() const = 0;
        virtual const Mat4& GetViewMatrix() const = 0;
        virtual const Mat4& GetProjectionMatrix() const = 0;
    };
}

#endif

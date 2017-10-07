#ifndef IRenderer_hpp
#define IRenderer_hpp

#include <memory>

#include "Matrix.hpp"

namespace Pht {
    class RenderableObject;
    class IMesh;
    class Material;
    class GuiView;
    class TextProperties;
    
    enum class ProjectionMode {
        Perspective,
        Orthographic
    };

    class IRenderer {
    public:
        virtual ~IRenderer() {}
        
        virtual void SetLightPosition(const Vec3& lightPositionWorldSpace) = 0;
        virtual void SetClearColorBuffer(bool clearColorBuffer) = 0;
        virtual void SetHudMode(bool hudMode) = 0;
        virtual void SetDepthTest(bool depthTest) = 0;
        virtual void SetProjectionMode(ProjectionMode projectionMode) = 0;
        virtual void SetHudCameraPosition(const Vec3& cameraPosition) = 0;
        virtual void LookAt(const Vec3& cameraPosition, const Vec3& target, const Vec3& up) = 0;
        virtual void Render(const RenderableObject& object, const Mat4& transform) = 0;
        virtual void RenderText(const std::string& text,
                                const Vec2& position,
                                const TextProperties& properties) = 0;
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

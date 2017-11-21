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
    class SceneObject;
    class Scene;
    
    enum class ProjectionMode {
        Perspective,
        Orthographic
    };

    class IRenderer {
    public:
        virtual ~IRenderer() {}
        
        virtual void SetLightDirection(const Vec3& lightDirection) = 0;
        virtual void SetDirectionalLightIntensity(float intensity) = 0;
        virtual void SetClearColorBuffer(bool clearColorBuffer) = 0;
        virtual void SetHudMode(bool hudMode) = 0;
        virtual void SetDepthTest(bool depthTest) = 0;
        virtual void SetDepthWrite(bool depthWrite) = 0;
        virtual void SetProjectionMode(ProjectionMode projectionMode) = 0;
        virtual void SetHudCameraPosition(const Vec3& cameraPosition) = 0;
        virtual void LookAt(const Vec3& cameraPosition, const Vec3& target, const Vec3& up) = 0;
        virtual void SetScissorBox(const Vec2& lowerLeft, const Vec2& size) = 0;
        virtual void SetScissorTest(bool scissorTest) = 0;
        virtual void Render(const RenderableObject& object, const Mat4& transform) = 0;
        virtual void RenderSceneObject(const SceneObject& sceneObject) = 0;
        virtual void RenderScene(const Scene& scene) = 0;
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

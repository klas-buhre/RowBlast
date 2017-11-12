#ifndef Renderer_hpp
#define Renderer_hpp

#include <memory>
#include <unordered_map>

#include <OpenGLES/ES3/gl.h>

#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "TextRenderer.hpp"

namespace Pht {
    class Renderer: public IRenderer {
    public:
        Renderer(bool createRenderBuffers);
        
        void SetLightPosition(const Vec3& lightPositionWorldSpace) override;
        void SetClearColorBuffer(bool clearColorBuffer) override;
        void SetHudMode(bool hudMode) override;
        void SetDepthTest(bool depthTest) override;
        void SetProjectionMode(ProjectionMode projectionMode) override;
        void SetHudCameraPosition(const Vec3& cameraPosition) override;
        void LookAt(const Vec3& cameraPosition, const Vec3& target, const Vec3& up) override;
        void SetScissorBox(const Vec2& lowerLeft, const Vec2& size) override;
        void SetScissorTest(bool scissorTest) override;
        void Render(const RenderableObject& object, const Mat4& modelTransform) override;
        void RenderSceneObject(const SceneObject& sceneObject) override;
        void RenderScene(const Scene& scene) override;
        void RenderText(const std::string& text,
                        const Vec2& position,
                        const TextProperties& properties) override;
        void RenderGuiView(const GuiView& view) override;
        std::unique_ptr<RenderableObject> CreateRenderableObject(const IMesh& mesh,
                                                                 const Material& material);
        int GetAdjustedNumPixels(int numPixels) const override;
        const Mat4& GetViewMatrix() const override;
        const Mat4& GetProjectionMatrix() const override;
        const Vec2& GetHudFrustumSize() const override;
        const Vec2& GetOrthographicFrustumSize() const override;
        const IVec2& GetRenderBufferSize() const override;

        void Initialize(bool createRenderBuffers);
        void ClearBuffers();
        
    private:
        void InitOpenGl(bool createRenderBuffers);
        void InitCamera();
        void InitHudFrustum();
        void InitShaders();
        void SetupProjectionInShaders();
        void SetLightPositionInShaders();
        const Vec3& GetCameraPosition() const;
        void SetTransforms(const Mat4& modelTransform,
                           const ShaderProgram::UniformHandles& uniforms);
        ShaderProgram& GetShaderProgram(ShaderType shaderType);
        void RenderTextInternal(const std::string& text,
                                const Vec2& position,
                                const TextProperties& properties);
        
        struct HudFrustum {
            Mat4 mProjection;
            Vec2 mSize;
        };
        
        bool mClearColorBuffer {true};
        bool mHudMode {false};
        ProjectionMode mProjectionMode {ProjectionMode::Perspective};
        GLuint mColorRenderbuffer {0};
        Vec3 mLightPositionWorldSpace;
        Vec3 mHudCameraPosition;
        Camera mCamera;
        HudFrustum mHudFrustum;
        Vec2 mOrthographicFrustumSize;
        IVec2 mRenderBufferSize;
        std::unordered_map<ShaderType, ShaderProgram> mShaders;
        std::unique_ptr<TextRenderer> mTextRenderer;
    };
}

#endif

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
#include "RenderQueue.hpp"

namespace Pht {
    class Renderer: public IRenderer {
    public:
        Renderer(bool createRenderBuffers);
        
        void SetLightDirection(const Vec3& lightDirection) override;
        void SetClearColorBuffer(bool clearColorBuffer) override;
        void SetHudMode(bool hudMode) override;
        void SetProjectionMode(ProjectionMode projectionMode) override;
        void RenderObject(const RenderableObject& object, const Mat4& modelTransform) override;
        void RenderScene(const Scene& scene) override;
        void RenderGuiView(const GuiView& view) override;
        int GetAdjustedNumPixels(int numPixels) const override;
        const Mat4& GetViewMatrix() const override;
        const Mat4& GetProjectionMatrix() const override;
        const Vec2& GetHudFrustumSize() const override;
        const Vec2& GetOrthographicFrustumSize() const override;
        const IVec2& GetRenderBufferSize() const override;

        void Init(bool createRenderBuffers);
        void InitRenderQueue(const Scene& scene);
        std::unique_ptr<RenderableObject> CreateRenderableObject(const IMesh& mesh,
                                                                 const Material& material);
        void ClearBuffers();
        
    private:
        void InitOpenGl(bool createRenderBuffers);
        void InitCamera();
        void InitHudFrustum();
        void InitShaders();
        void SetDepthTest(bool depthTest);
        void SetDepthWrite(bool depthWrite);
        void SetScissorBox(const Vec2& lowerLeft, const Vec2& size);
        void SetScissorTest(bool scissorTest);
        void SetupProjectionInShaders();
        void SetLightDirectionInShaders();
        const Vec3& GetCameraPosition() const;
        void SetTransforms(const Mat4& modelTransform,
                           const ShaderProgram::UniformHandles& uniforms);
        void SetMaterialProperties(const ShaderProgram::UniformHandles& uniforms,
                                   const Material& material,
                                   ShaderType shaderType,
                                   const RenderableObject& object);
        ShaderProgram& GetShaderProgram(ShaderType shaderType);
        void RenderText(const std::string& text,
                        const Vec2& position,
                        const TextProperties& properties);
        void RenderTextImpl(const std::string& text,
                            const Vec2& position,
                            const TextProperties& properties);
        void Render(const RenderPass& renderPass, DistanceFunction distanceFunction);
        Vec2 CalculateTextHudPosition(const TextComponent& textComponent);
        
        struct HudFrustum {
            Mat4 mProjection;
            Vec2 mSize;
        };
        
        struct GlobalLight {
            Vec3 mDirectionWorldSpace;
            float mDirectionalIntensity {1.0f};
            float mAmbientIntensity {1.0f};
        };
        
        ProjectionMode mProjectionMode {ProjectionMode::Perspective};
        GLuint mColorRenderbuffer {0};
        GlobalLight mGlobalLight;
        Camera mCamera;
        Vec3 mHudCameraPosition;
        HudFrustum mHudFrustum;
        Vec2 mOrthographicFrustumSize;
        IVec2 mRenderBufferSize;
        RenderQueue mRenderQueue;
        std::unordered_map<ShaderType, ShaderProgram> mShaders;
        std::unique_ptr<TextRenderer> mTextRenderer;
        bool mClearColorBuffer {true};
        bool mHudMode {false};
    };
}

#endif

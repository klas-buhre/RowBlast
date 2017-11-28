#include "Renderer.hpp"

#include <assert.h>

#define STRINGIFY(A)  #A
#include "../Shaders/PixelLighting.vert"
#include "../Shaders/PixelLighting.frag"
#include "../Shaders/VertexLighting.vert"
#include "../Shaders/VertexLighting.frag"
#include "../Shaders/TexturedLighting.vert"
#include "../Shaders/TexturedLighting.frag"
#include "../Shaders/Textured.vert"
#include "../Shaders/Textured.frag"
#include "../Shaders/EnvMap.vert"
#include "../Shaders/EnvMap.frag"
#include "../Shaders/VertexColor.vert"
#include "../Shaders/VertexColor.frag"
#include "../Shaders/Particle.vert"
#include "../Shaders/Particle.frag"
#include "../Shaders/PointParticle.vert"
#include "../Shaders/PointParticle.frag"

#include "RenderableObject.hpp"
#include "Material.hpp"
#include "IMesh.hpp"
#include "Vector.hpp"
#include "VertexBuffer.hpp"
#include "Font.hpp"
#include "GuiView.hpp"
#include "SceneObject.hpp"
#include "Scene.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "TextComponent.hpp"
#include "VboCache.hpp"

using namespace Pht;

namespace {
    const static auto defaultScreenHeight {1136};
    const Mat4 identityMatrix;
   
    struct FrustumSettings {
        float mHeight;
        float mZNearClip;
        float mZFarClip;
    };
    
    const static FrustumSettings perspectiveFrustumSettings {
        .mHeight = 7.1f,
        .mZNearClip = 5.0f,
        .mZFarClip = 750.0f
    };

    const static FrustumSettings orthographicFrustumSettings {
        .mHeight = 24.317f,
        .mZNearClip = -1.0f,
        .mZFarClip = 75.0f
    };

    const static FrustumSettings hudFrustumSettings {
        .mHeight = 26.625f,
        .mZNearClip = -1.0f,
        .mZFarClip = 75.0f
    };

    bool IsParticleShader(ShaderType shaderType) {
        switch (shaderType) {
            case ShaderType::Particle:
            case ShaderType::PointParticle:
                return true;
            default:
                return false;
        }
    }
    
    void BindSpecialTextures(ShaderType shaderType, const Material& material) {
        switch (shaderType) {
            case ShaderType::EnvMap:
                glBindTexture(GL_TEXTURE_CUBE_MAP, material.GetTexture());
                break;
            case ShaderType::PointParticle:
                glBindTexture(GL_TEXTURE_2D, material.GetTexture());
                break;
            default:
                break;
        }
    }
    
    void SetupBlend(bool isParticleShader, const Material& material) {
        if (isParticleShader) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        } else if (material.GetBlend() == Blend::Yes) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else {
            glDisable(GL_BLEND);
        }
    }

    int CalculateStride(VertexFlags vertexFlags) {
        int stride {sizeof(Vec3)};
        
        if (vertexFlags.mNormals) {
            stride += sizeof(Vec3);
        }
        
        if (vertexFlags.mTextureCoords) {
            stride += sizeof(Vec2);
        }
        
        if (vertexFlags.mColors) {
            stride += sizeof(Vec4);
        }
        
        if (vertexFlags.mPointSizes) {
            stride += sizeof(float);
        }
        
        return stride;
    }
    
    void EnableVertexAttributes(const ShaderProgram& shaderProgram,
                                const Material& material,
                                GLuint vertexBufferId) {
        // Bind the vertex buffer.
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

        auto vertexFlags {shaderProgram.GetVertexFlags()};
        auto stride {CalculateStride(vertexFlags)};
        
        auto& attributes {shaderProgram.GetAttributes()};
        
        glEnableVertexAttribArray(attributes.mPosition);
        glVertexAttribPointer(attributes.mPosition, 3, GL_FLOAT, GL_FALSE, stride, 0);
        
        auto offset {sizeof(Vec3)};
        
        if (vertexFlags.mNormals) {
            glEnableVertexAttribArray(attributes.mNormal);
            glVertexAttribPointer(attributes.mNormal,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  stride,
                                  reinterpret_cast<const GLvoid*>(offset));
            offset += sizeof(Vec3);
        }
        
        if (vertexFlags.mTextureCoords) {
            glEnableVertexAttribArray(attributes.mTextureCoord);
            glVertexAttribPointer(attributes.mTextureCoord,
                                  2,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  stride,
                                  reinterpret_cast<const GLvoid*>(offset));
            glBindTexture(GL_TEXTURE_2D, material.GetTexture());
            offset += sizeof(Vec2);
        }

        if (vertexFlags.mColors) {
            glEnableVertexAttribArray(attributes.mColor);
            glVertexAttribPointer(attributes.mColor,
                                  4,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  stride,
                                  reinterpret_cast<const GLvoid*>(offset));
            offset += sizeof(Vec4);
        }

        if (vertexFlags.mPointSizes) {
            glEnableVertexAttribArray(attributes.mPointSize);
            glVertexAttribPointer(attributes.mPointSize,
                                  1,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  stride,
                                  reinterpret_cast<const GLvoid*>(offset));
            offset += sizeof(float);
        }
    }
    
    void DisableVertexAttributes(const ShaderProgram& shaderProgram) {
        auto& attributes {shaderProgram.GetAttributes()};
    
        glDisableVertexAttribArray(attributes.mPosition);
        glDisableVertexAttribArray(attributes.mNormal);
        glDisableVertexAttribArray(attributes.mTextureCoord);
        glDisableVertexAttribArray(attributes.mColor);
        glDisableVertexAttribArray(attributes.mPointSize);
    }
}

Renderer::Renderer(bool createRenderBuffers) :
    mShaders {
        {ShaderType::PixelLighting,    {{.mNormals = true}}},
        {ShaderType::VertexLighting,   {{.mNormals = true}}},
        {ShaderType::TexturedLighting, {{.mNormals = true, .mTextureCoords = true}}},
        {ShaderType::Textured,         {{.mTextureCoords = true}}},
        {ShaderType::EnvMap,           {{.mNormals = true}}},
        {ShaderType::VertexColor,      {{.mColors = true}}},
        {ShaderType::Particle,         {{.mTextureCoords = true, .mColors = true}}},
        {ShaderType::PointParticle,    {{.mColors = true, .mPointSizes = true}}}
    } {
    
    if (createRenderBuffers) {
        glGenRenderbuffers(1, &mColorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mColorRenderbuffer);
    }
}

void Renderer::Initialize(bool createRenderBuffers) {
    InitOpenGl(createRenderBuffers);
    InitCamera();
    InitHudFrustum();
    InitShaders();
    mTextRenderer = std::make_unique<TextRenderer>(mRenderBufferSize);
}

void Renderer::InitOpenGl(bool createRenderBuffers) {
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &mRenderBufferSize.x);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &mRenderBufferSize.y);

    if (createRenderBuffers) {        
        // Create a depth buffer.
        GLuint depthRenderbuffer {0};
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER,
                              GL_DEPTH_COMPONENT16,
                              mRenderBufferSize.x,
                              mRenderBufferSize.y);
        
        // Create the framebuffer object.
        GLuint framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT0,
                                  GL_RENDERBUFFER,
                                  mColorRenderbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mColorRenderbuffer);
        
        glViewport(0, 0, mRenderBufferSize.x, mRenderBufferSize.y);
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void Renderer::InitCamera() {
    mCamera = Camera {
        mRenderBufferSize,
        perspectiveFrustumSettings.mHeight,
        perspectiveFrustumSettings.mZNearClip,
        perspectiveFrustumSettings.mZFarClip
    };
    
    Vec3 cameraPosition {0.0f, 0.0f, 0.0f};
    Vec3 target {0.0f, 0.0f, -10.0f};
    Vec3 up {0.0f, 1.0f, 0.0f};
    mCamera.LookAt(cameraPosition, target, up);
    
    mOrthographicFrustumSize.y = orthographicFrustumSettings.mHeight;
    mOrthographicFrustumSize.x = mOrthographicFrustumSize.y * mRenderBufferSize.x / mRenderBufferSize.y;
    mCamera.SetOrthographicProjection(mRenderBufferSize,
                                      mOrthographicFrustumSize.y,
                                      orthographicFrustumSettings.mZNearClip,
                                      orthographicFrustumSettings.mZFarClip);
}

void Renderer::InitHudFrustum() {
    mHudCameraPosition = Vec3 {0.0f, 0.0f, 0.0f};
    float frustumWidth {hudFrustumSettings.mHeight * mRenderBufferSize.x / mRenderBufferSize.y};
    mHudFrustum.mProjection = Mat4::OrthographicProjection(-frustumWidth / 2.0f,
                                                           frustumWidth / 2.0f,
                                                           -hudFrustumSettings.mHeight / 2.0f,
                                                           hudFrustumSettings.mHeight / 2.0f,
                                                           hudFrustumSettings.mZNearClip,
                                                           hudFrustumSettings.mZFarClip);
    mHudFrustum.mSize = {frustumWidth, hudFrustumSettings.mHeight};
}

void Renderer::InitShaders() {
    
    // Build shaders.
    GetShaderProgram(ShaderType::PixelLighting).Build(PixelLightingVertexShader, PixelLightingFragmentShader);
    GetShaderProgram(ShaderType::VertexLighting).Build(VertexLightingVertexShader, VertexLightingFragmentShader);
    GetShaderProgram(ShaderType::TexturedLighting).Build(TexturedLightingVertexShader, TexturedLightingFragmentShader);
    GetShaderProgram(ShaderType::Textured).Build(TexturedVertexShader, TexturedFragmentShader);
    GetShaderProgram(ShaderType::EnvMap).Build(EnvMapVertexShader, EnvMapFragmentShader);
    GetShaderProgram(ShaderType::VertexColor).Build(VertexColorVertexShader, VertexColorFragmentShader);
    GetShaderProgram(ShaderType::Particle).Build(ParticleVertexShader, ParticleFragmentShader);
    GetShaderProgram(ShaderType::PointParticle).Build(PointParticleVertexShader, PointParticleFragmentShader);
    
    SetupProjectionInShaders();
}

void Renderer::SetupProjectionInShaders() {
    auto& projectionMatrix {GetProjectionMatrix()};
    
    for (auto& shader: mShaders) {
        shader.second.SetProjection(projectionMatrix);
    }
}

void Renderer::ClearBuffers() {
    if (mClearColorBuffer) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void Renderer::SetLightDirection(const Vec3& lightDirection) {
    mGlobalLight.mDirectionWorldSpace = lightDirection;
    SetLightDirectionInShaders();
}

void Renderer::SetDirectionalLightIntensity(float intensity) {
    mGlobalLight.mDirectionalIntensity = intensity;
}

void Renderer::SetLightDirectionInShaders() {
    // The view matrix is made for pre-multiplication so it needs to be transposed in order to
    // post-multiplicate with light position.
    auto transposedViewMatrix {GetViewMatrix().Transposed()};
    auto lightPosCamSpace {transposedViewMatrix * Vec4{mGlobalLight.mDirectionWorldSpace, 0.0f}};
    Vec3 lightPosCamSpaceVec3 {lightPosCamSpace.x, lightPosCamSpace.y, lightPosCamSpace.z};
    auto normalizedLightPosition {lightPosCamSpaceVec3.Normalized()};
    
    for (auto& shader: mShaders) {
        shader.second.SetLightPosition(normalizedLightPosition);
    }
}

void Renderer::SetClearColorBuffer(bool clearColorBuffer) {
    mClearColorBuffer = clearColorBuffer;
}

void Renderer::SetHudMode(bool hudMode) {
    mHudMode = hudMode;
    SetupProjectionInShaders();
}

void Renderer::SetDepthTest(bool depthTest) {
    if (depthTest) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void Renderer::SetDepthWrite(bool depthWrite) {
    if (depthWrite) {
        glDepthMask(GL_TRUE);
    } else {
        glDepthMask(GL_FALSE);
    }
}

void Renderer::SetProjectionMode(ProjectionMode projectionMode) {
    mProjectionMode = projectionMode;
    SetupProjectionInShaders();
}

void Renderer::SetHudCameraPosition(const Vec3& cameraPosition) {
    mHudCameraPosition = cameraPosition;
}

void Renderer::LookAt(const Vec3& cameraPosition, const Vec3& target, const Vec3& up) {
    mCamera.LookAt(cameraPosition, target, up);
    SetLightDirectionInShaders();
}

void Renderer::SetScissorBox(const Vec2& lowerLeft, const Vec2& size) {
    assert(mHudMode || mProjectionMode == ProjectionMode::Orthographic);

    Vec4 localOrigin {0.0f, 0.0f, 0.0f, 1.0f};
    auto modelView {Mat4::Translate(lowerLeft.x, lowerLeft.y, 0.0f) * GetViewMatrix()};
    auto normProjPos {GetProjectionMatrix() * modelView.Transposed() * localOrigin};
    
    auto& frustumSize {mHudMode ? GetHudFrustumSize() : GetOrthographicFrustumSize()};
    
    Pht::Vec2 lowerLeftProjected {
        normProjPos.x * frustumSize.x / 2.0f,
        normProjPos.y * frustumSize.y / 2.0f
    };

    auto pixelX {
        mRenderBufferSize.x / 2.0f + mRenderBufferSize.x * lowerLeftProjected.x / frustumSize.x
    };
    
    auto pixelY {
        mRenderBufferSize.y / 2.0f + mRenderBufferSize.y * lowerLeftProjected.y / frustumSize.y
    };
    
    IVec2 lowerLeftInPixels {static_cast<int>(pixelX), static_cast<int>(pixelY)};
    
    IVec2 sizeInPixels {
        static_cast<int>(mRenderBufferSize.x * size.x / frustumSize.x),
        static_cast<int>(mRenderBufferSize.y * size.y / frustumSize.y)
    };
    
    glScissor(lowerLeftInPixels.x, lowerLeftInPixels.y, sizeInPixels.x, sizeInPixels.y);
}

void Renderer::SetScissorTest(bool scissorTest) {
    if (scissorTest) {
        glEnable(GL_SCISSOR_TEST);
    } else {
        glDisable(GL_SCISSOR_TEST);
    }
}

std::unique_ptr<RenderableObject> Renderer::CreateRenderableObject(const IMesh& mesh,
                                                                   const Material& material) {
    auto shaderProgram {GetShaderProgram(material.GetShaderType())};
    return std::make_unique<RenderableObject>(material, mesh, shaderProgram.GetVertexFlags());
}

void Renderer::Render(const RenderableObject& object, const Mat4& modelTransform) {
    auto& material {object.GetMaterial()};
    auto shaderType {material.GetShaderType()};
    
    // Select and use the shader.
    auto& shaderProgram {GetShaderProgram(shaderType)};
    shaderProgram.Use();
    auto& uniforms {shaderProgram.GetUniforms()};
    
    // Set the transforms used by the shaders.
    SetTransforms(modelTransform, uniforms);

    // Set the material properties used by the shaders.
    SetMaterialProperties(uniforms, material, shaderType, object);

    auto& vbo {object.GetVbo()};
    
    // Enable vertex attribute arrays.
    EnableVertexAttributes(shaderProgram, material, vbo.GetVertexBufferId());

    switch (object.GetRenderMode()) {
        case RenderMode::Triangles:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.GetIndexBufferId());
            glDrawElements(GL_TRIANGLES, vbo.GetIndexCount(), GL_UNSIGNED_SHORT, 0);
            break;
        case RenderMode::Points:
            glDrawArrays(GL_POINTS, 0, vbo.GetPointCount());
            break;
    }
    
    DisableVertexAttributes(shaderProgram);
    
    if (IsParticleShader(shaderType)) {
        SetDepthTest(true);
    }
}

void Renderer::SetTransforms(const Mat4& modelTransform, 
                             const ShaderProgram::UniformHandles& uniforms) {
    // Set up the modelview matrix.
    Mat4 modelview {modelTransform * GetViewMatrix()};
    glUniformMatrix4fv(uniforms.mModelview, 1, 0, modelview.Pointer());

    // Set the normal matrix in camera space. Modelview is orthogonal, so its Inverse-Transpose is
    // itself.
    auto normalMatrix {modelview.ToMat3()};
    glUniformMatrix3fv(uniforms.mNormalMatrix, 1, 0, normalMatrix.Pointer());
    
    // Set the model matrix.
    glUniformMatrix4fv(uniforms.mModel, 1, 0, modelTransform.Pointer());
    glUniformMatrix3fv(uniforms.mModel3x3, 1, 0, modelTransform.ToMat3().Pointer());
    
    // Set the camera position in world space.
    glUniform3fv(uniforms.mCameraPosition, 1, GetCameraPosition().Pointer());
}

void Renderer::SetMaterialProperties(const ShaderProgram::UniformHandles& uniforms,
                                     const Material& material,
                                     ShaderType shaderType,
                                     const RenderableObject& object) {
    auto& ambient {material.GetAmbient()};
    glUniform3f(uniforms.mAmbientMaterial, ambient.mRed, ambient.mGreen, ambient.mBlue);
    
    auto intensity {mGlobalLight.mDirectionalIntensity};
    auto& diffuse {material.GetDiffuse()};
    glUniform3f(uniforms.mDiffuseMaterial,
                diffuse.mRed * intensity,
                diffuse.mGreen * intensity,
                diffuse.mBlue * intensity);
    
    auto& specular {material.GetSpecular()};
    glUniform3f(uniforms.mSpecularMaterial,
                specular.mRed * intensity,
                specular.mGreen * intensity,
                specular.mBlue * intensity);
    
    glUniform1f(uniforms.mShininess, material.GetShininess());
    glUniform1f(uniforms.mReflectivity, material.GetReflectivity());
    glUniform1f(uniforms.mOpacity, material.GetOpacity());
    
    BindSpecialTextures(shaderType, material);
    auto isParticleShader {IsParticleShader(shaderType)};
    SetupBlend(isParticleShader, material);
    
    if (isParticleShader) {
        glDisable(GL_DEPTH_TEST);
    }
}

ShaderProgram& Renderer::GetShaderProgram(ShaderType shaderType) {
    auto shader {mShaders.find(shaderType)};
    assert(shader != std::end(mShaders));
    return shader->second;
}

const Vec3& Renderer::GetCameraPosition() const {
    if (mHudMode) {
        return mHudCameraPosition;
    }
    
    return mCamera.GetPosition();
}

int Renderer::GetAdjustedNumPixels(int numPixels) const {
    return numPixels * mRenderBufferSize.y / defaultScreenHeight;
}

const Mat4& Renderer::GetViewMatrix() const {
    if (mHudMode) {
        return identityMatrix;
    }
    
    return mCamera.GetViewMatrix();
}

const Mat4& Renderer::GetProjectionMatrix() const {
    if (mHudMode) {
        return mHudFrustum.mProjection;
    }
    
    switch (mProjectionMode) {
        case ProjectionMode::Perspective:
            return mCamera.GetProjectionMatrix();
        case ProjectionMode::Orthographic:
            return mCamera.GetOrthographicProjectionMatrix();
    }
}

const Vec2& Renderer::GetHudFrustumSize() const {
    return mHudFrustum.mSize;
}

const Vec2& Renderer::GetOrthographicFrustumSize() const {
    return mOrthographicFrustumSize;
}

const IVec2& Renderer::GetRenderBufferSize() const {
    return mRenderBufferSize;
}

void Renderer::RenderText(const std::string& text,
                          const Vec2& position,
                          const TextProperties& properties) {
    if (properties.mShadow == TextShadow::Yes) {
        TextProperties shadowProperties {properties};
        shadowProperties.mColor = properties.mShadowColor;
        
        RenderTextInternal(text, position, shadowProperties);
        RenderTextInternal(text, position + properties.mOffset * properties.mScale, properties);
    } else {
        RenderTextInternal(text, position, properties);
    }
}

void Renderer::RenderTextInternal(const std::string& text,
                                  const Vec2& position,
                                  const TextProperties& properties) {
    auto pixelX {
        mRenderBufferSize.x / 2.0f + mRenderBufferSize.x * position.x / mHudFrustum.mSize.x
    };
    
    auto pixelY {
        mRenderBufferSize.y / 2.0f + mRenderBufferSize.y * position.y / mHudFrustum.mSize.y
    };
    
    Vec2 pixelPosition {std::floor(pixelX), std::floor(pixelY)};
    
    mTextRenderer->RenderText(text, pixelPosition, properties);
}

void Renderer::RenderGuiView(const GuiView& view) {
    SetHudMode(true);
    SetDepthTest(view.GetDepthTest());
    
    for (auto& sceneObject: view.GetSceneObjects()) {
        auto* renerable {sceneObject->GetRenderable()};
        
        if (renerable && sceneObject->IsVisible()) {
            Render(*renerable, sceneObject->GetMatrix() * view.GetMatrix());
        }
    }

    for (auto& text: view.GetTexts()) {
        if (text->mIsVisible) {
            RenderText(text->mText, view.GetPosition() + text->mPosition, text->mProperties);
        }
    }
    
    SetDepthTest(true);
    SetHudMode(false);
}

void Renderer::RenderSceneObject(const SceneObject& sceneObject) {
    auto* renderable {sceneObject.GetRenderable()};
    
    if (renderable && sceneObject.IsVisible()) {
        Render(*renderable, sceneObject.GetMatrix());
    }
    
    for (auto& child: sceneObject.GetChildren()) {
        RenderSceneObject(*child);
    }
}

void Renderer::RenderScene(const Scene& scene) {
    // Setup camera.
    auto* camera {scene.GetCamera()};
    assert(camera);
    mCamera.LookAt(camera->GetSceneObject().GetWorldSpacePosition(),
                   camera->GetTarget(),
                   camera->GetUp());
    
    // Setup the lighting.
    auto* globalLight {scene.GetGlobalLight()};
    assert(globalLight);
    SetLightDirection(globalLight->GetDirection());
    SetDirectionalLightIntensity(globalLight->GetDirectionalIntensity());
    
    // Build the render queue.
    auto& renderQueue {scene.GetRenderQueue()};
    renderQueue.Build(GetViewMatrix());
    
    // Start by rendering the opaque objects and enable depth write for those.
    SetDepthWrite(true);
    
    RenderQueue::Entry* previousEntry {nullptr};
    
    for (auto& renderEntry: renderQueue) {
        if (!renderEntry.mDepthWrite) {
            if (previousEntry == nullptr || previousEntry->mDepthWrite) {
                // Transition into rendering the transparent objects.
                SetDepthWrite(false);
            }
        }
        
        auto* sceneObject {renderEntry.mSceneObject};
        auto* renderable {sceneObject->GetRenderable()};
        
        if (renderable) {
            Render(*renderable, sceneObject->GetMatrix());
        }
        
        auto* textComponent {sceneObject->GetComponent<TextComponent>()};
        
        if (textComponent) {
            auto sceneObjectPosition {sceneObject->GetWorldSpacePosition()};
            Vec2 textPosition {sceneObjectPosition.x, sceneObjectPosition.y};
            RenderText(textComponent->GetText(), textPosition, textComponent->GetProperties());
        }
        
        previousEntry = &renderEntry;
    }
    
    SetDepthWrite(true);
}

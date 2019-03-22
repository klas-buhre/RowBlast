#include "Renderer.hpp"

#include <assert.h>

#define STRINGIFY(A)  #A
#include "../Shaders/PixelLighting.vert"
#include "../Shaders/PixelLighting.frag"
#include "../Shaders/VertexLighting.vert"
#include "../Shaders/VertexLighting.frag"
#include "../Shaders/TexturedLighting.vert"
#include "../Shaders/TexturedLighting.frag"
#include "../Shaders/TexturedEmissiveLighting.vert"
#include "../Shaders/TexturedEmissiveLighting.frag"
#include "../Shaders/TexturedEnvMapLighting.vert"
#include "../Shaders/TexturedEnvMapLighting.frag"
#include "../Shaders/Textured.vert"
#include "../Shaders/Textured.frag"
#include "../Shaders/EnvMap.vert"
#include "../Shaders/EnvMap.frag"
#include "../Shaders/VertexColor.vert"
#include "../Shaders/VertexColor.frag"
#include "../Shaders/Particle.vert"
#include "../Shaders/Particle.frag"
#include "../Shaders/ParticleTextureColor.vert"
#include "../Shaders/ParticleTextureColor.frag"
#include "../Shaders/ParticleNoAlphaTexture.vert"
#include "../Shaders/ParticleNoAlphaTexture.frag"
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
#include "ISceneManager.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "TextComponent.hpp"
#include "VboCache.hpp"

using namespace Pht;

namespace {
    constexpr auto defaultScreenHeight = 1136;
    constexpr auto topPadding = 1.05f;
    constexpr auto bottomPadding = 1.67f;
    const Mat4 identityMatrix;
    const Vec4 modelSpaceOrigin {0.0f, 0.0f, 0.0f, 1.0f};
    
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
            case ShaderType::ParticleTextureColor:
            case ShaderType::ParticleNoAlphaTexture:
            case ShaderType::PointParticle:
                return true;
            default:
                return false;
        }
    }

    void BindSpecialTextures(ShaderType shaderType, const Material& material) {
        switch (shaderType) {
            case ShaderType::EnvMap:
                glBindTexture(GL_TEXTURE_CUBE_MAP, material.GetEnvMapTexture()->GetHandle());
                break;
            case ShaderType::PointParticle:
                glBindTexture(GL_TEXTURE_2D, material.GetTexture()->GetHandle());
                break;
            case ShaderType::TexturedEmissiveLighting:
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, material.GetEmissionTexture()->GetHandle());
                break;
            case ShaderType::TexturedEnvMapLighting:
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, material.GetEnvMapTexture()->GetHandle());
                break;
            default:
                break;
        }
    }
    
    void SetupBlend(bool isParticleShader, const Material& material) {
        switch (material.GetBlend()) {
            case Blend::Yes:
                glEnable(GL_BLEND);
                if (isParticleShader) {
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                } else {
                    auto* texture = material.GetTexture();
                    if (texture && texture->HasPremultipliedAlpha()) {
                        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                    } else {
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    }
                }
                break;
            case Blend::Additive:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
            case Blend::No:
                glDisable(GL_BLEND);
                break;
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

        auto vertexFlags = shaderProgram.GetVertexFlags();
        auto stride = CalculateStride(vertexFlags);
        
        auto& attributes = shaderProgram.GetAttributes();
        
        glEnableVertexAttribArray(attributes.mPosition);
        glVertexAttribPointer(attributes.mPosition, 3, GL_FLOAT, GL_FALSE, stride, 0);
        
        auto offset = sizeof(Vec3);
        
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
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.GetTexture()->GetHandle());
            
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
        auto& attributes = shaderProgram.GetAttributes();
    
        glDisableVertexAttribArray(attributes.mPosition);
        glDisableVertexAttribArray(attributes.mNormal);
        glDisableVertexAttribArray(attributes.mTextureCoord);
        glDisableVertexAttribArray(attributes.mColor);
        glDisableVertexAttribArray(attributes.mPointSize);
    }
}

Renderer::Renderer(bool createRenderBuffers) :
    mShaders {
        {ShaderType::PixelLighting,            {{.mNormals = true}}},
        {ShaderType::VertexLighting,           {{.mNormals = true}}},
        {ShaderType::TexturedLighting,         {{.mNormals = true, .mTextureCoords = true}}},
        {ShaderType::TexturedEmissiveLighting, {{.mNormals = true, .mTextureCoords = true}}},
        {ShaderType::TexturedEnvMapLighting,   {{.mNormals = true, .mTextureCoords = true}}},
        {ShaderType::Textured,                 {{.mTextureCoords = true}}},
        {ShaderType::EnvMap,                   {{.mNormals = true}}},
        {ShaderType::VertexColor,              {{.mColors = true}}},
        {ShaderType::Particle,                 {{.mTextureCoords = true, .mColors = true}}},
        {ShaderType::ParticleTextureColor,     {{.mTextureCoords = true, .mColors = true}}},
        {ShaderType::ParticleNoAlphaTexture,   {{.mTextureCoords = true, .mColors = true}}},
        {ShaderType::PointParticle,            {{.mColors = true, .mPointSizes = true}}}
    } {
    
    if (createRenderBuffers) {
        glGenRenderbuffers(1, &mColorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mColorRenderbuffer);
    }
}

void Renderer::Init(bool createRenderBuffers) {
    std::cout << "Pht::Renderer: Initializing..." << std::endl;
    
    InitOpenGl(createRenderBuffers);
    InitShaders();
    InitCamera(ISceneManager::defaultNarrowFrustumHeightFactor);

    mTextRenderer = std::make_unique<TextRenderer>(mRenderBufferSize);
    
    std::cout << "Pht::Renderer: Using " << mRenderBufferSize.x << "x" << mRenderBufferSize.y
              << " resolution." << std::endl;
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
    
    glEnable(GL_CULL_FACE);
}

void Renderer::InitCamera() {
    mCamera = Camera {
        mRenderBufferSize,
        perspectiveFrustumSettings.mHeight * GetFrustumHeightFactor(),
        perspectiveFrustumSettings.mZNearClip,
        perspectiveFrustumSettings.mZFarClip
    };
    
    Vec3 cameraPosition {0.0f, 0.0f, 0.0f};
    Vec3 target {0.0f, 0.0f, -10.0f};
    Vec3 up {0.0f, 1.0f, 0.0f};
    mCamera.LookAt(cameraPosition, target, up);
    
    mOrthographicFrustumSize.y = orthographicFrustumSettings.mHeight * GetFrustumHeightFactor();
    mOrthographicFrustumSize.x = mOrthographicFrustumSize.y * mRenderBufferSize.x / mRenderBufferSize.y;
    mCamera.SetOrthographicProjection(mRenderBufferSize,
                                      mOrthographicFrustumSize.y,
                                      orthographicFrustumSettings.mZNearClip,
                                      orthographicFrustumSettings.mZFarClip);
}

void Renderer::InitHudFrustum() {
    mHudCameraPosition = Vec3 {0.0f, 0.0f, 0.0f};
    auto frustumHeight = hudFrustumSettings.mHeight * GetFrustumHeightFactor();
    float frustumWidth {frustumHeight * mRenderBufferSize.x / mRenderBufferSize.y};
    mHudFrustum.mProjection = Mat4::OrthographicProjection(-frustumWidth / 2.0f,
                                                           frustumWidth / 2.0f,
                                                           -frustumHeight / 2.0f,
                                                           frustumHeight / 2.0f,
                                                           hudFrustumSettings.mZNearClip,
                                                           hudFrustumSettings.mZFarClip);
    mHudFrustum.mSize = {frustumWidth, frustumHeight};
}

float Renderer::GetAspectRatio() const {
    return static_cast<float>(mRenderBufferSize.y) / static_cast<float>(mRenderBufferSize.x);
}

float Renderer::GetFrustumHeightFactor() const {
    if (GetAspectRatio() >= 2.0f) {
        return mNarrowFrustumHeightFactor;
    }
    
    return 1.0f;
}

void Renderer::InitShaders() {
    
    // Build shaders.
    GetShaderProgram(ShaderType::PixelLighting).Build(PixelLightingVertexShader, PixelLightingFragmentShader);
    GetShaderProgram(ShaderType::VertexLighting).Build(VertexLightingVertexShader, VertexLightingFragmentShader);
    GetShaderProgram(ShaderType::TexturedLighting).Build(TexturedLightingVertexShader, TexturedLightingFragmentShader);
    GetShaderProgram(ShaderType::TexturedEmissiveLighting).Build(TexturedEmissiveLightingVertexShader, TexturedEmissiveLightingFragmentShader);
    GetShaderProgram(ShaderType::TexturedEnvMapLighting).Build(TexturedEnvMapLightingVertexShader, TexturedEnvMapLightingFragmentShader);
    GetShaderProgram(ShaderType::Textured).Build(TexturedVertexShader, TexturedFragmentShader);
    GetShaderProgram(ShaderType::EnvMap).Build(EnvMapVertexShader, EnvMapFragmentShader);
    GetShaderProgram(ShaderType::VertexColor).Build(VertexColorVertexShader, VertexColorFragmentShader);
    GetShaderProgram(ShaderType::Particle).Build(ParticleVertexShader, ParticleFragmentShader);
    GetShaderProgram(ShaderType::ParticleTextureColor).Build(ParticleTextureColorVertexShader, ParticleTextureColorFragmentShader);
    GetShaderProgram(ShaderType::ParticleNoAlphaTexture).Build(ParticleNoAlphaTextureVertexShader, ParticleNoAlphaTextureFragmentShader);
    GetShaderProgram(ShaderType::PointParticle).Build(PointParticleVertexShader, PointParticleFragmentShader);
}

void Renderer::SetupProjectionInShaders() {
    auto& projectionMatrix = GetProjectionMatrix();
    
    for (auto& shader: mShaders) {
        shader.second.SetProjection(projectionMatrix);
    }
}

void Renderer::InitRenderQueue(const Scene& scene) {
    mRenderQueue.Init(scene.GetRoot());
}

std::unique_ptr<RenderableObject> Renderer::CreateRenderableObject(const IMesh& mesh,
                                                                   const Material& material) {
    auto shaderProgram = GetShaderProgram(material.GetShaderType());
    return std::make_unique<RenderableObject>(material, mesh, shaderProgram.GetVertexFlags());
}

void Renderer::ClearBuffers() {
    if (mClearColorBuffer) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void Renderer::RenderScene(const Scene& scene) {
    const CameraComponent* previousCamera = nullptr;
    const LightComponent* previousLight = nullptr;
    
    for (auto& renderPass: scene.GetRenderPasses()) {
        if (!renderPass.IsEnabled()) {
            continue;
        }

        // Set up camera.
        auto* cameraOverride = renderPass.GetCamera();
        auto* camera = cameraOverride ? cameraOverride : scene.GetCamera();
        assert(camera);
        if (camera != previousCamera) {
            auto cameraPositionWorldSpace = camera->GetSceneObject().GetWorldSpacePosition();
            
            if (renderPass.IsHudMode()) {
                mHudCameraPosition = cameraPositionWorldSpace;
            } else {
                mCamera.LookAt(cameraPositionWorldSpace, camera->GetTarget(), camera->GetUp());
            }
            
            SetLightDirectionInShaders();
            previousCamera = camera;
        }

        // Set up the lighting.
        auto* lightOverride = renderPass.GetLight();
        auto* light = lightOverride ? lightOverride : scene.GetGlobalLight();
        assert(light);
        if (light != previousLight) {
            SetLightDirection(light->GetDirection());
            mGlobalLight.mDirectionalIntensity = light->GetDirectionalIntensity();
            mGlobalLight.mAmbientIntensity = light->GetAmbientIntensity();
            previousLight = light;
        }
        
        // Render the pass.
        Render(renderPass, scene.GetDistanceFunction());
    }
}

void Renderer::Render(const RenderPass& renderPass, DistanceFunction distanceFunction) {
    auto isHudMode = renderPass.IsHudMode();
    auto projectionMode = renderPass.GetProjectionMode();
    if (isHudMode != mHudMode || projectionMode != mProjectionMode) {
        SetHudMode(isHudMode);
        SetProjectionMode(projectionMode);
        SetupProjectionInShaders();
    }
    
    auto& scissorBox = renderPass.GetScissorBox();
    if (scissorBox.HasValue()) {
        auto& scissorBoxValue = scissorBox.GetValue();
        SetScissorBox(scissorBoxValue.mLowerLeft, scissorBoxValue.mSize);
        SetScissorTest(true);
    }
    
    mIsDepthTestAllowed = renderPass.IsDepthTestAllowed();
    
    // Build the render queue.
    mRenderQueue.Build(GetViewMatrix(),
                       renderPass.GetRenderOrder(),
                       distanceFunction,
                       renderPass.GetLayerMask());
    
    if (renderPass.GetRenderOrder() == RenderOrder::Optimized) {
        // Start by rendering the opaque objects and enable depth write for those.
        SetDepthWrite(true);
    }
    
    RenderQueue::Entry* previousEntry {nullptr};
    
    for (auto& renderEntry: mRenderQueue) {
        if (renderPass.GetRenderOrder() == RenderOrder::Optimized) {
            if (!renderEntry.mDepthWrite) {
                if (previousEntry == nullptr || previousEntry->mDepthWrite) {
                    // Transition into rendering the transparent objects.
                    SetDepthWrite(false);
                }
            }
        } else {
            SetDepthWrite(renderEntry.mDepthWrite);
        }
        
        auto* sceneObject = renderEntry.mSceneObject;
        if (auto* renderable = sceneObject->GetRenderable()) {
            RenderObject(*renderable, sceneObject->GetMatrix());
        }
        
        if (auto* textComponent = sceneObject->GetComponent<TextComponent>()) {
            auto textPosition = CalculateTextHudPosition(*textComponent);
            RenderText(textComponent->GetText(), textPosition, textComponent->GetProperties());
        }
        
        previousEntry = &renderEntry;
    }
    
    SetDepthWrite(true);
    
    if (scissorBox.HasValue()) {
        SetScissorTest(false);
    }
}

void Renderer::SetLightDirection(const Vec3& lightDirection) {
    mGlobalLight.mDirectionWorldSpace = lightDirection;
    SetLightDirectionInShaders();
}

void Renderer::SetLightDirectionInShaders() {
    // Since the matrix is row-major it has to be transposed in order to multiply with the vector.
    auto transposedViewMatrix = GetViewMatrix().Transposed();
    auto lightPosCamSpace = transposedViewMatrix * Vec4{mGlobalLight.mDirectionWorldSpace, 0.0f};
    Vec3 lightPosCamSpaceVec3 {lightPosCamSpace.x, lightPosCamSpace.y, lightPosCamSpace.z};
    auto normalizedLightPosition = lightPosCamSpaceVec3.Normalized();
    
    for (auto& shader: mShaders) {
        shader.second.SetLightPosition(normalizedLightPosition);
    }
}

void Renderer::EnableShader(ShaderType shaderType) {
    GetShaderProgram(shaderType).SetIsEnabled(true);
}

void Renderer::DisableShader(ShaderType shaderType) {
    GetShaderProgram(shaderType).SetIsEnabled(false);
}

void Renderer::SetClearColorBuffer(bool clearColorBuffer) {
    mClearColorBuffer = clearColorBuffer;
}

void Renderer::SetDepthTest(const DepthState& depthState) {
    if (!mIsDepthTestAllowed && !depthState.mDepthTestAllowedOverride) {
        glDisable(GL_DEPTH_TEST);
    } else {
        if (depthState.mDepthTest) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }
}

void Renderer::SetDepthWrite(bool depthWrite) {
    if (depthWrite) {
        glDepthMask(GL_TRUE);
    } else {
        glDepthMask(GL_FALSE);
    }
}

void Renderer::SetHudMode(bool hudMode) {
    mHudMode = hudMode;
}

void Renderer::SetProjectionMode(ProjectionMode projectionMode) {
    mProjectionMode = projectionMode;
}

void Renderer::SetScissorBox(const Vec2& lowerLeft, const Vec2& size) {
    assert(mHudMode || mProjectionMode == ProjectionMode::Orthographic);
    
    Vec4 localOrigin {0.0f, 0.0f, 0.0f, 1.0f};
    auto modelView = Mat4::Translate(lowerLeft.x, lowerLeft.y, 0.0f) * GetViewMatrix();
    auto modelViewProjection = modelView * GetProjectionMatrix();
    
    // Since the matrix is row-major it has to be transposed in order to multiply with the vector.
    auto clipSpacePos = modelViewProjection.Transposed() * localOrigin;
    auto normProjPos = clipSpacePos / clipSpacePos.w;
    
    auto& frustumSize = mHudMode ? GetHudFrustumSize() : GetOrthographicFrustumSize();
    
    Vec2 lowerLeftProjected {
        normProjPos.x * frustumSize.x / 2.0f,
        normProjPos.y * frustumSize.y / 2.0f
    };

    auto pixelX =
        mRenderBufferSize.x / 2.0f + mRenderBufferSize.x * lowerLeftProjected.x / frustumSize.x;
    
    auto pixelY =
        mRenderBufferSize.y / 2.0f + mRenderBufferSize.y * lowerLeftProjected.y / frustumSize.y;
    
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

void Renderer::RenderObject(const RenderableObject& object, const Mat4& modelTransform) {
    auto& material = object.GetMaterial();
    auto shaderType = material.GetShaderType();
    
    // Select and use the shader.
    auto& shaderProgram = GetShaderProgram(shaderType);
    assert(shaderProgram.IsEnabled());
    shaderProgram.Use();
    auto& uniforms = shaderProgram.GetUniforms();
    
    // Set the transforms used by the shaders.
    SetTransforms(modelTransform, uniforms);

    // Set the material properties used by the shaders.
    SetMaterialProperties(uniforms, material, shaderType, object);

    auto& vbo = object.GetVbo();
    
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
}

void Renderer::SetTransforms(const Mat4& modelTransform, 
                             const ShaderProgram::UniformHandles& uniforms) {
    // Note: the matrix in the matrix lib is row-major while OpenGL expects column-major. However,
    // it works since all transforms are created in row-major order while OpenGL reads the matrix in
    // column-major order which transposes it. Transposing the matrix is required in order to
    // multiply with a vector: M * v.
    auto modelview = modelTransform * GetViewMatrix();
    auto modelViewProjection = modelview * GetProjectionMatrix();
    glUniformMatrix4fv(uniforms.mModelViewProjection, 1, 0, modelViewProjection.Pointer());

    // Set the normal matrix in camera space. Modelview is orthogonal, so its Inverse-Transpose is
    // itself.
    auto normalMatrix = modelview.ToMat3();
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
    auto ambientlIntensity = mGlobalLight.mAmbientIntensity;
    auto& ambient = material.GetAmbient();
    glUniform3f(uniforms.mAmbientMaterial,
                ambient.mRed * ambientlIntensity,
                ambient.mGreen * ambientlIntensity,
                ambient.mBlue * ambientlIntensity);
    
    auto directionalIntensity = mGlobalLight.mDirectionalIntensity;
    auto& diffuse = material.GetDiffuse();
    glUniform3f(uniforms.mDiffuseMaterial,
                diffuse.mRed * directionalIntensity,
                diffuse.mGreen * directionalIntensity,
                diffuse.mBlue * directionalIntensity);
    
    auto& specular = material.GetSpecular();
    glUniform3f(uniforms.mSpecularMaterial,
                specular.mRed * directionalIntensity,
                specular.mGreen * directionalIntensity,
                specular.mBlue * directionalIntensity);

    auto& emissive = material.GetEmissive();
    glUniform3f(uniforms.mEmissiveMaterial, emissive.mRed, emissive.mGreen, emissive.mBlue);

    glUniform1f(uniforms.mShininess, material.GetShininess());
    glUniform1f(uniforms.mReflectivity, material.GetReflectivity());
    glUniform1f(uniforms.mOpacity, material.GetOpacity());
    
    glUniform1i(uniforms.mSampler, 0);
    glUniform1i(uniforms.mSecondSampler, 1);
    
    BindSpecialTextures(shaderType, material);
    SetupBlend(IsParticleShader(shaderType), material);
    SetDepthTest(material.GetDepthState());
}

ShaderProgram& Renderer::GetShaderProgram(ShaderType shaderType) {
    auto shader = mShaders.find(shaderType);
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
    return numPixels * mRenderBufferSize.y / (defaultScreenHeight * GetFrustumHeightFactor());
}

void Renderer::InitCamera(float narrowFrustumHeightFactor) {
    mNarrowFrustumHeightFactor = narrowFrustumHeightFactor;

    InitCamera();
    InitHudFrustum();
    SetupProjectionInShaders();
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

float Renderer::GetTopPaddingHeight() const {
    if (GetAspectRatio() >= 2.0f) {
        return topPadding * mNarrowFrustumHeightFactor;
    }
    
    return 0.0f;
}

float Renderer::GetBottomPaddingHeight() const {
    if (GetAspectRatio() >= 2.0f) {
        return bottomPadding * mNarrowFrustumHeightFactor;
    }
    
    return 0.0f;
}

void Renderer::RenderText(const std::string& text,
                          const Vec2& position,
                          const TextProperties& properties) {
    auto textPosition = position;

    if (properties.mSecondShadow == TextShadow::Yes) {
        TextProperties shadowProperties {properties};
        shadowProperties.mColor = properties.mSecondShadowColor;
        shadowProperties.mMidGradientColorSubtraction = Optional<Vec3> {};
        shadowProperties.mTopGradientColorSubtraction = Optional<Vec3> {};

        RenderTextImpl(text,
                       position - properties.mSecondShadowOffset * properties.mScale,
                       shadowProperties);
    }

    if (properties.mShadow == TextShadow::Yes) {
        TextProperties shadowProperties {properties};
        shadowProperties.mColor = properties.mShadowColor;
        shadowProperties.mMidGradientColorSubtraction = Optional<Vec3> {};
        shadowProperties.mTopGradientColorSubtraction = Optional<Vec3> {};

        RenderTextImpl(text, position, shadowProperties);
        
        textPosition = position + properties.mShadowOffset * properties.mScale;
    }
    
    if (properties.mSpecular == TextSpecular::Yes) {
        TextProperties specularProperties {properties};
        specularProperties.mColor = properties.mSpecularColor;
        specularProperties.mMidGradientColorSubtraction = Optional<Vec3> {};
        specularProperties.mTopGradientColorSubtraction = Optional<Vec3> {};

        RenderTextImpl(text,
                       textPosition + properties.mSpecularOffset * properties.mScale,
                       specularProperties);
    }
        
    RenderTextImpl(text, textPosition, properties);
}

void Renderer::RenderTextImpl(const std::string& text,
                              const Vec2& position,
                              const TextProperties& properties) {
    auto pixelX =
        mRenderBufferSize.x / 2.0f + mRenderBufferSize.x * position.x / mHudFrustum.mSize.x;
    
    auto pixelY =
        mRenderBufferSize.y / 2.0f + mRenderBufferSize.y * position.y / mHudFrustum.mSize.y;
    
    Vec2 pixelPosition {pixelX, pixelY};
    
    if (properties.mSnapToPixel == SnapToPixel::Yes) {
        pixelPosition.x = std::floor(pixelPosition.x);
        pixelPosition.y = std::floor(pixelPosition.y);
    }
    
    auto italicSlant =
        mRenderBufferSize.x * properties.mItalicSlant * properties.mScale / mHudFrustum.mSize.x;
    
    mTextRenderer->RenderText(text, pixelPosition, italicSlant, properties);
}

Vec2 Renderer::CalculateTextHudPosition(const TextComponent& textComponent) {
    auto& sceneObject = textComponent.GetSceneObject();
    
    if (mHudMode) {
        auto sceneObjectPosition = sceneObject.GetWorldSpacePosition();
        return Vec2 {sceneObjectPosition.x, sceneObjectPosition.y};
    }

    auto modelView = sceneObject.GetMatrix() * GetViewMatrix();
    auto modelViewProjection = modelView * GetProjectionMatrix();
    
    // Since the matrix is row-major it has to be transposed in order to multiply with the vector.
    auto clipSpacePos = modelViewProjection.Transposed() * modelSpaceOrigin;
    auto normProjPos = clipSpacePos / clipSpacePos.w;
    auto& hudSize = GetHudFrustumSize();

    return Pht::Vec2 {normProjPos.x * hudSize.x / 2.0f, normProjPos.y * hudSize.y / 2.0f};
}

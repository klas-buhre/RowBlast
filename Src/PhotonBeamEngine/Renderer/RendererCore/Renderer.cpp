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

    bool IsParticleShader(ShaderId shaderId) {
        switch (shaderId) {
            case ShaderId::Particle:
            case ShaderId::ParticleTextureColor:
            case ShaderId::ParticleNoAlphaTexture:
            case ShaderId::PointParticle:
                return true;
            default:
                return false;
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
    
    void EnableVertexAttributes(const ShaderProgram& shaderProgram) {
        auto vertexFlags = shaderProgram.GetVertexFlags();
        auto stride = CalculateStride(vertexFlags);
        auto& attributes = shaderProgram.GetAttributes();
        
        glDisableVertexAttribArray(attributes.mTextCoords);
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
        } else {
            glDisableVertexAttribArray(attributes.mNormal);
        }
        
        if (vertexFlags.mTextureCoords) {
            glEnableVertexAttribArray(attributes.mTextureCoord);
            glVertexAttribPointer(attributes.mTextureCoord,
                                  2,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  stride,
                                  reinterpret_cast<const GLvoid*>(offset));
            offset += sizeof(Vec2);
        } else {
            glDisableVertexAttribArray(attributes.mTextureCoord);
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
        } else {
            glDisableVertexAttribArray(attributes.mColor);
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
        } else {
            glDisableVertexAttribArray(attributes.mPointSize);
        }
    }
}

Renderer::Renderer(bool createRenderBuffers) {
    CreateShader(ShaderId::PixelLighting,            {.mNormals = true});
    CreateShader(ShaderId::VertexLighting,           {.mNormals = true});
    CreateShader(ShaderId::TexturedLighting,         {.mNormals = true, .mTextureCoords = true});
    CreateShader(ShaderId::TexturedEmissiveLighting, {.mNormals = true, .mTextureCoords = true});
    CreateShader(ShaderId::TexturedEnvMapLighting,   {.mNormals = true, .mTextureCoords = true});
    CreateShader(ShaderId::Textured,                 {.mTextureCoords = true});
    CreateShader(ShaderId::EnvMap,                   {.mNormals = true});
    CreateShader(ShaderId::VertexColor,              {.mColors = true});
    CreateShader(ShaderId::Particle,                 {.mTextureCoords = true, .mColors = true});
    CreateShader(ShaderId::ParticleTextureColor,     {.mTextureCoords = true, .mColors = true});
    CreateShader(ShaderId::ParticleNoAlphaTexture,   {.mTextureCoords = true, .mColors = true});
    CreateShader(ShaderId::PointParticle,            {.mColors = true, .mPointSizes = true});
    
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

    mRenderState.Init();
    mRenderState.SetCullFace(true);
    mTextRenderer = std::make_unique<TextRenderer>(mRenderState, mRenderBufferSize);

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
    GetShader(ShaderId::PixelLighting).Build(PixelLightingVertexShader, PixelLightingFragmentShader);
    GetShader(ShaderId::VertexLighting).Build(VertexLightingVertexShader, VertexLightingFragmentShader);
    GetShader(ShaderId::TexturedLighting).Build(TexturedLightingVertexShader, TexturedLightingFragmentShader);
    GetShader(ShaderId::TexturedEmissiveLighting).Build(TexturedEmissiveLightingVertexShader, TexturedEmissiveLightingFragmentShader);
    GetShader(ShaderId::TexturedEnvMapLighting).Build(TexturedEnvMapLightingVertexShader, TexturedEnvMapLightingFragmentShader);
    GetShader(ShaderId::Textured).Build(TexturedVertexShader, TexturedFragmentShader);
    GetShader(ShaderId::EnvMap).Build(EnvMapVertexShader, EnvMapFragmentShader);
    GetShader(ShaderId::VertexColor).Build(VertexColorVertexShader, VertexColorFragmentShader);
    GetShader(ShaderId::Particle).Build(ParticleVertexShader, ParticleFragmentShader);
    GetShader(ShaderId::ParticleTextureColor).Build(ParticleTextureColorVertexShader, ParticleTextureColorFragmentShader);
    GetShader(ShaderId::ParticleNoAlphaTexture).Build(ParticleNoAlphaTextureVertexShader, ParticleNoAlphaTextureFragmentShader);
    GetShader(ShaderId::PointParticle).Build(PointParticleVertexShader, PointParticleFragmentShader);
}

void Renderer::InitRenderQueue(const Scene& scene) {
    mRenderQueue.Init(scene.GetRoot());
}

std::unique_ptr<RenderableObject> Renderer::CreateRenderableObject(const IMesh& mesh,
                                                                   const Material& material) {
    auto& shaderProgram = GetShader(material.GetShaderId());
    return std::make_unique<RenderableObject>(material, mesh, shaderProgram.GetVertexFlags());
}

void Renderer::ClearFrameBuffer() {
    // Depth writing has to be enabled in order to clear the depth buffer.
    mRenderState.SetDepthWrite(true);

    if (mClearColorBuffer) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void Renderer::RenderScene(const Scene& scene, float frameSeconds) {
    const CameraComponent* previousCamera = nullptr;
    const LightComponent* previousLight = nullptr;
    
    IF_USING_FRAME_STATS(mRenderState.ResetFrameStats());
    
    for (auto& renderPass: scene.GetRenderPasses()) {
        if (!renderPass.IsEnabled()) {
            continue;
        }
        
        mRenderState.OnBeginRenderPass();

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
            
            CalculateCameraSpaceLightDirection();
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
    
    IF_USING_FRAME_STATS(mRenderState.LogFrameStats(frameSeconds));
}

void Renderer::Render(const RenderPass& renderPass, DistanceFunction distanceFunction) {
    auto isHudMode = renderPass.IsHudMode();
    auto projectionMode = renderPass.GetProjectionMode();
    if (isHudMode != mHudMode || projectionMode != mProjectionMode) {
        SetHudMode(isHudMode);
        SetProjectionMode(projectionMode);
    }
    
    auto& scissorBox = renderPass.GetScissorBox();
    if (scissorBox.HasValue()) {
        auto& scissorBoxValue = scissorBox.GetValue();
        SetScissorBox(scissorBoxValue.mLowerLeft, scissorBoxValue.mSize);
        mRenderState.SetScissorTest(true);
    } else {
        mRenderState.SetScissorTest(false);
    }
    
    mIsDepthTestAllowed = renderPass.IsDepthTestAllowed();
    
    // Build the render queue.
    mRenderQueue.Build(GetViewMatrix(),
                       renderPass.GetRenderOrder(),
                       distanceFunction,
                       renderPass.GetLayerMask());
    
    if (renderPass.GetRenderOrder() == RenderOrder::Optimized) {
        // Start by rendering the opaque objects and enable depth write for those.
        mRenderState.SetDepthWrite(true);
    }
    
    RenderQueue::Entry* previousEntry {nullptr};
    
    for (auto& renderEntry: mRenderQueue) {
        if (renderPass.GetRenderOrder() == RenderOrder::Optimized) {
            if (!renderEntry.mDepthWrite) {
                if (previousEntry == nullptr || previousEntry->mDepthWrite) {
                    // Transition into rendering the transparent objects.
                    mRenderState.SetDepthWrite(false);
                }
            }
        } else {
            mRenderState.SetDepthWrite(renderEntry.mDepthWrite);
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
}

void Renderer::SetLightDirection(const Vec3& lightDirection) {
    mGlobalLight.mDirectionWorldSpace = lightDirection;
    CalculateCameraSpaceLightDirection();
}

void Renderer::CalculateCameraSpaceLightDirection() {
    // Since the matrix is row-major it has to be transposed in order to multiply with the vector.
    auto transposedViewMatrix = GetViewMatrix().Transposed();
    auto lightPosCamSpace = transposedViewMatrix * Vec4{mGlobalLight.mDirectionWorldSpace, 0.0f};
    Vec3 lightPosCamSpaceVec3 {lightPosCamSpace.x, lightPosCamSpace.y, lightPosCamSpace.z};
    mGlobalLight.mDirectionCameraSpace = lightPosCamSpaceVec3.Normalized();
}

void Renderer::EnableShader(ShaderId shaderId) {
    GetShader(shaderId).SetIsEnabled(true);
}

void Renderer::DisableShader(ShaderId shaderId) {
    GetShader(shaderId).SetIsEnabled(false);
}

void Renderer::SetClearColorBuffer(bool clearColorBuffer) {
    mClearColorBuffer = clearColorBuffer;
}

void Renderer::SetDepthTest(const DepthState& depthState) {
    if (!mIsDepthTestAllowed && !depthState.mDepthTestAllowedOverride) {
        mRenderState.SetDepthTest(false);
    } else {
        if (depthState.mDepthTest) {
            mRenderState.SetDepthTest(true);
        } else {
            mRenderState.SetDepthTest(false);
        }
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

void Renderer::RenderObject(const RenderableObject& renderableObject, const Mat4& modelTransform) {
    auto& material = renderableObject.GetMaterial();
    auto shaderId = material.GetShaderId();
    auto& shaderProgram = GetShader(shaderId);
    
    auto isShaderSameAsLastDraw = mRenderState.IsShaderInUse(shaderProgram);
    if (!isShaderSameAsLastDraw) {
        mRenderState.UseShader(shaderProgram);
    }
    
    SetTransforms(modelTransform, shaderProgram);
    
    if (!isShaderSameAsLastDraw || !mRenderState.IsMaterialInUse(material)) {
        mRenderState.UseMaterial(material);
        SetMaterialProperties(material, shaderId, shaderProgram);
    }
    
    auto& vbo = renderableObject.GetVbo();
    if (!isShaderSameAsLastDraw || !mRenderState.IsVboInUse(vbo)) {
        mRenderState.UseVbo(vbo);
        SetVbo(renderableObject, shaderProgram);
    }

    switch (renderableObject.GetRenderMode()) {
        case RenderMode::Triangles:
            glDrawElements(GL_TRIANGLES, vbo.GetIndexCount(), GL_UNSIGNED_SHORT, 0);
            break;
        case RenderMode::Points:
            glDrawArrays(GL_POINTS, 0, vbo.GetPointCount());
            break;
    }
    
    IF_USING_FRAME_STATS(mRenderState.ReportDrawCall());
}

void Renderer::SetTransforms(const Mat4& modelTransform, ShaderProgram& shaderProgram) {
    // Note: the matrix in the matrix lib is row-major while OpenGL expects column-major. However,
    // it works since all transforms are created in row-major order while OpenGL reads the matrix in
    // column-major order which transposes it. Transposing the matrix is required in order to
    // multiply with a vector: M * v.
    auto modelview = modelTransform * GetViewMatrix();
    auto modelViewProjection = modelview * GetProjectionMatrix();
    auto& uniforms = shaderProgram.GetUniforms();
    glUniformMatrix4fv(uniforms.mModelViewProjection, 1, 0, modelViewProjection.Pointer());

    // Set the normal matrix in camera space. Modelview is orthogonal, so its Inverse-Transpose is
    // itself.
    auto normalMatrix = modelview.ToMat3();
    glUniformMatrix3fv(uniforms.mNormalMatrix, 1, 0, normalMatrix.Pointer());
    
    // Set the model matrix.
    glUniformMatrix4fv(uniforms.mModel, 1, 0, modelTransform.Pointer());
    glUniformMatrix3fv(uniforms.mModel3x3, 1, 0, modelTransform.ToMat3().Pointer());
    
    // Set the camera position in world space.
    shaderProgram.SetCameraPosition(GetCameraPosition());
    
    // Set light direction in camera space.
    shaderProgram.SetLightPosition(mGlobalLight.mDirectionCameraSpace);
}

void Renderer::SetMaterialProperties(const Material& material,
                                     ShaderId shaderId,
                                     const ShaderProgram& shaderProgram) {
    auto& uniforms = shaderProgram.GetUniforms();
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
    
    BindTextures(material, shaderId, shaderProgram);
    SetupBlend(material, shaderId);
    SetDepthTest(material.GetDepthState());
}

void Renderer::BindTextures(const Material& material,
                            ShaderId shaderId,
                            const ShaderProgram& shaderProgram) {
    switch (shaderId) {
        case ShaderId::EnvMap:
            mRenderState.BindTexture(GL_TEXTURE0,
                                     GL_TEXTURE_CUBE_MAP,
                                     material.GetEnvMapTexture()->GetHandle());
            break;
        case ShaderId::TexturedEmissiveLighting:
            mRenderState.BindTexture(GL_TEXTURE1,
                                     GL_TEXTURE_2D,
                                     material.GetEmissionTexture()->GetHandle());
            break;
        case ShaderId::TexturedEnvMapLighting:
            mRenderState.BindTexture(GL_TEXTURE1,
                                     GL_TEXTURE_CUBE_MAP,
                                     material.GetEnvMapTexture()->GetHandle());
            break;
        default:
            break;
    }
    
    if (shaderProgram.GetVertexFlags().mTextureCoords) {
        mRenderState.BindTexture(GL_TEXTURE0,
                                 GL_TEXTURE_2D,
                                 material.GetTexture()->GetHandle());
    }
}


void Renderer::SetupBlend(const Material& material, ShaderId shaderId) {
    switch (material.GetBlend()) {
        case Blend::Yes:
            mRenderState.SetBlend(true);
            if (IsParticleShader(shaderId)) {
                mRenderState.SetBlendFunc(GL_SRC_ALPHA, GL_ONE);
            } else {
                auto* texture = material.GetTexture();
                if (texture && texture->HasPremultipliedAlpha()) {
                    mRenderState.SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                } else {
                    mRenderState.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                }
            }
            break;
        case Blend::Additive:
            mRenderState.SetBlend(true);
            mRenderState.SetBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case Blend::No:
            mRenderState.SetBlend(false);
            break;
    }
}

void Renderer::SetVbo(const RenderableObject& renderableObject,
                      const ShaderProgram& shaderProgram) {
    auto& vbo = renderableObject.GetVbo();
    
    // Bind the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, vbo.GetVertexBufferId());
    
    // Enable vertex attribute arrays.
    EnableVertexAttributes(shaderProgram);
    
    if (renderableObject.GetRenderMode() == RenderMode::Triangles) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.GetIndexBufferId());
    }
}

void Renderer::CreateShader(ShaderId shaderId, const VertexFlags& vertexFlags) {
    auto shaderProgram = std::make_unique<ShaderProgram>(vertexFlags);
    mShaders.insert(std::make_pair(shaderId, std::move(shaderProgram)));
}

ShaderProgram& Renderer::GetShader(ShaderId shaderId) {
    auto shader = mShaders.find(shaderId);
    assert(shader != std::end(mShaders));
    return *shader->second;
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

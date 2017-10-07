#include "Renderer.hpp"

#define STRINGIFY(A)  #A
#include "../Shaders/PixelLighting.vert"
#include "../Shaders/PixelLighting.frag"
#include "../Shaders/VertexLighting.vert"
#include "../Shaders/VertexLighting.frag"
#include "../Shaders/TexturedLighting.vert"
#include "../Shaders/TexturedLighting.frag"
#include "../Shaders/Textured.vert"
#include "../Shaders/Textured.frag"
#include "../Shaders/CubeMap.vert"
#include "../Shaders/CubeMap.frag"
#include "../Shaders/VertexColor.vert"
#include "../Shaders/VertexColor.frag"
#include "../Shaders/Particle.vert"
#include "../Shaders/Particle.frag"

#include "RenderableObject.hpp"
#include "Material.hpp"
#include "IMesh.hpp"
#include "Vector.hpp"
#include "VertexBuffer.hpp"
#include "Font.hpp"
#include "GuiView.hpp"
#include "SceneObject.hpp"

using namespace Pht;

namespace {
    const auto defaultScreenHeight {1136};
    const Mat4 identityMatrix;
    
    void SetMaterialProperties(const ShaderProgram::UniformHandles& uniforms,
                               const Material& material,
                               ShaderType shaderType,
                               const RenderableObject& object) {
        auto& ambient {material.GetAmbient()};
        glUniform3f(uniforms.mAmbientMaterial, ambient.mRed, ambient.mGreen, ambient.mBlue);
        
        auto& diffuse {material.GetDiffuse()};
        glUniform3f(uniforms.mDiffuseMaterial, diffuse.mRed, diffuse.mGreen, diffuse.mBlue);
        
        auto& specular {material.GetSpecular()};
        glUniform3f(uniforms.mSpecularMaterial, specular.mRed, specular.mGreen, specular.mBlue);
        
        glUniform1f(uniforms.mShininess, material.GetShininess());
        glUniform1f(uniforms.mReflectivity, material.GetReflectivity());
        glUniform1f(uniforms.mOpacity, material.GetOpacity());
        
        switch (shaderType) {
            case ShaderType::CubeMap:
                glBindTexture(GL_TEXTURE_CUBE_MAP, material.GetTexture());
                break;
            case ShaderType::Particle:
                glBindTexture(GL_TEXTURE_2D, material.GetTexture());
                glDisable(GL_DEPTH_TEST);
                break;
            default:
                break;
        }
        
        if (shaderType == ShaderType::Particle) {
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
                                const RenderableObject& object) {
        // Bind the vertex buffer.
        glBindBuffer(GL_ARRAY_BUFFER, object.GetVertexBufferId());

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
    mPixelLightingShader    {{.mNormals = true}},
    mVertexLightingShader   {{.mNormals = true}},
    mTexturedLightingShader {{.mNormals = true, .mTextureCoords = true}},
    mTexturedShader         {{.mTextureCoords = true}},
    mCubeMapShader          {{.mNormals = true}},
    mVertexColorShader      {{.mColors = true}},
    mParticleShader         {{.mColors = true, .mPointSizes = true}} {
    
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
    auto frustumHeight {7.1f};
    auto zNear {5.0f};
    auto zFar {750.0f};
    
    mCamera = Camera {mRenderBufferSize, frustumHeight, zNear, zFar};
    Vec3 cameraPosition {0.0f, 0.0f, 0.0f};
    Vec3 target {0.0f, 0.0f, -10.0f};
    Vec3 up {0.0f, 1.0f, 0.0f};
    mCamera.LookAt(cameraPosition, target, up);
    
    mOrthographicFrustumSize.y = 24.317f;
    mOrthographicFrustumSize.x = mOrthographicFrustumSize.y * mRenderBufferSize.x / mRenderBufferSize.y;
    mCamera.SetOrthographicProjection(mRenderBufferSize, mOrthographicFrustumSize.y, -1.0f, 75.0f);
}

void Renderer::InitHudFrustum() {
    mHudCameraPosition = Vec3 {0.0f, 0.0f, 0.0f};
    auto frustumHeight {26.625f};
    float frustumWidth {frustumHeight * mRenderBufferSize.x / mRenderBufferSize.y};
    mHudFrustum.mProjection = Mat4::OrthographicProjection(-frustumWidth / 2.0f,
                                                           frustumWidth / 2.0f,
                                                           -frustumHeight / 2.0f,
                                                           frustumHeight / 2.0f,
                                                           -1.0f,
                                                           75.0f);
    mHudFrustum.mSize = {frustumWidth, frustumHeight};
}

void Renderer::InitShaders() {
    
    // Build shaders.
    mPixelLightingShader.Build(PixelLightingVertexShader, PixelLightingFragmentShader);
    mVertexLightingShader.Build(VertexLightingVertexShader, VertexLightingFragmentShader);
    mTexturedLightingShader.Build(TexturedLightingVertexShader, TexturedLightingFragmentShader);
    mTexturedShader.Build(TexturedVertexShader, TexturedFragmentShader);
    mCubeMapShader.Build(CubeMapVertexShader, CubeMapFragmentShader);
    mVertexColorShader.Build(VertexColorVertexShader, VertexColorFragmentShader);
    mParticleShader.Build(ParticleVertexShader, ParticleFragmentShader);
    
    SetupProjectionInShaders();
}

void Renderer::SetupProjectionInShaders() {
    auto& projectionMatrix {GetProjectionMatrix()};
    
    mPixelLightingShader.SetProjection(projectionMatrix);
    mVertexLightingShader.SetProjection(projectionMatrix);
    mTexturedLightingShader.SetProjection(projectionMatrix);
    mTexturedShader.SetProjection(projectionMatrix);
    mCubeMapShader.SetProjection(projectionMatrix);
    mVertexColorShader.SetProjection(projectionMatrix);
    mParticleShader.SetProjection(projectionMatrix);
}

void Renderer::ClearBuffers() {
    if (mClearColorBuffer) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void Renderer::SetLightPosition(const Vec3& lightPositionWorldSpace) {
    mLightPositionWorldSpace = lightPositionWorldSpace;
    SetLightPositionInShaders();
}

void Renderer::SetLightPositionInShaders() {
    // The view matrix is made for pre-multiplication so it needs to be transposed in order to
    // post-multiplicate with light position.
    auto transposedViewMatrx {GetViewMatrix().Transposed()};
    auto lightPosCamSpace {transposedViewMatrx * Vec4{mLightPositionWorldSpace, 0.0f}};
    Vec3 lightPosCamSpaceVec3 {lightPosCamSpace.x, lightPosCamSpace.y, lightPosCamSpace.z};
    auto normalizedLightPosition {lightPosCamSpaceVec3.Normalized()};
    
    mPixelLightingShader.SetLightPosition(normalizedLightPosition);
    mVertexLightingShader.SetLightPosition(normalizedLightPosition);
    mTexturedLightingShader.SetLightPosition(normalizedLightPosition);
    mTexturedShader.SetLightPosition(normalizedLightPosition);
    mCubeMapShader.SetLightPosition(normalizedLightPosition);
    mParticleShader.SetLightPosition(normalizedLightPosition);
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

void Renderer::SetProjectionMode(ProjectionMode projectionMode) {
    mProjectionMode = projectionMode;
    SetupProjectionInShaders();
}

void Renderer::SetHudCameraPosition(const Vec3& cameraPosition) {
    mHudCameraPosition = cameraPosition;
}

void Renderer::LookAt(const Vec3& cameraPosition, const Vec3& target, const Vec3& up) {
    mCamera.LookAt(cameraPosition, target, up);
    SetLightPositionInShaders();
}

std::unique_ptr<RenderableObject> Renderer::CreateRenderableObject(const IMesh& mesh,
                                                                   const Material& material) {
    auto shaderProgram {GetShaderProgram(material.GetShaderType())};
    VertexBuffer vertexBuffer {mesh.GetVertices(shaderProgram.GetVertexFlags())};
    
    // Create the VBO for the vertices, normals and texture coords.
    GLuint vertexBufferId {0};
    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexBuffer.GetVertexBufferSize() * sizeof(float),
                 vertexBuffer.GetVertexBuffer(),
                 GL_STATIC_DRAW);
    
    // Create the VBO for the indices.
    GLuint indexBufferId {0};
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 vertexBuffer.GetIndexBufferSize() * sizeof(GLushort),
                 vertexBuffer.GetIndexBuffer(),
                 GL_STATIC_DRAW);
    
    return std::make_unique<RenderableObject>(material,
                                              vertexBufferId,
                                              indexBufferId,
                                              vertexBuffer.GetIndexBufferSize());
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

    // Enable vertex attribute arrays.
    EnableVertexAttributes(shaderProgram, material, object);

    switch (object.GetRenderMode()) {
        case RenderMode::Triangles:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.GetIndexBufferId());
            glDrawElements(GL_TRIANGLES, object.GetIndexCount(), GL_UNSIGNED_SHORT, 0);
            break;
        case RenderMode::Points:
            glDrawArrays(GL_POINTS, 0, object.GetPointCount());
            break;
    }
    
    DisableVertexAttributes(shaderProgram);
    
    if (shaderType == ShaderType::Particle) {
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

ShaderProgram& Renderer::GetShaderProgram(ShaderType shaderType) {
    switch (shaderType) {
        case ShaderType::PixelLighting:
            return mPixelLightingShader;
        case ShaderType::VertexLighting:
            return mVertexLightingShader;
        case ShaderType::TexturedLighting:
            return mTexturedLightingShader; 
        case ShaderType::Textured:
            return mTexturedShader;
        case ShaderType::CubeMap:
            return mCubeMapShader;
        case ShaderType::VertexColor:
            return mVertexColorShader;
        case ShaderType::Particle:
            return mParticleShader;
    }
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
        if (sceneObject->IsVisible()) {
            Render(sceneObject->GetRenderable(), sceneObject->GetMatrix() * view.GetMatrix());
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

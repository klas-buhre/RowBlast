#include "ParticleEffect.hpp"

#include "MathUtils.hpp"
#include "Fnv1Hash.hpp"
#include "IParticleSystem.hpp"
#include "SceneObject.hpp"

using namespace Pht;

namespace {
    Vec3 RotateZ(const Vec3 v, float sinTheta, float cosTheta) {
        return {v.x * cosTheta - v.y * sinTheta, v.x * sinTheta + v.y * cosTheta, 0.0f};
    }
    
    bool UpdateParticle(Particle& particle, float dt, const ParticleSettings& particleSettings) {
        particle.mAge += dt;
        
        if (particle.mAge > particle.mTimeToLive) {
            particle.mIsActive = false;
            return false;
        }
        
        particle.mVelocity += particleSettings.mAcceleration * dt;
        
        if (particleSettings.mDragCoefficient.HasValue()) {
            auto dragCoeff {particleSettings.mDragCoefficient.GetValue()};
            particle.mVelocity -= particle.mVelocity * dragCoeff * dt;
        }

        particle.mPosition += particle.mVelocity * dt;
        particle.mZAngle += particle.mZAngularVelocity * dt;
        
        if (particle.mAge < particleSettings.mGrowDuration) {
            if (particleSettings.mInitialSize.HasValue()) {
                particle.mSize += (particle.mFullSize - particleSettings.mInitialSize.GetValue()) *
                                  dt / particleSettings.mGrowDuration;
            } else if (particleSettings.mInitialPointSize.HasValue()) {
                auto deltaSize {
                    (particle.mFullSize.x - particleSettings.mInitialPointSize.GetValue()) *
                    dt / particleSettings.mGrowDuration
                };
                
                particle.mSize += {deltaSize, deltaSize};
            } else {
                particle.mSize += particle.mFullSize * dt / particleSettings.mGrowDuration;
            }
            
            if (particle.mSize.x > particle.mFullSize.x) {
                particle.mSize.x = particle.mFullSize.x;
            }
            
            if (particle.mSize.y > particle.mFullSize.y) {
                particle.mSize.y = particle.mFullSize.y;
            }
        } else if (particle.mAge > particle.mTimeToLive - particleSettings.mShrinkDuration) {
            auto deltaSize {particle.mFullSize * dt / particleSettings.mShrinkDuration};
            
            Vec2 scaledDeltaSize {
                deltaSize.x * particleSettings.mShrinkScale.x,
                deltaSize.y * particleSettings.mShrinkScale.y
            };
            
            particle.mSize -= scaledDeltaSize;
            
            if (particle.mSize.x < 0.0f) {
                particle.mSize.x = 0.0f;
            }
            
            if (particle.mSize.y < 0.0f) {
                particle.mSize.y = 0.0f;
            }
        } else {
            particle.mSize = particle.mFullSize;
        }

        if (particle.mAge > particle.mTimeToLive - particleSettings.mFadeOutDuration) {
            particle.mColor.w -= dt / particleSettings.mFadeOutDuration;
            
            if (particle.mColor.w < 0.0f) {
                particle.mColor.w = 0.0f;
            }
        }

        return true;
    }
    
    int CalculateNumParticles(const EmitterSettings& emitterSettings,
                              const ParticleSettings& particleSettings) {
        return emitterSettings.mFrequency *
               (particleSettings.mTimeToLive + particleSettings.mTimeToLiveRandomPart) * 2.0f +
               emitterSettings.mBurst;
    }
}

const ComponentId ParticleEffect::id {Hash::Fnv1a("ParticleEffect")};

ParticleEffect::ParticleEffect(SceneObject& sceneObject,
                               IParticleSystem& particleSystem,
                               const ParticleSettings& particleSettings,
                               const EmitterSettings& emitterSettings,
                               RenderMode renderMode) :
    mSceneObject {sceneObject},
    mParticleSystem {particleSystem},
    mEmitter {particleSettings, emitterSettings},
    mRenderMode {renderMode} {
    
    auto numParticles {CalculateNumParticles(emitterSettings, particleSettings)};
    mParticles.resize(numParticles);
    
    Material material {particleSettings.mTextureFilename};
    material.SetBlend(Blend::Yes);
    material.GetDepthState().mDepthTest = false;
    
    switch (renderMode) {
        case RenderMode::Points: {
            VertexFlags vertexFlags {.mColors = true, .mPointSizes = true};
            mVertexBuffer = std::make_unique<VertexBuffer>(numParticles, 0, vertexFlags);
            material.SetShaderType(ShaderType::PointParticle);
            mRenderableObject = std::make_unique<RenderableObject>(material, RenderMode::Points);
            break;
        }
        case RenderMode::Triangles: {
            VertexFlags vertexFlags {.mTextureCoords = true, .mColors = true};
            mVertexBuffer = std::make_unique<VertexBuffer>(numParticles * 4,
                                                           numParticles * 6,
                                                           vertexFlags);
            material.SetShaderType(ShaderType::Particle);
            mRenderableObject = std::make_unique<RenderableObject>(material, RenderMode::Triangles);
            break;
        }
    }
    
    sceneObject.SetRenderable(mRenderableObject.get());
    mSceneObject.SetIsVisible(false);
}

ParticleEffect::~ParticleEffect() {
    mParticleSystem.RemoveParticleEffect(*this);
}

void ParticleEffect::Start() {
    mEmitter.Start();
    mIsActive = true;
    
    for (auto& particle: mParticles) {
        particle.mIsActive = false;
    }
    
    mSceneObject.SetIsVisible(true);
}

void ParticleEffect::Stop() {
    mIsActive = false;
    mSceneObject.SetIsVisible(false);
}

void ParticleEffect::Update(float dt) {
    if (!mIsActive) {
        return;
    }

    mEmitter.Update(dt, mParticles);
    auto anyActiveParticles {false};
    auto& particleSettings {mEmitter.GetParticleSettings()};
    
    for (auto& particle: mParticles) {
        if (!particle.mIsActive) {
            continue;
        }
        
        if (UpdateParticle(particle, dt, particleSettings)) {
            anyActiveParticles = true;
        }
    }
    
    if (!anyActiveParticles) {
        Stop();
    } else {
        WriteVertexBuffer();
    }
}

void ParticleEffect::WriteVertexBuffer() {
    mVertexBuffer->Reset();
    
    switch (mRenderMode) {
        case RenderMode::Points:
            WritePoints();
            break;
        case RenderMode::Triangles:
            WriteTriangles();
            break;
    }
}

void ParticleEffect::WritePoints() {
    for (auto& particle: mParticles) {
        if (particle.mIsActive) {
            mVertexBuffer->Write(particle.mPosition, particle.mColor, particle.mSize.x);
        }
    }
    
    mRenderableObject->UploadPoints(*mVertexBuffer, BufferUsage::DynamicDraw);
}

void ParticleEffect::WriteTriangles() {
    for (auto& particle: mParticles) {
        if (particle.mIsActive) {
            WriteParticleTriangles(particle);
        }
    }
    
    mRenderableObject->UploadTriangles(*mVertexBuffer, BufferUsage::DynamicDraw);
}

void ParticleEffect::WriteParticleTriangles(const Particle& particle) {
    auto& color {particle.mColor};
    auto halfSize {particle.mSize / 2.0f};
    
    Vec3 v1 {-halfSize.x, -halfSize.y, 0.0f};
    Vec3 v2 {halfSize.x, -halfSize.y, 0.0f};
    Vec3 v3 {halfSize.x, halfSize.y, 0.0f};
    Vec3 v4 {-halfSize.x, halfSize.y, 0.0f};
    
    if (particle.mZAngle != 0.0f) {
        auto thetaRadians {ToRadians(particle.mZAngle)};
        auto sinTheta {std::sin(thetaRadians)};
        auto cosTheta {std::cos(thetaRadians)};
        
        v1 = RotateZ(v1, sinTheta, cosTheta);
        v2 = RotateZ(v2, sinTheta, cosTheta);
        v3 = RotateZ(v3, sinTheta, cosTheta);
        v4 = RotateZ(v4, sinTheta, cosTheta);
    }

    auto& position {particle.mPosition};
    
    mVertexBuffer->BeginFace();

    mVertexBuffer->Write(v1 + position, Vec2 {0.0f, 1.0f}, color);
    mVertexBuffer->Write(v2 + position, Vec2 {1.0f, 1.0f}, color);
    mVertexBuffer->Write(v3 + position, Vec2 {1.0f, 0.0f}, color);
    mVertexBuffer->Write(v4 + position, Vec2 {0.0f, 0.0f}, color);
    
    mVertexBuffer->AddIndex(0);
    mVertexBuffer->AddIndex(1);
    mVertexBuffer->AddIndex(2);
    mVertexBuffer->AddIndex(2);
    mVertexBuffer->AddIndex(3);
    mVertexBuffer->AddIndex(0);
}

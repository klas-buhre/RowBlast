#include "ParticleSystem.hpp"

#include "IEngine.hpp"
#include "MathUtils.hpp"

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
        particle.mPosition += particle.mVelocity * dt;
        particle.mZAngle += particle.mZAngularVelocity * dt;
        
        if (particle.mAge < particleSettings.mGrowDuration) {
            particle.mSize += particle.mFullSize * dt / particleSettings.mGrowDuration;
            
            if (particle.mSize > particle.mFullSize) {
                particle.mSize = particle.mFullSize;
            }
        } else if (particle.mAge > particle.mTimeToLive - particleSettings.mShrinkDuration) {
            particle.mSize -= particle.mFullSize * dt / particleSettings.mShrinkDuration;
            if (particle.mSize < 0.0f) {
                particle.mSize = 0.0f;
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

ParticleSystem::ParticleSystem(IEngine& engine,
                               const ParticleSettings& particleSettings,
                               const EmitterSettings& emitterSettings,
                               RenderMode renderMode) :
    mEngine {engine},
    mEmitter {engine, particleSettings, emitterSettings},
    mRenderMode {renderMode} {
    
    auto numParticles {CalculateNumParticles(emitterSettings, particleSettings)};
    mParticles.resize(numParticles);
    
    Material material {particleSettings.mTextureFilename};
    
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
}

void ParticleSystem::Start() {
    mEmitter.Start();
    mIsActive = true;
    
    for (auto& particle: mParticles) {
        particle.mIsActive = false;
    }
}

bool ParticleSystem::Update() {
    if (!mIsActive) {
        return false;
    }

    mEmitter.Update(mParticles);
    auto dt {mEngine.GetLastFrameSeconds()};
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
        mIsActive = false;
    } else {
        WriteVertexBuffer();
    }
    
    return mIsActive;
}

const RenderableObject* ParticleSystem::GetRenderableObject() const {
    if (mIsActive) {
        return mRenderableObject.get();
    }
    
    return nullptr;
}

void ParticleSystem::WriteVertexBuffer() {
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

void ParticleSystem::WritePoints() {
    for (auto& particle: mParticles) {
        if (particle.mIsActive) {
            mVertexBuffer->Write(particle.mPosition, particle.mColor, particle.mSize);
        }
    }
    
    mRenderableObject->UploadPoints(*mVertexBuffer, BufferUsage::DynamicDraw);
}

void ParticleSystem::WriteTriangles() {
    for (auto& particle: mParticles) {
        if (particle.mIsActive) {
            WriteParticleTriangles(particle);
        }
    }
    
    mRenderableObject->UploadTriangles(*mVertexBuffer, BufferUsage::DynamicDraw);
}

void ParticleSystem::WriteParticleTriangles(const Particle& particle) {
    auto& color {particle.mColor};
    auto halfSize {particle.mSize / 2.0f};
    
    Vec3 v1 {-halfSize, -halfSize, 0.0f};
    Vec3 v2 {halfSize, -halfSize, 0.0f};
    Vec3 v3 {halfSize, halfSize, 0.0f};
    Vec3 v4 {-halfSize, halfSize, 0.0f};
    
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

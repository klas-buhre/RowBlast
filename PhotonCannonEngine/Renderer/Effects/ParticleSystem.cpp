#include "ParticleSystem.hpp"

#include "IEngine.hpp"

using namespace Pht;

namespace {
    bool UpdateParticle(Particle& particle, float dt, const ParticleSettings& particleSettings) {
        particle.mAge += dt;
        
        if (particle.mAge > particle.mTimeToLive) {
            particle.mIsActive = false;
            return false;
        }
        
        particle.mVelocity += particleSettings.mAcceleration * dt;
        particle.mPosition += particle.mVelocity * dt;
        
        if (particle.mAge > particle.mTimeToLive - particleSettings.mFadeOutDuration) {
            particle.mColor.w -= dt / particleSettings.mFadeOutDuration;
            if (particle.mColor.w < 0.0f) {
                particle.mColor.w = 0.0f;
            }
        }
        
        if (particle.mAge > particle.mTimeToLive - particleSettings.mShrinkDuration) {
            particle.mSize -= particle.mOriginalSize * dt / particleSettings.mShrinkDuration;
            if (particle.mSize < 0.0f) {
                particle.mSize = 0.0f;
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
//    auto& color {particle.mColor};

    mVertexBuffer->BeginFace();
/*
    mVertexBuffer->Write(const Vec3& vertex, const Vec2& textureCoord, color);
    mVertexBuffer->Write(mVertices.mV1.mVertex, mVertices.mV1.mColor);
    mVertexBuffer->Write(mVertices.mV2.mVertex, mVertices.mV2.mColor);
    mVertexBuffer->Write(mVertices.mV3.mVertex, mVertices.mV3.mColor);
*/
    mVertexBuffer->AddIndex(0);
    mVertexBuffer->AddIndex(1);
    mVertexBuffer->AddIndex(2);
    mVertexBuffer->AddIndex(2);
    mVertexBuffer->AddIndex(3);
    mVertexBuffer->AddIndex(0);
}

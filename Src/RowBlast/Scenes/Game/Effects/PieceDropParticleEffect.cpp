#include "PieceDropParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "SceneObject.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"

// Game includes.
#include "GameScene.hpp"
#include "FallingPiece.hpp"

using namespace RowBlast;

namespace {
    constexpr auto emitterHeightInCells = 3.0f;
    constexpr auto emitterWidthInCells = 0.5f;
}

PieceDropParticleEffect::PieceDropParticleEffect(Pht::IEngine& engine, GameScene& scene) :
    mScene {scene},
    mContainerSceneObject {std::make_unique<Pht::SceneObject>()} {
    
    CreateParticleEffects(engine);
}

void PieceDropParticleEffect::CreateParticleEffects(Pht::IEngine& engine) {
    auto cellSize = mScene.GetCellSize();
    
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, emitterHeightInCells * cellSize / 2.0f + cellSize / 2.0f, 0.0f},
        .mSize = Pht::Vec3{emitterWidthInCells * cellSize, emitterHeightInCells * cellSize, 0.0f},
        .mTimeToLive = 0.0f,
        .mBurst = 5
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 1.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 1.0f, 0.0f},
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.1f, 0.1f, 0.1f, 0.0f},
        .mTextureFilename = "particle_sprite_twinkle_blurred.png",
        .mTimeToLive = 0.7f,
        .mTimeToLiveRandomPart = 0.4f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocityRandomPart = 800.0f,
        .mSize = Pht::Vec2{1.42f, 1.42f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.4f
    };
    
    auto& particleSystem = engine.GetParticleSystem();
    
    for (auto& effect: mParticleEffects) {
        effect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
        mContainerSceneObject->AddChild(*effect);
    }
}

void PieceDropParticleEffect::Init() {
    mScene.GetPieceDropEffectsContainer().AddChild(*mContainerSceneObject);
}

void PieceDropParticleEffect::StartEffect(const FallingPiece& fallingPiece) {
    auto& pieceType = fallingPiece.GetPieceType();
    auto pieceNumRows = pieceType.GetGridNumRows();
    auto pieceNumColumns = pieceType.GetGridNumColumns();
    auto cellSize = mScene.GetCellSize();
    auto pieceFieldPos = fallingPiece.GetRenderablePosition() * cellSize;
    auto& pieceGrid = pieceType.GetGrid(fallingPiece.GetRotation());
    auto cellZPos = mScene.GetFieldPosition().z;
    
    for (auto column = 0; column < pieceNumColumns; ++column) {
        for (auto row = pieceNumRows - 1; row >= 0; --row) {
            if (!pieceGrid[row][column].IsEmpty()) {
                Pht::Vec3 fieldPosition {
                    column * cellSize + cellSize / 2.0f + pieceFieldPos.x,
                    row * cellSize + pieceFieldPos.y,
                    cellZPos
                };
                
                StartParticleEffect(fieldPosition);
                break;
            }
        }
    }
}

void PieceDropParticleEffect::StartParticleEffect(const Pht::Vec3& fieldPosition) {
    for (auto& effectSceneObject: mParticleEffects) {
        auto* effect = effectSceneObject->GetComponent<Pht::ParticleEffect>();
        if (!effect->IsActive()) {
            effect->Start();
            effectSceneObject->GetTransform().SetPosition(fieldPosition);
            break;
        }
    }
}

void PieceDropParticleEffect::Update(float dt) {
    for (auto& effect: mParticleEffects) {
        effect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    }
}

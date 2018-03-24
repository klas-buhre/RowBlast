#include "PieceDropParticleEffect.hpp"

// Game includes.
#include "GameScene.hpp"
#include "FallingPiece.hpp"
#include "IEngine.hpp"
#include "SceneObject.hpp"
#include "ParticleSystem.hpp"
#include "ParticleEffect.hpp"

using namespace BlocksGame;

PieceDropParticleEffect::PieceDropParticleEffect(Pht::IEngine& engine, GameScene& scene) :
    mScene {scene},
    mContainerSceneObject {std::make_unique<Pht::SceneObject>()} {
    
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 1.0f, 0.0f},
        .mSize = Pht::Vec3{scene.GetCellSize(), 2.0f, 1.0f},
        .mTimeToLive = 0.0f,
        .mBurst = 3
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
        .mZAngularVelocityRandomPart = 400.0f,
        .mSize = 2.42f,
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.4f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    
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
    auto& pieceType {fallingPiece.GetPieceType()};
    auto pieceNumRows {pieceType.GetGridNumRows()};
    auto pieceNumColumns {pieceType.GetGridNumColumns()};
    auto cellSize {mScene.GetCellSize()};
    auto pieceFieldPos {fallingPiece.GetRenderablePosition() * cellSize};
    auto& pieceGrid {pieceType.GetGrid(fallingPiece.GetRotation())};
    auto cellZPos {mScene.GetFieldPosition().z};
    
    for (auto column {0}; column < pieceNumColumns; ++column) {
        for (auto row {pieceNumRows - 1}; row >= 0; --row) {
            if (!pieceGrid[row][column].IsEmpty()) {
                Pht::Vec3 fieldPosition {
                    column * cellSize + cellSize / 2.0f + pieceFieldPos.x,
                    row * cellSize + cellSize + pieceFieldPos.y,
                    cellZPos
                };
                
                StartEffect(fieldPosition);
                break;
            }
        }
    }
}

void PieceDropParticleEffect::StartEffect(const Pht::Vec3& fieldPosition) {
    for (auto& effectSceneObject: mParticleEffects) {
        auto* effect {effectSceneObject->GetComponent<Pht::ParticleEffect>()};
        
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

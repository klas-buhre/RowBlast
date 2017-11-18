#include "PieceDropParticleEffect.hpp"

// Game includes.
#include "GameScene.hpp"
#include "FallingPiece.hpp"
#include "IEngine.hpp"
#include "SceneObject.hpp"
#include "ParticleSystem.hpp"
#include "ParticleEffect.hpp"

using namespace BlocksGame;

PieceDropParticleEffect::PieceDropParticleEffect(Pht::IEngine& engine, const GameScene& scene) :
    mScene {scene} {
    
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 1.0f, 0.0f},
        .mSize = Pht::Vec3{scene.GetCellSize(), 2.0f, 1.0f},
        .mTimeToLive = 0.0f,
        .mBurst = 5
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 1.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 1.0f, 0.0f},
        .mColor = Pht::Vec4{0.5f, 0.5f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.1f, 0.1f, 0.1f, 0.0f},
        .mTextureFilename = "star_particle.png",
        .mTimeToLive = 0.7f,
        .mTimeToLiveRandomPart = 0.4f,
        .mFadeOutDuration = 0.4f,
        .mZAngularVelocityRandomPart = 350.0f,
        .mSize = 1.0f,
        .mSizeRandomPart = 1.0f,
        .mShrinkDuration = 0.4f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    
    for (auto& effect: mParticleEffects) {
        effect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
    }
}

void PieceDropParticleEffect::StartEffect(const FallingPiece& fallingPiece) {
    auto& pieceType {fallingPiece.GetPieceType()};
    auto pieceNumRows {pieceType.GetGridNumRows()};
    auto pieceNumColumns {pieceType.GetGridNumColumns()};
    auto cellSize {mScene.GetCellSize()};
    auto fieldLowerLeft {mScene.GetFieldLoweLeft()};
    auto pieceWorldPos {fieldLowerLeft + fallingPiece.GetRenderablePosition() * cellSize};
    auto& pieceGrid {pieceType.GetGrid(fallingPiece.GetRotation())};
    auto cellZPos {mScene.GetFieldPosition().z};
    auto colorBrighten {0.8f};
    
    auto& materialColor {pieceType.GetFirstRenderable().GetMaterial().GetAmbient()};
    Pht::Vec4 color {
        materialColor.mRed + colorBrighten,
        materialColor.mGreen + colorBrighten,
        materialColor.mBlue + colorBrighten,
        1.0f
    };
    
    for (auto column {0}; column < pieceNumColumns; ++column) {
        for (auto row {pieceNumRows - 1}; row >= 0; --row) {
            if (!pieceGrid[row][column].IsEmpty()) {
                Pht::Vec3 scenePosition {
                    column * cellSize + cellSize / 2.0f + pieceWorldPos.x,
                    row * cellSize + cellSize + pieceWorldPos.y,
                    cellZPos
                };
                
                StartEffect(scenePosition, color);
                break;
            }
        }
    }
}

void PieceDropParticleEffect::StartEffect(const Pht::Vec3& scenePosition, const Pht::Vec4& color) {
    for (auto& effectSceneObject: mParticleEffects) {
        auto* effect {effectSceneObject->GetComponent<Pht::ParticleEffect>()};
        
        if (!effect->IsActive()) {
            auto& particleSettings {effect->GetEmitter().GetParticleSettings()};
            particleSettings.mColor = color;
            
            effect->Start();
            effectSceneObject->ResetTransform();
            effectSceneObject->Translate(scenePosition);
            break;
        }
    }
}

void PieceDropParticleEffect::Update(float dt) {
    for (auto& effect: mParticleEffects) {
        effect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    }
}

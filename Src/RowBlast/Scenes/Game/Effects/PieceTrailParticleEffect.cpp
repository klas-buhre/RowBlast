#include "PieceTrailParticleEffect.hpp"

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
    constexpr auto trailYScale = 1.5f;
    constexpr auto trailHeightInCells = 4.5f * trailYScale;
    constexpr auto lowerTrailHeightInCells = 1.0f * trailYScale;
    constexpr auto trailWidthInCells = 1.075f;
    constexpr auto trailDuration = 0.7f;
}

PieceTrailParticleEffect::PieceTrailParticleEffect(Pht::IEngine& engine, GameScene& scene) :
    mScene {scene},
    mContainerSceneObject {std::make_unique<Pht::SceneObject>()} {
    
    CreateTrailEffects(engine, "block_trail_blue.png", mBlueTrailEffects);
    CreateTrailEffects(engine, "block_trail_green.png", mGreenTrailEffects);
    CreateTrailEffects(engine, "block_trail_red.png", mRedTrailEffects);
    CreateTrailEffects(engine, "block_trail_yellow.png", mYellowTrailEffects);
}

void PieceTrailParticleEffect::CreateTrailEffects(Pht::IEngine& engine,
                                                  const std::string& textureFilename,
                                                  TrailEffects& trailEffects) {
    auto cellSize = mScene.GetCellSize();
    
    Pht::Vec3 emitterPosition {
        0.0f, trailHeightInCells * cellSize / 2.0f - lowerTrailHeightInCells * cellSize, -0.1f
    };
    
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = emitterPosition,
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };
    
    Pht::ParticleSettings particleSettings {
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 0.5f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = textureFilename,
        .mTimeToLive = trailDuration,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = trailDuration,
        .mSize = Pht::Vec2{cellSize * trailWidthInCells, cellSize * trailHeightInCells},
        .mSizeRandomPart = 0.0f,
        .mGrowDuration = 0.0f,
        .mShrinkDuration = 0.0f
    };
    
    auto& particleSystem = engine.GetParticleSystem();
    
    for (auto& effect: trailEffects) {
        effect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
        effect->GetRenderable()->GetMaterial().SetShaderId(Pht::ShaderId::ParticleTextureColor);
        mContainerSceneObject->AddChild(*effect);
    }
}

void PieceTrailParticleEffect::Init() {
    mScene.GetPieceDropEffectsContainer().AddChild(*mContainerSceneObject);
}

void PieceTrailParticleEffect::StartEffect(const FallingPiece& fallingPiece) {
    auto& pieceType = fallingPiece.GetPieceType();
    auto pieceNumRows = pieceType.GetGridNumRows();
    auto pieceNumColumns = pieceType.GetGridNumColumns();
    auto cellSize = mScene.GetCellSize();
    auto pieceFieldPos = fallingPiece.GetRenderablePosition() * cellSize;
    auto& pieceGrid = pieceType.GetGrid(fallingPiece.GetRotation());
    auto cellZPos = mScene.GetFieldPosition().z;
    
    for (auto column = 0; column < pieceNumColumns; ++column) {
        for (auto row = pieceNumRows - 1; row >= 0; --row) {
            auto& cell = pieceGrid[row][column];

            if (!cell.IsEmpty()) {
                Pht::Vec3 fieldPosition {
                    column * cellSize + cellSize / 2.0f + pieceFieldPos.x,
                    row * cellSize + cellSize + pieceFieldPos.y,
                    cellZPos
                };
                
                auto fill = cell.mFirstSubCell.mFill;
                
                if (fill == Fill::LowerLeftHalf || fill == Fill::LowerRightHalf) {
                    fieldPosition.y -= cellSize;
                }
                
                switch (cell.mFirstSubCell.mColor) {
                    case BlockColor::Blue:
                        StartTrailEffect(fieldPosition, mBlueTrailEffects);
                        break;
                    case BlockColor::Green:
                        StartTrailEffect(fieldPosition, mGreenTrailEffects);
                        break;
                    case BlockColor::Red:
                        StartTrailEffect(fieldPosition, mRedTrailEffects);
                        break;
                    case BlockColor::Yellow:
                        StartTrailEffect(fieldPosition, mYellowTrailEffects);
                        break;
                    case BlockColor::None:
                        break;
                }

                break;
            }
        }
    }
}

void PieceTrailParticleEffect::StartTrailEffect(const Pht::Vec3& fieldPosition,
                                                TrailEffects& trailEffects) {
    for (auto& effectSceneObject: trailEffects) {
        auto* effect = effectSceneObject->GetComponent<Pht::ParticleEffect>();
        if (!effect->IsActive()) {
            effect->Start();
            effectSceneObject->GetTransform().SetPosition(fieldPosition);
            break;
        }
    }
}

void PieceTrailParticleEffect::Update(float dt) {
    UpdateTrailEffects(dt, mBlueTrailEffects);
    UpdateTrailEffects(dt, mGreenTrailEffects);
    UpdateTrailEffects(dt, mRedTrailEffects);
    UpdateTrailEffects(dt, mYellowTrailEffects);
}

void PieceTrailParticleEffect::UpdateTrailEffects(float dt, TrailEffects& trailEffects) {
    for (auto& effect: trailEffects) {
        effect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    }
}

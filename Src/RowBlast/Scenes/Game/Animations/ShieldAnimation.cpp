#include "ShieldAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "Level.hpp"

using namespace RowBlast;

namespace {
    constexpr auto shieldWidthInCells = 9.2f;
    constexpr auto shieldHeightInCells = 2.5f;
    constexpr auto shieldOpacity = 0.275f;
    constexpr auto flashOpacity = 1.0f;
    constexpr auto flashDuration = 0.57f;
    constexpr auto pulseAmplitude = 0.05f;
    constexpr auto pulseDuration = 1.25f;
    constexpr auto appearTime = 0.4f;
    constexpr auto inactiveRelativeYPosition = 13.3f;
}

ShieldAnimation::ShieldAnimation(Pht::IEngine& engine,
                                 GameScene& scene,
                                 const ScrollController& scrollController) :
    mScene {scene},
    mScrollController {scrollController} {

    Pht::Material shieldMaterial {"shield.png"};
    shieldMaterial.SetBlend(Pht::Blend::Additive);
    shieldMaterial.GetDepthState().mDepthTest = false;
    shieldMaterial.SetOpacity(shieldOpacity);
    
    auto& sceneManager = engine.GetSceneManager();
    auto cellSize = scene.GetCellSize();
    auto shieldWidth = cellSize * shieldWidthInCells;
    auto shieldHeight = cellSize * shieldHeightInCells;
    mShieldRenderable = sceneManager.CreateRenderableObject(Pht::QuadMesh {shieldWidth, shieldHeight},
                                                            shieldMaterial);

    mShieldSceneObject = std::make_unique<Pht::SceneObject>(mShieldRenderable.get());
}

void ShieldAnimation::Init(const Level& level) {
    mLevel = &level;

    if (level.GetObjective() != Level::Objective::BringDownTheAsteroid) {
        return;
    }
    
    mScene.GetFieldBlocksContainer().AddChild(*mShieldSceneObject);
    mState = State::Inactive;
    mShieldSceneObject->SetIsVisible(false);
}

void ShieldAnimation::Start() {
    mState = State::Appearing;
    mElapsedTime = 0.0f;
    mShieldSceneObject->SetIsVisible(true);
}

void ShieldAnimation::StartFlash() {
    mState = State::Flashing;
    mElapsedTime = 0.0f;
}

void ShieldAnimation::Update(float dt) {
    if (mLevel->GetObjective() != Level::Objective::BringDownTheAsteroid) {
        return;
    }
    
    switch (mState) {
        case State::Appearing:
            UpdateInAppearingState(dt);
            break;
        case State::Pulsating:
            UpdateInPulsatingState(dt);
            break;
        case State::Flashing:
            UpdateInFlashingState(dt);
            break;
        case State::Inactive:
            break;
    }
}

void ShieldAnimation::UpdateInAppearingState(float dt) {
    mElapsedTime += dt;

    auto normalizedTime = mElapsedTime / appearTime;
    mShieldRenderable->GetMaterial().SetOpacity(normalizedTime * shieldOpacity);
    
    auto yDiff = inactiveRelativeYPosition - shieldRelativeYPosition;
    mShieldRelativeYPosition = shieldRelativeYPosition + (1.0f - normalizedTime) * yDiff;
    auto& transform = mShieldSceneObject->GetTransform();
    transform.SetScale({1.0f, normalizedTime, 1.0f});
    
    if (mElapsedTime > appearTime) {
        transform.SetScale(1.0f);
        mShieldRenderable->GetMaterial().SetOpacity(shieldOpacity);
        mShieldRelativeYPosition = shieldRelativeYPosition;
        GoToPulsatingState();
    }
    
    UpdatePosition();
}

void ShieldAnimation::UpdatePosition() {
    auto cellSize = mScene.GetCellSize();
    auto lowestVisibleRow = mScrollController.GetLowestVisibleRow();
    
    Pht::Vec3 positionInField {
        mScene.GetFieldWidth() / 2.0f,
        (lowestVisibleRow + mShieldRelativeYPosition + shieldHeightInCells / 2.0f) * cellSize,
        mScene.GetShieldAnimationZ()
    };

    mShieldSceneObject->GetTransform().SetPosition(positionInField);
}

void ShieldAnimation::UpdateInPulsatingState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > pulseDuration) {
        mElapsedTime = 0.0f;
    }

    auto sineOfT = std::sin(mElapsedTime * 2.0f * 3.1415f / pulseDuration);
    auto opactiy = shieldOpacity + pulseAmplitude * sineOfT;
    mShieldRenderable->GetMaterial().SetOpacity(opactiy);
    
    UpdatePosition();
}

void ShieldAnimation::UpdateInFlashingState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > flashDuration) {
        mShieldRenderable->GetMaterial().SetOpacity(shieldOpacity);
        GoToPulsatingState();
    } else {
        auto normalizedTime = (flashDuration - mElapsedTime) / flashDuration;
        auto opacityDiff = flashOpacity - shieldOpacity;
        auto opacity = shieldOpacity + normalizedTime * opacityDiff;
        
        mShieldRenderable->GetMaterial().SetOpacity(opacity);
    }
    
    UpdatePosition();
}

void ShieldAnimation::GoToPulsatingState() {
    mState = State::Pulsating;
    mElapsedTime = 0.0f;
}

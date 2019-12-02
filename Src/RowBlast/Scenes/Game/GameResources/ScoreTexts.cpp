#include "ScoreTexts.hpp"

// Engine includes.
#include "TextComponent.hpp"
#include "MathUtils.hpp"
#include "Scene.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "StringUtils.hpp"

using namespace RowBlast;

namespace {
    constexpr auto numScoreTexts = 30;
    constexpr auto waitDuration = 0.075f;
    constexpr auto scaleUpDuration = 0.27f;
    constexpr auto displayTextDuration = 0.43f;
    constexpr auto scaleDownDuration = 0.27f;
    constexpr auto slideDistance = 0.55f;
    constexpr auto slideVelocity = slideDistance / (scaleUpDuration + displayTextDuration + scaleDownDuration);
    constexpr auto textShadowAlpha = 0.35f;
    constexpr auto textScale = 0.56f;
    
    Pht::StaticVector<Pht::Vec2, 20> scalePoints {
        {0.0f, 0.0f},
        {0.1f, 0.005f},
        {0.2f, 0.01f},
        {0.3f, 0.02f},
        {0.35f, 0.035f},
        {0.4f, 0.05f},
        {0.45f, 0.065f},
        {0.5f, 0.08f},
        {0.55f, 0.115f},
        {0.6f, 0.15f},
        {0.65f, 0.225f},
        {0.7f, 0.3f},
        {0.75f, 0.41f},
        {0.8f, 0.52f},
        {0.85f, 0.62f},
        {0.9f, 0.7f},
        {0.95f, 0.87f},
        {1.0f, 1.0f},
    };
}

ScoreTexts::ScoreTexts(GameScene& scene,
                       const CommonResources& commonResources,
                       SceneId sceneId) :
    mScene {scene},
    mSceneId {sceneId} {

    switch (sceneId) {
        case SceneId::Game:
            mScoreTexts.resize(numScoreTexts);
            break;
        case SceneId::LevelCompletedSubScene:
            mScoreTexts.resize(1);
            break;
    }
    
    for (auto& scoreText: mScoreTexts) {
        scoreText = std::make_unique<ScoreText>(scene, commonResources);
    }
}

void ScoreTexts::Init() {
    auto& container = GetContainer();
    for (auto& scoreText: mScoreTexts) {
        scoreText->Init(container);
    }
}

void ScoreTexts::Start(int numPoints, const Pht::Vec2& position) {
    Start(numPoints, position, waitDuration);
}

void ScoreTexts::Start(int numPoints, const Pht::Vec2& position, float delay) {
    for (auto& scoreText: mScoreTexts) {
        if (scoreText->IsInactive()) {
            scoreText->Start(numPoints, position, delay);
            return;
        }
    }
    
    mScoreTexts.front()->Start(numPoints, position, delay);
}

void ScoreTexts::Update(float dt) {
    for (auto& scoreText: mScoreTexts) {
        scoreText->Update(dt);
    }
}

bool ScoreTexts::IsInactive() const {
    for (auto& scoreText: mScoreTexts) {
        if (!scoreText->IsInactive()) {
            return false;
        }
    }
    
    return true;
}

Pht::SceneObject& ScoreTexts::GetContainer() {
    switch (mSceneId) {
        case SceneId::Game:
            return mScene.GetScoreTextContainer();
        case SceneId::LevelCompletedSubScene:
            return mScene.GetHud().GetMovesContainer();
    }
}

ScoreTexts::ScoreText::ScoreText(GameScene& scene, const CommonResources& commonResources) :
    mScene {scene} {
        
    auto& font = commonResources.GetHussarFontSize52PotentiallyZoomedScreen();

    Pht::TextProperties textProperties {
        font,
        textScale,
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.105f, 0.105f},
        {0.0f, 0.0f, 0.0f, textShadowAlpha}
    };
    textProperties.mAlignment = Pht::TextAlignment::CenterX;
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;
    textProperties.mItalicSlant = 0.15f;
    textProperties.mTopGradientColorSubtraction = Pht::Vec3 {0.13f, 0.26f, 0.39f};
    textProperties.mSpecular = Pht::TextSpecular::Yes;
    textProperties.mSpecularOffset = {0.045f, 0.045f};
    
    mSceneObject = std::make_unique<Pht::SceneObject>();
    mTextSceneObject = std::make_unique<Pht::SceneObject>();
    mTextSceneObject->GetTransform().SetPosition({0.0f, -0.5f, UiLayer::text});
    
    std::string digitsPlaceHolder {"     "}; // Warning! Must be five spaces to fit digits.
        
    auto textComponent =
        std::make_unique<Pht::TextComponent>(*mTextSceneObject, digitsPlaceHolder, textProperties);

    mTextSceneObject->SetComponent(std::move(textComponent));
    mSceneObject->AddChild(*mTextSceneObject);
}

void ScoreTexts::ScoreText::Init(Pht::SceneObject& parentObject) {
    parentObject.AddChild(*mSceneObject);
    mSceneObject->SetIsVisible(false);
    mSceneObject->SetIsStatic(true);
}

void ScoreTexts::ScoreText::Start(int numPoints, const Pht::Vec2& position, float delay) {
    mSceneObject->SetIsStatic(false);

    auto* textComponent = mTextSceneObject->GetComponent<Pht::TextComponent>();
    assert(textComponent);
    auto& scoreText = textComponent->GetText();
    StringUtils::FillStringWithSpaces(scoreText);
    StringUtils::WriteIntegerToString(numPoints, scoreText, numPoints < 100 ? 1 : 0);

    auto& textProperties = textComponent->GetProperties();
    textProperties.mColor.w = 1.0f;
    textProperties.mSpecularColor.w = 1.0f;
    textProperties.mShadowColor.w = textShadowAlpha;
    
    mState = State::Waiting;
    mElapsedTime = 0.0f;
    mDelay = delay;
    
    const auto cellSize = mScene.GetCellSize();

    Pht::Vec3 scenePosition {
        position.x * cellSize + cellSize / 2.0f,
        position.y * cellSize + cellSize / 2.0f,
        UiLayer::text
    };
    
    mSceneObject->GetTransform().SetPosition(scenePosition);
    Pht::SceneObjectUtils::ScaleRecursively(*mSceneObject, 0.0f);
}

void ScoreTexts::ScoreText::Update(float dt) {
    switch (mState) {
        case State::Waiting:
            UpdateInWaitingState(dt);
            break;
        case State::ScalingUp:
            UpdateInScalingUpState(dt);
            break;
        case State::SlidingUp:
            UpdateInSlidingUpState(dt);
            break;
        case State::SlidingUpFadingOutAndScalingDown:
            UpdateInSlidingUpFadingOutAndScalingDownState(dt);
            break;
        case State::Inactive:
            break;
    }
}

void ScoreTexts::ScoreText::UpdateInWaitingState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > mDelay) {
        mState = State::ScalingUp;
        mElapsedTime = 0.0f;
        mSceneObject->SetIsVisible(true);
    }
}

void ScoreTexts::ScoreText::UpdateInScalingUpState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > scaleUpDuration) {
        mState = State::SlidingUp;
        mElapsedTime = 0.0f;
        Pht::SceneObjectUtils::ScaleRecursively(*mSceneObject, textScale);
    } else {
        auto reversedNormalizedTime = 1.0f - (mElapsedTime / scaleUpDuration);
        auto scale = (1.0f - Pht::Lerp(reversedNormalizedTime, scalePoints)) * textScale;

        Pht::SceneObjectUtils::ScaleRecursively(*mSceneObject, scale);
    }
    
    mSceneObject->GetTransform().Translate({0.0f, slideVelocity * dt, 0.0f});
}

void ScoreTexts::ScoreText::UpdateInSlidingUpState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > displayTextDuration) {
        mState = State::SlidingUpFadingOutAndScalingDown;
        mElapsedTime = 0.0f;
    }
    
    mSceneObject->GetTransform().Translate({0.0f, slideVelocity * dt, 0.0f});
}

void ScoreTexts::ScoreText::UpdateInSlidingUpFadingOutAndScalingDownState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > scaleDownDuration) {
        mState = State::Inactive;
        mSceneObject->SetIsVisible(false);
        mSceneObject->SetIsStatic(true);
    } else {
        auto reversedNormalizedTime = (scaleDownDuration - mElapsedTime) / scaleDownDuration;
        Pht::SceneObjectUtils::ScaleRecursively(*mSceneObject, reversedNormalizedTime * textScale);

        auto alpha = reversedNormalizedTime;
        auto& textProperties = mTextSceneObject->GetComponent<Pht::TextComponent>()->GetProperties();
        textProperties.mColor.w = alpha;
        textProperties.mSpecularColor.w = alpha;
        textProperties.mShadowColor.w = alpha * textShadowAlpha;
    }
    
    mSceneObject->GetTransform().Translate({0.0f, slideVelocity * dt, 0.0f});
}

bool ScoreTexts::ScoreText::IsInactive() const {
    return mState == State::Inactive;
}

#include "BeginTextAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto textVisibleDuration {1.0f};
    constexpr auto textInvisibleDuration {0.5f};
}

BeginTextAnimation::BeginTextAnimation(Pht::IEngine& engine,
                                       const CommonResources& commonResources) :
    mEngine {engine},
    mCommonResources {commonResources} {}

void BeginTextAnimation::Init(Pht::Scene& scene, Pht::SceneObject& parentObject) {
    Pht::TextProperties textProperties {
        mCommonResources.GetHussarFontSize35(PotentiallyZoomedScreen::No),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.35f, 0.35f, 0.35f, 0.75f}
    };

    auto& text {scene.CreateText("Tap to begin!", textProperties)};
    mSceneObject = &text.GetSceneObject();
    mSceneObject->GetTransform().SetPosition({-2.9f, -9.0f, UiLayer::text});
    mSceneObject->SetIsVisible(false);
    parentObject.AddChild(*mSceneObject);
}

void BeginTextAnimation::Start() {
    GoToTextVisibleState();
}

void BeginTextAnimation::Update() {
    switch (mState) {
        case State::TextVisible:
            UpdateInTextVisibleState();
            break;
        case State::TextInvisible:
            UpdateInTextInvisibleState();
            break;
        case State::Inactive:
            break;
    }
}

bool BeginTextAnimation::IsActive() const {
    return mState != State::Inactive;
}

void BeginTextAnimation::UpdateInTextVisibleState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    if (mElapsedTime >= textVisibleDuration) {
        GoToTextInvisibleState();
    }
}

void BeginTextAnimation::UpdateInTextInvisibleState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    if (mElapsedTime >= textInvisibleDuration) {
        GoToTextVisibleState();
    }
}

void BeginTextAnimation::GoToTextVisibleState() {
    mState = State::TextVisible;
    mElapsedTime = 0.0f;
    mSceneObject->SetIsVisible(true);
}

void BeginTextAnimation::GoToTextInvisibleState() {
    mState = State::TextInvisible;
    mElapsedTime = 0.0f;
    mSceneObject->SetIsVisible(false);
}

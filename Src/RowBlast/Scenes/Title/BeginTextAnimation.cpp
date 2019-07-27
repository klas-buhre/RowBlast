#include "BeginTextAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "TextComponent.hpp"
#include "IAnimationSystem.hpp"
#include "Animation.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto textVisibleDuration = 1.0f;
    constexpr auto textInvisibleDuration = 0.5f;
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
    
    textProperties.mAlignment = Pht::TextAlignment::CenterX;

    auto& text = scene.CreateText("Tap screen to begin!", textProperties);
    mSceneObject = &text.GetSceneObject();
    mSceneObject->GetTransform().SetPosition({0.0f, -8.5f, UiLayer::text});
    mSceneObject->SetIsVisible(false);
    parentObject.AddChild(*mSceneObject);
    
    std::vector<Pht::Keyframe> keyframes {
        {.mTime = 0.0f, .mIsVisible = true},
        {.mTime = textVisibleDuration, .mIsVisible = false},
        {.mTime = textVisibleDuration + textInvisibleDuration, .mIsVisible = true}
    };

    auto& animationSystem = mEngine.GetAnimationSystem();
    auto& animation = animationSystem.CreateAnimation(*mSceneObject, keyframes);
    animation.SetInterpolation(Pht::Interpolation::None);
    
    animationSystem.AddAnimation(animation);
}

void BeginTextAnimation::Start() {
    mSceneObject->GetComponent<Pht::Animation>()->Play();
}

bool BeginTextAnimation::IsActive() const {
    return mSceneObject->GetComponent<Pht::Animation>()->IsPlaying();
}

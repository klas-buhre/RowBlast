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
    
    auto& animationSystem {mEngine.GetAnimationSystem()};
    auto& animation {animationSystem.CreateAnimation(*mSceneObject)};
    animation.SetInterpolation(Pht::Interpolation::None);
    
    Pht::Keyframe keyframe1 {0.0f};
    keyframe1.SetIsVisible(true);
    animation.AddKeyframe(keyframe1);
    
    Pht::Keyframe keyframe2 {textVisibleDuration};
    keyframe2.SetIsVisible(false);
    animation.AddKeyframe(keyframe2);

    Pht::Keyframe keyframe3 {textVisibleDuration + textInvisibleDuration};
    keyframe3.SetIsVisible(true);
    animation.AddKeyframe(keyframe3);

    animationSystem.AddAnimation(animation);
}

void BeginTextAnimation::Start() {
    mSceneObject->GetComponent<Pht::Animation>()->Play();
}

bool BeginTextAnimation::IsActive() const {
    return mSceneObject->GetComponent<Pht::Animation>()->IsPlaying();
}

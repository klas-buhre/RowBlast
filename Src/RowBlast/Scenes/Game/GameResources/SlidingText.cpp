#include "SlidingText.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "IAudio.hpp"
#include "TextComponent.hpp"
#include "MathUtils.hpp"
#include "ObjMesh.hpp"
#include "Scene.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"
#include "SceneObjectUtils.hpp"
#include "ISceneManager.hpp"
#include "IAnimationSystem.hpp"
#include "Animation.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"
#include "GradientRectangle.hpp"
#include "UiLayer.hpp"
#include "AudioResources.hpp"
#include "GuiUtils.hpp"
#include "LevelResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto rectangleFadeInTime = 0.3f;
    constexpr auto slideTime = 0.2f;
    constexpr auto ufoHeadStartTime = 0.2f;
    constexpr auto displayDistance = 0.65f;
    constexpr auto textWidth = 8.8f;
    const Pht::Vec3 centerPosition {0.0f, 1.8f, 0.0f};
    const Pht::Vec3 leftUfoPosition {-13.0f, 5.4f, UiLayer::slidingTextUfo};
    const Pht::Vec3 centerUfoPosition {0.0f, 5.4f, UiLayer::slidingTextUfo};
    const Pht::Vec3 rightUfoPosition {13.0f, 5.4f, UiLayer::slidingTextUfo};
    
    enum class AnimationClip {
        ScaleUp = 0,
        ScaleDown,
        Rotation
    };
    
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
    
    void WriteIntegerAtBeginningOfStringNoPadding(int value, std::string& str) {
        constexpr auto bufSize = 64;
        char buffer[bufSize];
        
        std::snprintf(buffer, bufSize, "%d   ", value); // Warning: Must be three spaces at the end.
        
        assert(str.size() >= 3);
        str[0] = buffer[0];
        str[1] = buffer[1];
        str[2] = buffer[2];
    }
}

SlidingText::SlidingText(Pht::IEngine& engine,
                         GameScene& scene,
                         const CommonResources& commonResources,
                         const LevelResources& levelResources) :
    mEngine {engine},
    mScene {scene},
    mUfo {engine, commonResources, 3.2f},
    mUfoAnimation {engine, mUfo} {
    
    auto& font = commonResources.GetHussarFontSize52PotentiallyZoomedScreen();
    auto upperY = 0.43f;
    auto lowerY = -1.28f;
    mTextMessages.reserve(7);
    
    mClearBlocksMessage = &CreateText(font,
                                      4.0f,
                                      {{-3.96f, upperY}, "CLEAR ALL", {0.25f, 0.85f}},
                                      {{-5.17f, lowerY}, "GRAY BLOCKS", {4.9f, 1.15f}},
                                      ExtraAnimations{
                                          .mUfo = true,
                                          .mGrayCube = true,
                                          .mNumObjects = true
                                      });
    mBlocksClearedMessage = &CreateText(font,
                                        3.0f,
                                        {{-5.17f, upperY}, "GRAY BLOCKS", {4.9f, 1.15f}},
                                        {{-3.3f, lowerY}, "CLEARED!", {4.85f, 1.15f}},
                                        ExtraAnimations{
                                            .mUfo = true,
                                            .mGrayCube = true,
                                            .mCheckMark = true
                                        });
    mFillSlotsMessage = &CreateText(font,
                                    4.0f,
                                    {{-2.97f, upperY}, "FILL ALL", {0.35f, 1.15f}},
                                    {{-4.73f, lowerY}, "GRAY SLOTS", {5.25f, 1.15f}},
                                    ExtraAnimations{
                                        .mUfo = true,
                                        .mBlueprintSlot = true,
                                        .mNumObjects = true
                                    });
    mSlotsFilledMessage = &CreateText(font,
                                      2.5f,
                                      {{-3.85f, upperY}, "ALL SLOTS", {0.65f, 1.15f}},
                                      {{-2.31f, lowerY}, "FILLED!", {4.9f, 1.15f}},
                                      ExtraAnimations{
                                          .mUfo = true,
                                          .mBlueprintSlot = true,
                                          .mCheckMark = true
                                      });
    mBringDownTheAsteroidMessage = &CreateText(font,
                                               4.0f,
                                               {{-5.0f, upperY}, "BRING DOWN", {0.35f, 1.15f}},
                                               {{-5.2f, lowerY}, "THE ASTEROID", {5.1f, 1.15f}},
                                               ExtraAnimations{
                                                   .mUfo = true,
                                                   .mAsteroid = true,
                                                   .mDownArrow = true
                                               });
    mTheAsteroidIsDownMessage = &CreateText(font,
                                            2.5f,
                                            {{-5.1f, upperY}, "THE ASTEROID", {0.2f, 1.15f}},
                                            {{-3.6f, lowerY}, "IS DOWN!", {1.95f, 1.15f}},
                                            ExtraAnimations{
                                                .mUfo = true,
                                                .mAsteroid = true,
                                                .mCheckMark = true
                                            });
    mOutOfMovesMessage = &CreateText(font,
                                     3.0f,
                                     {{-2.8f, upperY}, "OUT OF", {0.65f, 1.15f}},
                                     {{-2.9f, lowerY}, "MOVES!", {4.9f, 1.15f}},
                                     ExtraAnimations{
                                         .mMoves = true,
                                         .mNumObjects = true
                                     });

    CreateTwinkleParticleEffects();
    CreateExtraAnimationsContainer(commonResources, levelResources, font);
}

SlidingText::TextMessage&
SlidingText::CreateText(const Pht::Font& font,
                        float displayTime,
                        const TextLineConfig& upperTextLineConfig,
                        const TextLineConfig& lowerTextLineConfig,
                        const ExtraAnimations& extraAnimations) {
    Pht::TextProperties textProperties {
        font,
        1.1f,
        {1.0f, 0.95f, 0.9f, 1.0f},
        Pht::TextShadow::Yes,
        {0.03f, 0.03f},
        {0.79f, 0.59f, 0.395f, 1.0f}
    };
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;
    textProperties.mItalicSlant = 0.15f;
    textProperties.mMidGradientColorSubtraction = Pht::Vec3 {0.1f, 0.2f, 0.3f};
    textProperties.mSpecular = Pht::TextSpecular::Yes;
    textProperties.mSpecularOffset = {0.02f, 0.02f};
    textProperties.mSecondShadow = Pht::TextShadow::Yes;
    textProperties.mSecondShadowColor = {0.15f, 0.15f, 0.15f, 0.5f};
    textProperties.mSecondShadowOffset = Pht::Vec2 {0.075f, 0.075f};

    mTextMessages.push_back(
        TextMessage {
            CreateTextLine(upperTextLineConfig, textProperties),
            CreateTextLine(lowerTextLineConfig, textProperties),
            extraAnimations,
            displayTime
        }
    );
    
    return mTextMessages.back();
}

SlidingText::TextLine
SlidingText::CreateTextLine(const TextLineConfig& textLineConfig,
                            const Pht::TextProperties textProperties) {
    TextLine textLine {
        .mPosition = {textLineConfig.mPosition.x, textLineConfig.mPosition.y, 0.5f},
        .mSceneObject = std::make_unique<Pht::SceneObject>(),
        .mTwinkleRelativePosition = Pht::Vec3{
            textLineConfig.mTwinkleRelativePosition.x,
            textLineConfig.mTwinkleRelativePosition.y,
            UiLayer::buttonText
        }
    };

    auto textComponent =
        std::make_unique<Pht::TextComponent>(*textLine.mSceneObject,
                                             textLineConfig.mText,
                                             textProperties);
    textLine.mSceneObject->SetComponent<Pht::TextComponent>(std::move(textComponent));
    return textLine;
}

void SlidingText::CreateTwinkleParticleEffects() {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };

    Pht::ParticleSettings upperParticleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "particle_sprite_twinkle_blurred.png",
        .mTimeToLive = 1.0f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocity = 100.0f,
        .mSize = Pht::Vec2{6.5f, 6.5f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.5f
    };
    
    auto& particleSystem = mEngine.GetParticleSystem();
    mUpperTwinkleParticleEffect = particleSystem.CreateParticleEffectSceneObject(upperParticleSettings,
                                                                                 particleEmitterSettings,
                                                                                 Pht::RenderMode::Triangles);
    Pht::ParticleSettings lowerParticleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "particle_sprite_twinkle_blurred.png",
        .mTimeToLive = 1.0f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocity = -100.0f,
        .mSize = Pht::Vec2{4.2f, 4.2f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.5f
    };

    mLowerTwinkleParticleEffect = particleSystem.CreateParticleEffectSceneObject(lowerParticleSettings,
                                                                                 particleEmitterSettings,
                                                                                 Pht::RenderMode::Triangles);
}

void SlidingText::CreateExtraAnimationsContainer(const CommonResources& commonResources,
                                                 const LevelResources& levelResources,
                                                 const Pht::Font& font) {
    mExtraAnimationsContainer = std::make_unique<Pht::SceneObject>();
    mExtraAnimationsContainer->GetTransform().SetPosition({0.0f, -2.65f, UiLayer::root});
    
    CreateGreyCubeAnimation(commonResources);
    CreateAsteroidAnimation();
    CreateBlueprintSlotAnimation(levelResources);
    CreateMovesAnimation(commonResources);
    CreateNumObjectsTextAnimation(font);
    
    mCheckMarkAnimation = &CreateIconAnimation("checkmark.png",
                                               {1.1f, 0.0f, UiLayer::root},
                                               {1.325f, 1.325f},
                                               {0.41f, 0.82f, 0.41f, 1.0f});
    mDownArrowAnimation = &CreateIconAnimation("down_arrow.png",
                                               {1.1f, 0.0f, UiLayer::root},
                                               {1.05f, 1.4f},
                                               {1.0f, 0.95f, 0.9f, 1.0f});
}

void SlidingText::CreateGreyCubeAnimation(const CommonResources& commonResources) {
    auto& sceneManager = mEngine.GetSceneManager();
    auto& grayMaterial = commonResources.GetMaterials().GetGrayYellowMaterial();
    auto grayCubeSceneObject = sceneManager.CreateSceneObject(Pht::ObjMesh {"cube_428.obj", 1.25f},
                                                              grayMaterial,
                                                              mSceneResources);
    mExtraAnimationsContainer->AddChild(*grayCubeSceneObject);
    grayCubeSceneObject->GetTransform().SetPosition({-0.7f, 0.0f, UiLayer::block});
    grayCubeSceneObject->SetIsVisible(false);

    mGreyCubeAnimation = &CreateScalingAndRotationAnimation(*grayCubeSceneObject,
                                                            1.0f,
                                                            Pht::Vec3{0.0f, 0.0f, 0.0f},
                                                            Pht::Vec3{48.0f, 48.0f, 0.0f});
    mSceneResources.AddSceneObject(std::move(grayCubeSceneObject));
}

void SlidingText::CreateAsteroidAnimation() {
    auto& sceneManager = mEngine.GetSceneManager();
    Pht::Material asteroidMaterial {"gray_asteroid.jpg", 0.865f, 1.23f, 0.0f, 1.0f};
    asteroidMaterial.SetDepthTestAllowedOverride(true);
    
    auto asteroidSceneObject =
        sceneManager.CreateSceneObject(Pht::ObjMesh {"asteroid_2000.obj", 19.0f, Pht::MoveMeshToOrigin::Yes},
                                       asteroidMaterial,
                                       mSceneResources);
    
    mExtraAnimationsContainer->AddChild(*asteroidSceneObject);
    asteroidSceneObject->SetIsVisible(false);
    
    auto scale = 1.48f;
    auto& transform = asteroidSceneObject->GetTransform();
    transform.SetPosition({-0.6f, 0.0f, UiLayer::block});
    transform.SetScale(scale);
    
    mAsteroidAnimation = &CreateScalingAndRotationAnimation(*asteroidSceneObject,
                                                            scale,
                                                            Pht::Vec3{-25.0f, 15.0f, -12.0f},
                                                            Pht::Vec3{-25.0f, 100.0f, -12.0f});
    mSceneResources.AddSceneObject(std::move(asteroidSceneObject));
}

void SlidingText::CreateBlueprintSlotAnimation(const LevelResources& levelResources) {
    auto& blueprintSlotContainer = mSceneResources.CreateSceneObject();
    auto& transform = blueprintSlotContainer.GetTransform();
    transform.SetPosition({-0.6f, 0.0f, UiLayer::block});
    auto scale = 1.0f;
    transform.SetScale(scale);
    mExtraAnimationsContainer->AddChild(blueprintSlotContainer);
    blueprintSlotContainer.SetIsVisible(false);

    auto& blueprintSlot = mSceneResources.CreateSceneObject();
    blueprintSlot.SetRenderable(&levelResources.GetBlueprintSlotNonDepthWritingRenderable());
    blueprintSlotContainer.AddChild(blueprintSlot);

    auto& fieldCell = mSceneResources.CreateSceneObject();
    fieldCell.SetRenderable(&levelResources.GetFieldCellRenderable());
    fieldCell.GetTransform().SetPosition({0.0f, 0.0f, -0.005f});
    blueprintSlotContainer.AddChild(fieldCell);
    
    mBlueprintSlotAnimation = &CreateScalingAndRotationAnimation(blueprintSlotContainer,
                                                                 scale,
                                                                 Pht::Optional<Pht::Vec3>{},
                                                                 Pht::Optional<Pht::Vec3>{});
}

void SlidingText::CreateMovesAnimation(const CommonResources& commonResources) {
    auto iconScale = 1.85f;
    auto& movesIcon = CreateMovesIcon(*mExtraAnimationsContainer, commonResources, iconScale);
    movesIcon.SetIsVisible(false);

    mMovesAnimation = &CreateScalingAndRotationAnimation(movesIcon,
                                                         iconScale,
                                                         Pht::Vec3{0.0f, -25.0f, 0.0f},
                                                         Pht::Vec3{-50.0f, -35.0f, 0.0f});
}

Pht::SceneObject& SlidingText::CreateMovesIcon(Pht::SceneObject& parent,
                                               const CommonResources& commonResources,
                                               float scale) {
    auto& movesIconSceneObject = mSceneResources.CreateSceneObject();
    parent.AddChild(movesIconSceneObject);

    auto& baseTransform = movesIconSceneObject.GetTransform();
    baseTransform.SetPosition({-0.5f, 0.0f, UiLayer::block});
    baseTransform.SetScale(scale);

    Pht::ObjMesh mesh {"arrow_428_seg3_w001.obj", 3.2f};
    auto& material = commonResources.GetMaterials().GetBlueArrowMaterial();
    auto arrowRenderable = mEngine.GetSceneManager().CreateRenderableObject(mesh, material);

    CreateArrow({0.0f, 0.25f, 0.0f}, {90.0f, 0.0f, 90.0f}, *arrowRenderable, movesIconSceneObject);
    CreateArrow({0.0f, -0.25f, 0.0f}, {270.0f, 0.0f, 90.0f}, *arrowRenderable, movesIconSceneObject);
    
    mSceneResources.AddRenderableObject(std::move(arrowRenderable));
    return movesIconSceneObject;
}

void SlidingText::CreateArrow(const Pht::Vec3& position,
                              const Pht::Vec3& rotation,
                              Pht::RenderableObject& renderable,
                              Pht::SceneObject& parent) {
    auto& arrow = mSceneResources.CreateSceneObject();
    arrow.GetTransform().SetPosition(position);
    arrow.GetTransform().SetRotation(rotation);
    arrow.SetRenderable(&renderable);
    parent.AddChild(arrow);
}

Pht::Animation&
SlidingText::CreateScalingAndRotationAnimation(Pht::SceneObject& sceneObject,
                                               float scale,
                                               const Pht::Optional<Pht::Vec3>& rotationA,
                                               const Pht::Optional<Pht::Vec3>& rotationB) {
    std::vector<Pht::Keyframe> scaleUpKeyframes {
        {.mTime = 0.0f, .mScale = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = slideTime, .mScale = Pht::Vec3{scale, scale, scale}}
    };
    auto& animationSystem = mEngine.GetAnimationSystem();
    auto& animation = animationSystem.CreateAnimation(sceneObject, scaleUpKeyframes);
    auto* scaleUpClip = animation.GetClip(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleUp));
    scaleUpClip->SetWrapMode(Pht::WrapMode::Once);

    std::vector<Pht::Keyframe> scaleDownKeyframes {
        {.mTime = 0.0f, .mScale = Pht::Vec3{scale, scale, scale}},
        {.mTime = slideTime, .mScale = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
    };
    auto& scaleDownClip =
        animation.CreateClip(scaleDownKeyframes,
                             static_cast<Pht::AnimationClipId>(AnimationClip::ScaleDown));
    scaleDownClip.SetWrapMode(Pht::WrapMode::Once);
    
    if (rotationA.HasValue() && rotationB.HasValue()) {
        std::vector<Pht::Keyframe> rotationKeyframes {
            {.mTime = 0.0f, .mRotation = rotationA.GetValue()},
            {.mTime = 6.0f, .mRotation = rotationB.GetValue()}
        };
        animation.CreateClip(rotationKeyframes,
                             static_cast<Pht::AnimationClipId>(AnimationClip::Rotation));
    }
    
    return animation;
}

void SlidingText::CreateNumObjectsTextAnimation(const Pht::Font& font) {
    auto numObjectsTextScale = 0.72f;

    Pht::TextProperties textProperties {
        font,
        numObjectsTextScale,
        {1.0f, 0.95f, 0.9f, 1.0f},
        Pht::TextShadow::Yes,
        {0.03f, 0.03f},
        {0.8f, 0.6f, 0.4f, 1.0f}
    };
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;
    textProperties.mItalicSlant = 0.15f;
    textProperties.mMidGradientColorSubtraction = Pht::Vec3 {0.1f, 0.2f, 0.3f};
    textProperties.mSpecular = Pht::TextSpecular::Yes;
    textProperties.mSpecularOffset = {0.02f, 0.02f};
    textProperties.mSecondShadow = Pht::TextShadow::Yes;
    textProperties.mSecondShadowColor = Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f};
    textProperties.mSecondShadowOffset = Pht::Vec2 {0.075f, 0.075f};

    auto& numObjectsSceneObject = mSceneResources.CreateSceneObject();
    numObjectsSceneObject.GetTransform().SetPosition({1.1f, 0.0f, UiLayer::root});
    numObjectsSceneObject.SetIsVisible(false);
    auto& textSceneObject = mSceneResources.CreateSceneObject();
    textSceneObject.GetTransform().SetPosition({-0.65f / numObjectsTextScale, -0.4f / numObjectsTextScale, UiLayer::text});
    auto textComponent =
        std::make_unique<Pht::TextComponent>(textSceneObject,
                                             "   ", // Warning! Must be three spaces to fit digits.
                                             textProperties);
    mNumObjectsText = textComponent.get();
    textSceneObject.SetComponent<Pht::TextComponent>(std::move(textComponent));

    numObjectsSceneObject.AddChild(textSceneObject);
    mExtraAnimationsContainer->AddChild(numObjectsSceneObject);

    std::vector<Pht::Keyframe> textScaleUpKeyframes {
        {.mTime = 0.0f, .mTextScale = 0.0f, .mIsVisible = true},
        {.mTime = slideTime, .mTextScale = numObjectsTextScale},
    };
    auto& animationSystem = mEngine.GetAnimationSystem();
    mNumObjectsTextAnimation = &animationSystem.CreateAnimation(numObjectsSceneObject, textScaleUpKeyframes);
    auto* textScaleUpClip = mNumObjectsTextAnimation->GetClip(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleUp));
    textScaleUpClip->SetWrapMode(Pht::WrapMode::Once);

    std::vector<Pht::Keyframe> textScaleDownKeyframes {
        {.mTime = 0.0f, .mTextScale = numObjectsTextScale},
        {.mTime = slideTime, .mTextScale = 0.0f, .mIsVisible = false},
    };
    auto& textScaleDownClip =
        mNumObjectsTextAnimation->CreateClip(textScaleDownKeyframes,
                                             static_cast<Pht::AnimationClipId>(AnimationClip::ScaleDown));
    textScaleDownClip.SetWrapMode(Pht::WrapMode::Once);
}

Pht::Animation& SlidingText::CreateIconAnimation(const std::string& filename,
                                                 const Pht::Vec3& position,
                                                 const Pht::Vec2& size,
                                                 const Pht::Vec4& color) {
    auto& icon = GuiUtils::CreateIconWithShadow(mEngine,
                                                mSceneResources,
                                                filename,
                                                position,
                                                size,
                                                *mExtraAnimationsContainer,
                                                color,
                                                Pht::Vec4{0.1f, 0.1f, 0.1f, 0.55f},
                                                Pht::Vec3{-0.075f, -0.075f, UiLayer::textShadow});
    icon.SetIsVisible(false);

    std::vector<Pht::Keyframe> scaleUpKeyframes {
        {.mTime = 0.0f, .mScale = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = slideTime, .mScale = Pht::Vec3{1.0f, 1.0f, 1.0f}},
    };
    auto& animationSystem = mEngine.GetAnimationSystem();
    auto& animation = animationSystem.CreateAnimation(icon, scaleUpKeyframes);
    auto* scaleUpClip = animation.GetClip(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleUp));
    scaleUpClip->SetWrapMode(Pht::WrapMode::Once);

    std::vector<Pht::Keyframe> scaleDownKeyframes {
        {.mTime = 0.0f, .mScale = Pht::Vec3{1.0f, 1.0f, 1.0f}},
        {.mTime = slideTime, .mScale = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
    };
    auto& scaleDownClip =
        animation.CreateClip(scaleDownKeyframes,
                             static_cast<Pht::AnimationClipId>(AnimationClip::ScaleDown));
    scaleDownClip.SetWrapMode(Pht::WrapMode::Once);
    
    return animation;
}

void SlidingText::Init() {
    mContainerSceneObject = &mScene.GetScene().CreateSceneObject();
    mContainerSceneObject->GetTransform().SetPosition(centerPosition);
    mScene.GetHudContainer().AddChild(*mContainerSceneObject);
    mContainerSceneObject->SetIsVisible(false);
    mContainerSceneObject->SetIsStatic(true);

    for (auto& textMessage: mTextMessages) {
        textMessage.mUpperTextLine.mSceneObject->SetIsVisible(false);
        mContainerSceneObject->AddChild(*(textMessage.mUpperTextLine.mSceneObject));
        textMessage.mLowerTextLine.mSceneObject->SetIsVisible(false);
        mContainerSceneObject->AddChild(*(textMessage.mLowerTextLine.mSceneObject));
    }
    
    CreateGradientRectangles(*mContainerSceneObject);
    
    mContainerSceneObject->AddChild(*mUpperTwinkleParticleEffect);
    mContainerSceneObject->AddChild(*mLowerTwinkleParticleEffect);
    
    mUfo.Init(mScene.GetUiViewsContainer());
    mUfo.Hide();
    mEngine.GetRenderer().DisableShader(Pht::ShaderId::TexturedEnvMapLighting);
    
    mContainerSceneObject->AddChild(*mExtraAnimationsContainer);
    mExtraAnimationsContainer->SetIsVisible(false);

    auto& frustumSize = mEngine.GetRenderer().GetHudFrustumSize();
    mLeftPosition = {-frustumSize.x / 2.0f - textWidth / 2.0f, 0.0f, 0.0f};
    mRightPosition = {frustumSize.x / 2.0f + textWidth / 2.0f, 0.0f, 0.0f};
}

void SlidingText::CreateGradientRectangles(Pht::SceneObject& containerSceneObject) {
    auto& scene = mScene.GetScene();
    mGradientRectanglesSceneObject = &scene.CreateSceneObject();
    containerSceneObject.AddChild(*mGradientRectanglesSceneObject);
    mGradientRectanglesSceneObject->SetIsVisible(false);
    
    auto& frustumSize = mEngine.GetRenderer().GetHudFrustumSize();
    auto height = 5.8f;
    auto stripeHeight = 0.09f;
    auto stripeOffset = stripeHeight / 2.0f;
    Pht::Vec2 size {frustumSize.x, height};
    auto leftQuadWidth = frustumSize.x / 3.0f;
    auto rightQuadWidth = frustumSize.x / 3.0f;

    GradientRectangleColors colors {
        .mLeft = {0.93f, 0.5f, 0.0f, 0.8f},
        .mMid = {0.3f, 0.3f, 0.35f, 0.8f},
        .mRight = {0.93f, 0.5f, 0.0f, 0.8f}
    };

    CreateGradientRectangle(scene,
                            *mGradientRectanglesSceneObject,
                            {0.0f, 0.0f, UiLayer::slidingTextRectangle},
                            size,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            colors,
                            colors);

    GradientRectangleColors stripeColors {
        .mLeft = {0.93f, 0.5f, 0.0f, 0.8f},
        .mMid = {1.0f, 0.95f, 0.9f, 0.95f},
        .mRight = {0.93f, 0.5f, 0.0f, 0.8f}
    };

    Pht::Vec2 stripeSize {frustumSize.x, stripeHeight};
    CreateGradientRectangle(scene,
                            *mGradientRectanglesSceneObject,
                            {0.0f, height / 2.0f + stripeOffset, UiLayer::slidingTextRectangle},
                            stripeSize,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            stripeColors,
                            stripeColors);
    CreateGradientRectangle(scene,
                            *mGradientRectanglesSceneObject,
                            {0.0f, -height / 2.0f - stripeOffset, UiLayer::slidingTextRectangle},
                            stripeSize,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            stripeColors,
                            stripeColors);
}

void SlidingText::StartClearBlocksMessage(int numBlocks) {
    WriteIntegerAtBeginningOfStringNoPadding(numBlocks, mNumObjectsText->GetText());
    Start(*mClearBlocksMessage);
}

void SlidingText::StartBlocksClearedMessage() {
    Start(*mBlocksClearedMessage);
}

void SlidingText::StartFillSlotsMessage(int numSlots) {
    WriteIntegerAtBeginningOfStringNoPadding(numSlots, mNumObjectsText->GetText());
    Start(*mFillSlotsMessage);
}

void SlidingText::StartSlotsFilledMessage() {
    Start(*mSlotsFilledMessage);
}

void SlidingText::StartBringDownTheAsteroidMessage() {
    Start(*mBringDownTheAsteroidMessage);
}

void SlidingText::StartTheAsteroidIsDownMessage() {
    Start(*mTheAsteroidIsDownMessage);
}

void SlidingText::StartOutOfMovesMessage() {
    WriteIntegerAtBeginningOfStringNoPadding(0, mNumObjectsText->GetText());
    Start(*mOutOfMovesMessage);
}

void SlidingText::Start(const TextMessage& textMessage) {
    mTextMessage = &textMessage;
    mState = State::RectangleAppearing;
    mElapsedTime = 0.0f;
    
    mTextPosition = {0.0f, 0.0f, 0.0f};

    mDisplayVelocity = displayDistance / mTextMessage->mDisplayTime;
    mInitialVelocity = mRightPosition.x * 2.0f / slideTime - mDisplayVelocity -
                       mDisplayVelocity * mTextMessage->mDisplayTime / slideTime;
    mVelocity = mInitialVelocity;
    
    UpdateTextLineSceneObjectPositions();
    
    mContainerSceneObject->SetIsVisible(true);
    mContainerSceneObject->SetIsStatic(false);
    
    mGradientRectanglesSceneObject->SetIsVisible(true);
    mGradientRectanglesSceneObject->SetIsStatic(false);
    mGradientRectanglesSceneObject->GetTransform().SetPosition({0.0f, -0.85f, 0.0f});
    Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject, 0.0f);
    
    mUfoState = UfoState::Inactive;
    mUfo.SetPosition(rightUfoPosition);
    mUfoAnimation.Init();
    
    if (mTextMessage->mExtraAnimations.mUfo) {
        mUfo.Show();
    }
    
    mEngine.GetRenderer().EnableShader(Pht::ShaderId::TexturedEnvMapLighting);
}

SlidingText::State SlidingText::Update() {
    switch (mState) {
        case State::RectangleAppearing:
            UpdateInRectangleAppearingState();
            break;
        case State::SlidingIn:
            UpdateInSlidingInState();
            break;
        case State::DisplayingText:
            UpdateInDisplayingTextState();
            break;
        case State::SlidingOut:
            UpdateInSlidingOutState();
            break;
        case State::RectangleDisappearing:
            UpdateInRectangleDisappearingState();
            break;
        case State::Inactive:
            break;
    }
    
    return mState;
}

void SlidingText::UpdateInRectangleAppearingState() {
    auto dt = mEngine.GetLastFrameSeconds();
    mElapsedTime += dt;
    
    Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject,
                                               mElapsedTime / rectangleFadeInTime);

    auto normalizedTime = (rectangleFadeInTime - mElapsedTime) / rectangleFadeInTime;
    auto scale = 1.0f + 3.0f * Pht::Lerp(normalizedTime, scalePoints);
    
    mGradientRectanglesSceneObject->GetTransform().SetScale(scale);
    
    if (mElapsedTime > rectangleFadeInTime) {
        mState = State::SlidingIn;
        Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject, 1.0f);
        mGradientRectanglesSceneObject->GetTransform().SetScale(1.0f);
        
        mTextMessage->mUpperTextLine.mSceneObject->SetIsVisible(true);
        mTextMessage->mLowerTextLine.mSceneObject->SetIsVisible(true);

        mExtraAnimationsContainer->SetIsVisible(true);
        StartPlayingExtraAnimations();
        
        auto& audio = mEngine.GetAudio();
        audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::SlidingTextWhoosh1));
    }
}

void SlidingText::UpdateInSlidingInState() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    mTextPosition.x += mVelocity * dt;
    mVelocity -= dt * (mInitialVelocity - mDisplayVelocity) / slideTime;
    mElapsedTime += dt;
    
    UpdateExtraAnimations();
    
    if (mVelocity < mDisplayVelocity || mTextPosition.x >= mRightPosition.x - displayDistance / 2.0f) {
        mState = State::DisplayingText;
        mElapsedTime = 0.0f;
        mTextPosition.x = mRightPosition.x - displayDistance / 2.0f;
        
        mEngine.GetInput().EnableInput();
        mUpperTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
        mLowerTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
        FlyInUfo();
    }
    
    UpdateTextLineSceneObjectPositions();
}

void SlidingText::UpdateTextLineSceneObjectPositions() {
    auto upperTextLinePosition = mLeftPosition + mTextPosition + mTextMessage->mUpperTextLine.mPosition;
    mTextMessage->mUpperTextLine.mSceneObject->GetTransform().SetPosition(upperTextLinePosition);
    
    auto lowerTextLinePosition = mRightPosition - mTextPosition + mTextMessage->mLowerTextLine.mPosition;
    mTextMessage->mLowerTextLine.mSceneObject->GetTransform().SetPosition(lowerTextLinePosition);

    Pht::Vec3 upperTwinklePosition {
        upperTextLinePosition.x + mTextMessage->mUpperTextLine.mTwinkleRelativePosition.x,
        upperTextLinePosition.y + mTextMessage->mUpperTextLine.mTwinkleRelativePosition.y,
        UiLayer::buttonText
    };
    mUpperTwinkleParticleEffect->GetTransform().SetPosition(upperTwinklePosition);

    Pht::Vec3 lowerTwinklePosition {
        lowerTextLinePosition.x + mTextMessage->mLowerTextLine.mTwinkleRelativePosition.x,
        lowerTextLinePosition.y + mTextMessage->mLowerTextLine.mTwinkleRelativePosition.y,
        UiLayer::buttonText
    };
    mLowerTwinkleParticleEffect->GetTransform().SetPosition(lowerTwinklePosition);
}

void SlidingText::UpdateInDisplayingTextState() {
    auto dt = mEngine.GetLastFrameSeconds();
    mTextPosition.x += mDisplayVelocity * dt;
    mElapsedTime += dt;
    
    mUpperTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mLowerTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    UpdateUfo();
    UpdateExtraAnimations();
    
    if (mElapsedTime > mTextMessage->mDisplayTime || mEngine.GetInput().ConsumeWholeTouch()) {
        mState = State::SlidingOut;
        mElapsedTime = 0.0f;
        mVelocity = mDisplayVelocity;
        
        auto& audio = mEngine.GetAudio();
        audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::SlidingTextWhoosh2));
        
        mUpperTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Stop();
        mLowerTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    
        FlyOutUfo();
        StartScalingDownExtraAnimations();
    }
    
    if (mElapsedTime > mTextMessage->mDisplayTime - ufoHeadStartTime) {
        FlyOutUfo();
    }
    
    UpdateTextLineSceneObjectPositions();
}

void SlidingText::UpdateInSlidingOutState() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    mTextPosition.x += mVelocity * dt;
    mVelocity += dt * (mInitialVelocity - mDisplayVelocity) / slideTime;
    mElapsedTime += dt;

    UpdateTextLineSceneObjectPositions();
    UpdateUfo();
    UpdateExtraAnimations();

    if (mTextPosition.x >= mRightPosition.x * 2.0f) {
        mState = State::RectangleDisappearing;
        mElapsedTime = 0.0f;

        mTextMessage->mUpperTextLine.mSceneObject->SetIsVisible(false);
        mTextMessage->mLowerTextLine.mSceneObject->SetIsVisible(false);
    }
}

void SlidingText::UpdateInRectangleDisappearingState() {
    auto dt = mEngine.GetLastFrameSeconds();
    mElapsedTime += dt;
    
    Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject,
                                               (rectangleFadeInTime - mElapsedTime) / rectangleFadeInTime);
    UpdateUfo();
    UpdateExtraAnimations();

    if (mElapsedTime > rectangleFadeInTime) {
        mState = State::Inactive;
        mContainerSceneObject->SetIsVisible(false);
        mContainerSceneObject->SetIsStatic(true);
        mGradientRectanglesSceneObject->SetIsVisible(false);
        mGradientRectanglesSceneObject->SetIsStatic(true);
        mExtraAnimationsContainer->SetIsVisible(false);
        mUfo.Hide();
        mEngine.GetRenderer().DisableShader(Pht::ShaderId::TexturedEnvMapLighting);
    }
}

void SlidingText::UpdateUfo() {
    mUfoAnimation.Update();
    
    switch (mUfoState) {
        case UfoState::FlyingIn:
            if (!mUfoAnimation.IsActive()) {
                mUfoState = UfoState::Hovering;
            }
            break;
        default:
            break;
    }
}

void SlidingText::FlyInUfo() {
    switch (mUfoState) {
        case UfoState::Inactive:
            mUfoAnimation.StartShortWarpSpeed(centerUfoPosition);
            mUfoState = UfoState::FlyingIn;
            break;
        default:
            break;
    }
}

void SlidingText::FlyOutUfo() {
    switch (mUfoState) {
        case UfoState::FlyingIn:
        case UfoState::Hovering:
            mUfoAnimation.StartShortWarpSpeed(leftUfoPosition);
            mUfoState = UfoState::FlyingOut;
            break;
        case UfoState::FlyingOut:
        case UfoState::Inactive:
            break;
    }
}

void SlidingText::StartPlayingExtraAnimations() {
    if (mTextMessage->mExtraAnimations.mGrayCube) {
        mGreyCubeAnimation->Stop();
        mGreyCubeAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleUp));
        mGreyCubeAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::Rotation));
    }
    
    if (mTextMessage->mExtraAnimations.mAsteroid) {
        mAsteroidAnimation->Stop();
        mAsteroidAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleUp));
        mAsteroidAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::Rotation));
    }

    if (mTextMessage->mExtraAnimations.mBlueprintSlot) {
        mBlueprintSlotAnimation->Stop();
        mBlueprintSlotAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleUp));
    }

    if (mTextMessage->mExtraAnimations.mMoves) {
        mMovesAnimation->Stop();
        mMovesAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleUp));
        mMovesAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::Rotation));
    }

    if (mTextMessage->mExtraAnimations.mNumObjects) {
        mNumObjectsTextAnimation->Stop();
        mNumObjectsTextAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleUp));
    }
    
    if (mTextMessage->mExtraAnimations.mCheckMark) {
        mCheckMarkAnimation->Stop();
        mCheckMarkAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleUp));
    }
    
    if (mTextMessage->mExtraAnimations.mDownArrow) {
        mDownArrowAnimation->Stop();
        mDownArrowAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleUp));
    }
}

void SlidingText::UpdateExtraAnimations() {
    auto dt = mEngine.GetLastFrameSeconds();
    mGreyCubeAnimation->Update(dt);
    mAsteroidAnimation->Update(dt);
    mBlueprintSlotAnimation->Update(dt);
    mMovesAnimation->Update(dt);
    mNumObjectsTextAnimation->Update(dt);
    mCheckMarkAnimation->Update(dt);
    mDownArrowAnimation->Update(dt);
}

void SlidingText::StartScalingDownExtraAnimations() {
    if (mTextMessage->mExtraAnimations.mGrayCube) {
        mGreyCubeAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleDown));
    }

    if (mTextMessage->mExtraAnimations.mAsteroid) {
        mAsteroidAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleDown));
    }

    if (mTextMessage->mExtraAnimations.mBlueprintSlot) {
        mBlueprintSlotAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleDown));
    }

    if (mTextMessage->mExtraAnimations.mMoves) {
        mMovesAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleDown));
    }

    if (mTextMessage->mExtraAnimations.mNumObjects) {
        mNumObjectsTextAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleDown));
    }
    
    if (mTextMessage->mExtraAnimations.mCheckMark) {
        mCheckMarkAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleDown));
    }
    
    if (mTextMessage->mExtraAnimations.mDownArrow) {
        mDownArrowAnimation->Play(static_cast<Pht::AnimationClipId>(AnimationClip::ScaleDown));
    }
}

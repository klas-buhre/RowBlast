#include "OutOfMovesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ObjMesh.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"
#include "ISceneManager.hpp"
#include "MathUtils.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "UserServices.hpp"
#include "IGuiLightProvider.hpp"
#include "GameScene.hpp"
#include "GameHud.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 uiLightDirection {0.6f, 1.0f, 1.0f};
    const Pht::Vec3 uiCameraPosition {0.0f, 0.0f, 15.5f};
    constexpr auto rotationAnimationDuration = 3.8f;
    constexpr auto rotationAmplitude = 10.0f;
    constexpr auto scaleAnimationDuration = 1.0f;
    constexpr auto scaleAmplitude = 0.03f;
    constexpr auto iconScale = 1.88f;
    constexpr auto particleVelocity = 0.6f;
    constexpr auto particleVelocityRandomPart = 0.1f;
    
    Pht::Vec3 ParticleVelocityFunction() {
        auto phi = Pht::ToRadians(Pht::NormalizedRand() * 360.0f);
    
        auto magnitude =
            particleVelocity + (Pht::NormalizedRand() - 0.5f) * particleVelocityRandomPart;
        
        return {
            static_cast<float>(magnitude * std::cos(phi)),
            static_cast<float>(magnitude * std::sin(phi)),
            0.0f
        };
    }
}

OutOfMovesDialogView::OutOfMovesDialogView(Pht::IEngine& engine,
                                           const CommonResources& commonResources) :
    mEngine {engine} {
    
    mUpperHudSceneObject = &CreateSceneObject();
    mUpperHudSceneObject->SetIsStatic(true);
    GetRoot().AddChild(*mUpperHudSceneObject);

    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetMediumDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::lowerTextRectangle});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());

    CreateText({-2.6f, 4.95f, UiLayer::text},
               "CONTINUE?",
               guiResources.GetLargeWhiteTextProperties(zoom));

    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);
    GuiUtils::CreateTitleBarLine(engine, *this);
    
    CreateAddMovesIcon({0.0f, 2.25f, UiLayer::root}, commonResources);

    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-5.1f, -0.925f, UiLayer::text}, "Get 5 more moves and continue", textProperties);
    CreateText({-1.3f, -2.0f, UiLayer::text}, "playing", textProperties);

    mPlayOnButton = GuiUtils::CreatePlayOnButton(engine,
                                                 *this,
                                                 {0.0f, -4.8f, UiLayer::textRectangle},
                                                 PurchasingService::addMovesPriceInCoins,
                                                 commonResources,
                                                 zoom);
}

void OutOfMovesDialogView::CreateAddMovesIcon(const Pht::Vec3& position,
                                              const CommonResources& commonResources) {
    auto& container = CreateSceneObject();
    container.GetTransform().SetPosition(position);
    GetRoot().AddChild(container);
    
    CreateMovesIcon(container, commonResources);
    CreateGlowEffect(container);
    CreateParticles(container);

    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& textProperties = commonResources.GetGuiResources().GetSmallWhiteTextProperties(zoom);
    CreateText({-0.45f, -1.3f, UiLayer::buttonText}, "+5", textProperties, container);
    Pht::SceneObjectUtils::ScaleRecursively(container, 1.5f);
}

void OutOfMovesDialogView::CreateMovesIcon(Pht::SceneObject& parent,
                                           const CommonResources& commonResources) {
    mMovesIconSceneObject = &CreateSceneObject();
    parent.AddChild(*mMovesIconSceneObject);

    auto& baseTransform = mMovesIconSceneObject->GetTransform();
    baseTransform.SetPosition({0.0f, 0.1f, UiLayer::block});
    baseTransform.SetScale(iconScale);

    Pht::ObjMesh mesh {"arrow_428_seg3_w001.obj", 3.2f};
    auto& material = commonResources.GetMaterials().GetBlueArrowMaterial();
    mArrowRenderable = mEngine.GetSceneManager().CreateRenderableObject(mesh, material);

    CreateArrow({0.0f, 0.25f, 0.0f}, {90.0f, 0.0f, 90.0f}, *mArrowRenderable, *mMovesIconSceneObject);
    CreateArrow({0.0f, -0.25f, 0.0f}, {270.0f, 0.0f, 90.0f}, *mArrowRenderable, *mMovesIconSceneObject);
}

void OutOfMovesDialogView::CreateArrow(const Pht::Vec3& position,
                                       const Pht::Vec3& rotation,
                                       Pht::RenderableObject& renderable,
                                       Pht::SceneObject& parent) {
    auto& arrow = CreateSceneObject();
    arrow.GetTransform().SetPosition(position);
    arrow.GetTransform().SetRotation(rotation);
    arrow.SetRenderable(&renderable);
    parent.AddChild(arrow);
}

void OutOfMovesDialogView::CreateGlowEffect(Pht::SceneObject& parentObject) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 2
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{0.46f, 0.2f, 0.65f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare24.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.5f,
        .mZAngularVelocity = 20.0f,
        .mZAngularVelocityRandomPart = 10.0f,
        .mSize = Pht::Vec2{11.25f, 11.25f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.0f,
        .mGrowDuration = 0.5f
    };
    
    auto& particleSystem = mEngine.GetParticleSystem();
    mGlowEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                 particleEmitterSettings,
                                                                 Pht::RenderMode::Triangles);
    auto& material = mGlowEffect->GetRenderable()->GetMaterial();
    material.SetShaderId(Pht::ShaderId::ParticleNoAlphaTexture);
    
    mGlowEffect->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::panel});
    parentObject.AddChild(*mGlowEffect);
}

void OutOfMovesDialogView::CreateParticles(Pht::SceneObject& parentObject) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mFrequency = 4.0f
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocityFunction = ParticleVelocityFunction,
        .mColor = Pht::Vec4{1.0f, 0.5f, 0.8f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.1f, 0.1f, 0.1f, 0.0f},
        .mTextureFilename = "particle_sprite_point_blurred.png",
        .mTimeToLive = 3.5f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 2.5f,
        .mSize = Pht::Vec2{0.75f, 0.75f},
        .mSizeRandomPart = 0.2f,
        .mGrowDuration = 0.05f,
        .mShrinkDuration = 1.0f
    };
    
    auto& particleSystem = mEngine.GetParticleSystem();
    mParticles = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                particleEmitterSettings,
                                                                Pht::RenderMode::Triangles);
    mParticles->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::panel});
    parentObject.AddChild(*mParticles);
}

void OutOfMovesDialogView::SetUp(GameScene& scene) {
    mScene = &scene;
    scene.SetUiCameraPosition(uiCameraPosition);
    
    if (mGuiLightProvider) {
        mGuiLightProvider->SetGuiLightDirections(uiLightDirection, uiLightDirection);
    }

    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
    mParticles->GetComponent<Pht::ParticleEffect>()->Start();
    
    auto& hud = scene.GetHud();
    auto& upperHudContainer = hud.GetUpperContainer();
    upperHudContainer.DetachChild(&hud.GetProgressContainer());
    upperHudContainer.DetachChild(&hud.GetMovesContainer());
    mUpperHudSceneObject->DetachChildren();
    mUpperHudSceneObject->AddChild(hud.GetProgressContainer());
    mUpperHudSceneObject->AddChild(hud.GetMovesContainer());
    
    mRotationAnimationTime = 0.0;
    mScaleAnimationTime = 0.0f;
}

void OutOfMovesDialogView::Update() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mParticles->GetComponent<Pht::ParticleEffect>()->Update(dt);
    AnimateIconRotation(dt);
}

void OutOfMovesDialogView::AnimateIconRotation(float dt) {
    mRotationAnimationTime += dt;
    if (mRotationAnimationTime > rotationAnimationDuration) {
        mRotationAnimationTime = 0.0f;
    }

    auto tRotation = mRotationAnimationTime * 2.0f * 3.1415f / rotationAnimationDuration;
    auto xAngle = rotationAmplitude * std::sin(tRotation) - 20.0f;
    auto yAngle = rotationAmplitude * std::cos(tRotation) - 20.0f;
    mMovesIconSceneObject->GetTransform().SetRotation({xAngle, yAngle, 0.0f});
    
    mScaleAnimationTime += dt;
    if (mScaleAnimationTime > scaleAnimationDuration) {
        mScaleAnimationTime = 0.0f;
    }
    
    auto tScale = mScaleAnimationTime * 2.0f * 3.1415f / scaleAnimationDuration;
    auto scale = iconScale + iconScale * scaleAmplitude * std::sin(tScale);
    mMovesIconSceneObject->GetTransform().SetScale(scale);
}

void OutOfMovesDialogView::OnDeactivate() {
    if (mScene) {
        mScene->SetDefaultUiCameraPosition();
    }
    
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}

void OutOfMovesDialogView::HandOverHudObjects() {
    auto& hud = mScene->GetHud();
    mUpperHudSceneObject->DetachChildren();
    hud.GetUpperContainer().AddChild(hud.GetProgressContainer());
    hud.GetUpperContainer().AddChild(hud.GetMovesContainer());
}

#include "OutOfMovesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ObjMesh.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"
#include "ISceneManager.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "UserServices.hpp"
#include "PieceResources.hpp"
#include "IGuiLightProvider.hpp"
#include "GameScene.hpp"
#include "GameHud.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 uiLightDirection {0.6f, 1.0f, 1.0f};
    const Pht::Vec3 uiCameraPosition {0.0f, 0.0f, 15.5f};
    constexpr auto lPieceAnimationDuration = 3.8f;
    constexpr auto lPieceRotationAmplitude = 10.0f;
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
                                           const CommonResources& commonResources,
                                           const PieceResources& pieceResources) :
    mEngine {engine} {
    
    mUpperHudSceneObject = &CreateSceneObject();
    mUpperHudSceneObject->SetIsStatic(true);
    GetRoot().AddChild(*mUpperHudSceneObject);

    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetMediumDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());

    CreateText({-2.6f, 4.95f, UiLayer::text},
               "CONTINUE?",
               guiResources.GetLargeWhiteTextProperties(zoom));

    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);
    GuiUtils::CreateTitleBarLine(engine, *this);
    
    CreateAddMovesIcon({0.0f, 2.05f, UiLayer::root}, commonResources, pieceResources);

    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-5.1f, -0.825f, UiLayer::text}, "Get 5 more moves and continue", textProperties);
    CreateText({-1.3f, -1.9f, UiLayer::text}, "playing", textProperties);
    
    Pht::Vec2 playOnButtonInputSize {194.0f, 50.0f};

    MenuButton::Style playOnButtonStyle;
    playOnButtonStyle.mPressedScale = 1.05f;
    playOnButtonStyle.mTextScale = 1.05f;
    playOnButtonStyle.mRenderableObject = &guiResources.GetLargeGreenGlossyButton(zoom);
    playOnButtonStyle.mSelectedRenderableObject = &guiResources.GetLargeDarkGreenGlossyButton(zoom);

    mPlayOnButton = std::make_unique<MenuButton>(engine,
                                                 *this,
                                                 Pht::Vec3 {0.0f, -4.7f, UiLayer::textRectangle},
                                                 playOnButtonInputSize,
                                                 playOnButtonStyle);

    Pht::TextProperties buttonTextProperties {
        commonResources.GetHussarFontSize27(zoom),
        1.05f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };

    mPlayOnButton->CreateText({-3.1f, -0.24f, UiLayer::buttonText},
                              "CONTINUE           " + std::to_string(PurchasingService::addMovesPriceInCoins),
                              buttonTextProperties);

    auto& coin =
        CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                          commonResources.GetMaterials().GetGoldMaterial(),
                          engine.GetSceneManager());
    auto& coinTransform = coin.GetTransform();
    coinTransform.SetPosition({1.85f, 0.0f, UiLayer::buttonOverlayObject});
    coinTransform.SetRotation({0.0f, 40.0f, 0.0f});
    coinTransform.SetScale(0.9f);
    mPlayOnButton->GetSceneObject().AddChild(coin);
}

void OutOfMovesDialogView::CreateAddMovesIcon(const Pht::Vec3& position,
                                              const CommonResources& commonResources,
                                              const PieceResources& pieceResources) {
    auto& container = CreateSceneObject();
    container.GetTransform().SetPosition(position);
    GetRoot().AddChild(container);
    
    CreateLPiece(container, pieceResources);
    CreateGlowEffect(container);
    CreateParticles(container);

    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& textProperties = commonResources.GetGuiResources().GetSmallWhiteTextProperties(zoom);
    CreateText({-0.45f, -1.7f, UiLayer::buttonText}, "+5", textProperties, container);
}

void OutOfMovesDialogView::CreateLPiece(Pht::SceneObject& parentObject,
                                        const PieceResources& pieceResources) {
    mLPieceSceneObject = &CreateSceneObject();
    parentObject.AddChild(*mLPieceSceneObject);
    
    auto& baseTransform = mLPieceSceneObject->GetTransform();
    baseTransform.SetPosition({0.0f, 0.0f, UiLayer::block});
    auto scale = 0.75f;
    baseTransform.SetScale(scale);

    auto& blockRenderable =
        pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                BlockColor::Green,
                                                BlockBrightness::Normal);

    auto halfCellSize = 0.625f;
    CreateGreenBlock({-halfCellSize, -halfCellSize, 0.0f}, blockRenderable, *mLPieceSceneObject);
    CreateGreenBlock({halfCellSize, -halfCellSize, 0.0f}, blockRenderable, *mLPieceSceneObject);
    CreateGreenBlock({halfCellSize, halfCellSize, 0.0f}, blockRenderable, *mLPieceSceneObject);
}

void OutOfMovesDialogView::CreateGreenBlock(const Pht::Vec3& position,
                                            Pht::RenderableObject& blockRenderable,
                                            Pht::SceneObject& lPiece) {
    auto& block = CreateSceneObject();
    block.GetTransform().SetPosition(position);
    block.SetRenderable(&blockRenderable);
    lPiece.AddChild(block);
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
    material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
    
    mGlowEffect->GetTransform().SetPosition({0.2f, -0.2f, UiLayer::panel});
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
    mParticles->GetTransform().SetPosition({0.2f, -0.1f, UiLayer::panel});
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
    hud.GetUpperContainer().DetachChildren();
    mUpperHudSceneObject->DetachChildren();
    mUpperHudSceneObject->AddChild(hud.GetProgressContainer());
    mUpperHudSceneObject->AddChild(hud.GetMovesContainer());
    
    mAnimationTime = 0.0;
}

void OutOfMovesDialogView::Update() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mParticles->GetComponent<Pht::ParticleEffect>()->Update(dt);
    AnimateLPieceRotation(dt);
}

void OutOfMovesDialogView::AnimateLPieceRotation(float dt) {
    mAnimationTime += dt;
    if (mAnimationTime > lPieceAnimationDuration) {
        mAnimationTime = 0.0f;
    }

    auto t = mAnimationTime * 2.0f * 3.1415f / lPieceAnimationDuration;
    auto xAngle = lPieceRotationAmplitude * std::sin(t) - 20.0f;
    auto yAngle = lPieceRotationAmplitude * std::cos(t) - 20.0f;
    mLPieceSceneObject->GetTransform().SetRotation({xAngle, yAngle, 0.0f});
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
    hud.GetUpperContainer().DetachChildren();
    hud.GetUpperContainer().AddChild(hud.GetProgressContainer());
    hud.GetUpperContainer().AddChild(hud.GetMovesContainer());
}

#include "OutOfMovesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "UserServices.hpp"
#include "PieceResources.hpp"
#include "IGuiLightProvider.hpp"
#include "GameScene.hpp"
#include "GameHud.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 uiLightDirection {0.6f, 1.0f, 1.0f};
    const Pht::Vec3 uiCameraPosition {0.0f, 0.0f, 15.5f};
}

OutOfMovesDialogView::OutOfMovesDialogView(Pht::IEngine& engine,
                                           const CommonResources& commonResources,
                                           const PieceResources& pieceResources) :
    mEngine {engine} {
    
    mUpperHudSceneObject = &CreateSceneObject();
    mUpperHudSceneObject->SetIsStatic(true);
    GetRoot().AddChild(*mUpperHudSceneObject);

    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumDarkMenuWindow(PotentiallyZoomedScreen::Yes)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-3.5f, 4.95f, UiLayer::text},
               "OUT OF MOVES",
               guiResources.GetLargeWhiteTextProperties(zoom));
    
    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.3f,
        GetSize().y / 2.0f - 1.3f,
        UiLayer::textRectangle
    };
    
    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};
    
    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mRenderableObject = &guiResources.GetCloseButton(zoom);
    
    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    CreateAddMovesIcon({0.0f, 2.0f, UiLayer::root}, commonResources, pieceResources);

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-5.1f, -0.625f, UiLayer::text}, "Get 5 more moves and continue", textProperties);
    CreateText({-1.3f, -1.7f, UiLayer::text}, "playing", textProperties);
    
    Pht::Vec2 playOnButtonInputSize {194.0f, 50.0f};

    MenuButton::Style playOnButtonStyle;
    playOnButtonStyle.mPressedScale = 1.05f;
    playOnButtonStyle.mTextScale = 1.05f;
    playOnButtonStyle.mRenderableObject = &guiResources.GetLargeGreenGlossyButton(zoom);
    playOnButtonStyle.mSelectedRenderableObject = &guiResources.GetLargeDarkGreenGlossyButton(zoom);

    mPlayOnButton = std::make_unique<MenuButton>(engine,
                                                 *this,
                                                 Pht::Vec3 {0.0f, -4.65f, UiLayer::textRectangle},
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

    auto& coin {
        CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                          commonResources.GetMaterials().GetGoldMaterial(),
                          engine.GetSceneManager())
    };
    auto& coinTransform {coin.GetTransform()};
    coinTransform.SetPosition({1.85f, 0.0f, UiLayer::buttonOverlayObject});
    coinTransform.SetRotation({0.0f, 40.0f, 0.0f});
    coinTransform.SetScale(0.9f);
    mPlayOnButton->GetSceneObject().AddChild(coin);
}

void OutOfMovesDialogView::CreateAddMovesIcon(const Pht::Vec3& position,
                                              const CommonResources& commonResources,
                                              const PieceResources& pieceResources) {
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition(position);
    GetRoot().AddChild(container);
    
    CreateLPiece(container, pieceResources);
    CreateGlowEffect(container);
    
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize35(zoom),
        1.1f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.075f, 0.075f},
        {0.27f, 0.27f, 0.27f, 0.5f}
    };
    
    CreateText({-0.7f, -0.4f, UiLayer::buttonText}, "+5", textProperties, container);
}

void OutOfMovesDialogView::CreateLPiece(Pht::SceneObject& parentObject,
                                        const PieceResources& pieceResources) {
    auto& lPiece {CreateSceneObject()};
    parentObject.AddChild(lPiece);
    
    auto& baseTransform {lPiece.GetTransform()};
    baseTransform.SetPosition({-0.2f, 0.2f, UiLayer::root});
    baseTransform.SetRotation({-30.0f, -30.0f, 0.0f});
    auto scale {0.75f};
    baseTransform.SetScale(scale);
    
    auto& blockRenderable {
        pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                BlockColor::Green,
                                                BlockBrightness::Normal)
    };
    
    auto halfCellSize {0.625f};
    CreateGreenBlock({-halfCellSize, -halfCellSize, -scale}, blockRenderable, lPiece);
    CreateGreenBlock({halfCellSize, -halfCellSize, -scale}, blockRenderable, lPiece);
    CreateGreenBlock({halfCellSize, halfCellSize, -scale}, blockRenderable, lPiece);
}

void OutOfMovesDialogView::CreateGreenBlock(const Pht::Vec3& position,
                                            Pht::RenderableObject& blockRenderable,
                                            Pht::SceneObject& lPiece) {
    auto& block {CreateSceneObject()};
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
    
    auto& particleSystem {mEngine.GetParticleSystem()};
    mGlowEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                 particleEmitterSettings,
                                                                 Pht::RenderMode::Triangles);
    auto& material {mGlowEffect->GetRenderable()->GetMaterial()};
    material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
    
    mGlowEffect->GetTransform().SetPosition({0.2f, -0.2f, -1.9f});
    parentObject.AddChild(*mGlowEffect);
}

void OutOfMovesDialogView::SetUp(GameScene& scene) {
    mScene = &scene;
    scene.SetUiCameraPosition(uiCameraPosition);
    
    if (mGuiLightProvider) {
        mGuiLightProvider->SetGuiLightDirections(uiLightDirection, uiLightDirection);
    }

    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
    
    auto& hud {scene.GetHud()};
    hud.GetUpperContainer().DetachChildren();
    mUpperHudSceneObject->DetachChildren();
    mUpperHudSceneObject->AddChild(hud.GetProgressContainer());
    mUpperHudSceneObject->AddChild(hud.GetMovesContainer());
}

void OutOfMovesDialogView::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
}

void OutOfMovesDialogView::OnDeactivate() {
    if (mScene) {
        mScene->SetDefaultUiCameraPosition();
        
        auto& hud {mScene->GetHud()};
        mUpperHudSceneObject->DetachChildren();
        hud.GetUpperContainer().DetachChildren();
        hud.GetUpperContainer().AddChild(hud.GetProgressContainer());
        hud.GetUpperContainer().AddChild(hud.GetMovesContainer());
    }
    
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}

#include "PurchaseSuccessfulDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ObjMesh.hpp"
#include "TextComponent.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

namespace {
    constexpr auto glowLinesRotationSpeed {12.0f};
}

PurchaseSuccessfulDialogView::PurchaseSuccessfulDialogView(Pht::IEngine& engine,
                                                           const CommonResources& commonResources,
                                                           PotentiallyZoomedScreen zoom) :
    mEngine {engine} {

    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetMediumDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-4.2f, 5.05f, UiLayer::text}, "PURCHASE SUCCESSFUL", textProperties);
    
    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);
    GuiUtils::CreateTitleBarLine(engine, *this);

    auto coinXPosition = -0.45f;
    auto& coin =
        CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                          commonResources.GetMaterials().GetLightGoldMaterial(),
                          engine.GetSceneManager());
    auto& coinTransform = coin.GetTransform();
    coinTransform.SetPosition({coinXPosition, 1.5f, UiLayer::block});
    coinTransform.SetRotation({0.0f, 45.0f, 0.0f});
    coinTransform.SetScale(3.1f);
    GetRoot().AddChild(coin);
    
    CreateGlowEffect({coinXPosition, 1.5f, UiLayer::panel}, GetRoot(), 2.45f);
    
    auto& icon = GuiUtils::CreateIconWithShadow(engine,
                                                GetSceneResources(),
                                                "checkmark.png",
                                                {2.45f, 1.5f, UiLayer::text},
                                                {2.5f, 2.5f},
                                                GetRoot(),
                                                {0.325f, 0.76f, 0.325f, 1.0f},
                                                Pht::Vec4{0.0f, 0.0f, 0.0f, 0.35f},
                                                Pht::Vec3{-0.05f, -0.15f, UiLayer::textShadow});
    GetRoot().AddChild(icon);

    mConfirmationText = &CreateText({-5.5f, -1.75f, UiLayer::text}, "", textProperties);

    Pht::Vec2 okButtonInputSize {194.0f, 43.0f};

    MenuButton::Style okButtonStyle;
    okButtonStyle.mRenderableObject = &guiResources.GetMediumGreenGlossyButton(zoom);
    okButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkGreenGlossyButton(zoom);
    okButtonStyle.mPressedScale = 1.05f;

    mOkButton = std::make_unique<MenuButton>(engine,
                                             *this,
                                             Pht::Vec3 {0.0f, -4.75f, UiLayer::textRectangle},
                                             okButtonInputSize,
                                             okButtonStyle);
    mOkButton->CreateText({-0.5f, -0.23f, UiLayer::buttonText},
                          "OK",
                          guiResources.GetWhiteButtonTextProperties(zoom));
}

void PurchaseSuccessfulDialogView::CreateGlowEffect(const Pht::Vec3& position,
                                                    Pht::SceneObject& parentObject,
                                                    float scale) {
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
        .mColor = Pht::Vec4{0.43f, 0.43f, 0.43f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare24.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocity = glowLinesRotationSpeed,
        .mZAngularVelocityRandomPart = glowLinesRotationSpeed * 0.5f,
        .mSize = Pht::Vec2{8.0f, 8.0f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.0f
    };
    
    auto& particleSystem = mEngine.GetParticleSystem();
    mGlowEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                 particleEmitterSettings,
                                                                 Pht::RenderMode::Triangles);
    auto& material = mGlowEffect->GetRenderable()->GetMaterial();
    material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);

    auto& transform = mGlowEffect->GetTransform();
    transform.SetPosition(position);
    transform.SetScale(scale);
    parentObject.AddChild(*mGlowEffect);
}

void PurchaseSuccessfulDialogView::SetUp(int numCoins) {
    mConfirmationText->GetText() = "You have received " + std::to_string(numCoins) + " gold coins!";
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
}

void PurchaseSuccessfulDialogView::Update() {
    auto dt = mEngine.GetLastFrameSeconds();
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
}

#include "NoLivesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"
#include "ISceneManager.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "UserServices.hpp"
#include "StringUtils.hpp"
#include "UiLayer.hpp"
#include "IGuiLightProvider.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

namespace {
    constexpr auto countdownNumChars = 5;
    constexpr auto animationDuration = 3.8f;
    constexpr auto rotationAmplitude = 25.0f;
    constexpr auto heartBeatAnimationDuration = 1.0f;
    constexpr auto heartBeatAmplitude = 0.03f;
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

NoLivesDialogView::NoLivesDialogView(Pht::IEngine& engine,
                                     const CommonResources& commonResources,
                                     const UserServices& userServices,
                                     PotentiallyZoomedScreen zoom) :
    mEngine {engine},
    mUserServices {userServices} {
    
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetLargeDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-2.1f, 8.25f, UiLayer::text},
               "NO LIVES",
               guiResources.GetLargeWhiteTextProperties(zoom));
    
    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager = engine.GetSceneManager();
    auto& lineSceneObject =
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager);
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    CreateNoLivesIcon({0.0f, 5.1f, UiLayer::root}, commonResources, zoom);

    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    
    CreateText({-4.5f, 2.3f, UiLayer::text}, "Time until next life:", textProperties);
    mCountdownText = &CreateText({2.3f, 2.3f, UiLayer::text}, "00:00", textProperties);

    auto& line2SceneObject =
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager);
    line2SceneObject.GetTransform().SetPosition({0.0f, 1.6f, UiLayer::textRectangle});
    GetRoot().AddChild(line2SceneObject);
    
    CreateAddLivesIcon({0.0f, -0.9f, UiLayer::root}, commonResources, zoom);

    CreateText({-4.9f, -4.0f, UiLayer::text}, "Get 5 more lives and continue", textProperties);
    CreateText({-1.3f, -5.075f, UiLayer::text}, "playing", textProperties);

    Pht::Vec2 refillLivesInputSize {194.0f, 50.0f};

    MenuButton::Style refillLivesButtonStyle;
    refillLivesButtonStyle.mPressedScale = 1.05f;
    refillLivesButtonStyle.mTextScale = 1.05f;
    refillLivesButtonStyle.mRenderableObject = &guiResources.GetLargeBlueGlossyButton(zoom);
    refillLivesButtonStyle.mSelectedRenderableObject = &guiResources.GetLargeDarkBlueGlossyButton(zoom);

    mRefillLivesButton = std::make_unique<MenuButton>(engine,
                                                      *this,
                                                      Pht::Vec3 {0.0f, -7.9f, UiLayer::textRectangle},
                                                      refillLivesInputSize,
                                                      refillLivesButtonStyle);
    mRefillLivesButton->CreateIcon("play.png",
                                   {-3.3f, 0.0f, UiLayer::buttonText},
                                   {0.7f, 0.7f},
                                   {1.0f, 1.0f, 1.0f, 1.0f},
                                   Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                                   Pht::Vec3 {-0.05f, -0.05f, UiLayer::textShadow});

    Pht::TextProperties buttonTextProperties {
        commonResources.GetHussarFontSize27(zoom),
        1.05f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };

    mRefillLivesButton->CreateText({-2.7f, -0.24f, UiLayer::buttonText},
                                   "CONTINUE           " + std::to_string(PurchasingService::refillLivesPriceInCoins),
                                   buttonTextProperties);

    auto& coin =
        CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                          commonResources.GetMaterials().GetGoldMaterial(),
                          engine.GetSceneManager());
    auto& coinTransform = coin.GetTransform();
    coinTransform.SetPosition({2.25f, 0.0f, UiLayer::buttonOverlayObject2});
    coinTransform.SetRotation({0.0f, 40.0f, 0.0f});
    coinTransform.SetScale(0.9f);
    mRefillLivesButton->GetSceneObject().AddChild(coin);
    
    Pht::Material shaddowMaterial {Pht::Color{0.05f, 0.05f, 0.05f}};
    shaddowMaterial.SetOpacity(0.14f);
    shaddowMaterial.GetDepthState().mDepthTestAllowedOverride = true;
    auto& coinShaddow =
        CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                          shaddowMaterial,
                          engine.GetSceneManager());
    auto& coinShaddowTransform = coinShaddow.GetTransform();
    coinShaddowTransform.SetPosition({2.12f, -0.12f, UiLayer::buttonOverlayObject1});
    coinShaddowTransform.SetRotation({0.0f, 40.0f, 0.0f});
    coinShaddowTransform.SetScale(0.9f);
    mRefillLivesButton->GetSceneObject().AddChild(coinShaddow);
}

void NoLivesDialogView::CreateNoLivesIcon(const Pht::Vec3& position,
                                          const CommonResources& commonResources,
                                          PotentiallyZoomedScreen zoom) {
    auto& container = CreateSceneObject();
    container.GetTransform().SetPosition(position);
    GetRoot().AddChild(container);

    auto& sceneManager = mEngine.GetSceneManager();
    auto& heart =
        CreateSceneObject(Pht::ObjMesh {"heart_392.obj", 8.5f},
                          commonResources.GetMaterials().GetRedMaterial(),
                          sceneManager);
    container.AddChild(heart);
    
    CreateGlowEffect(container, 0.9f);
    
    Pht::TextProperties zeroTextProperties {
        commonResources.GetHussarFontSize35(zoom),
        1.1f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.27f, 0.27f, 0.27f, 0.5f}
    };
    
    CreateText({-0.4, -0.4f, UiLayer::text}, "0", zeroTextProperties, heart);
}

void NoLivesDialogView::CreateAddLivesIcon(const Pht::Vec3& position,
                                           const CommonResources& commonResources,
                                           PotentiallyZoomedScreen zoom) {
    auto& container = CreateSceneObject();
    container.GetTransform().SetPosition(position);
    GetRoot().AddChild(container);
    
    auto& sceneManager = mEngine.GetSceneManager();

    mHeartSceneObject = &CreateSceneObject(Pht::ObjMesh {"heart_392.obj", 9.75f},
                                           commonResources.GetMaterials().GetRedMaterial(),
                                           sceneManager);
    container.AddChild(*mHeartSceneObject);

    CreateGlowEffect(container, 1.35f);
    CreateParticles(container, 1.35f);
    
    Pht::TextProperties zeroTextProperties {
        commonResources.GetHussarFontSize35(zoom),
        1.1f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.27f, 0.27f, 0.27f, 0.5f}
    };
    
    CreateText({-0.775f, -0.6f, UiLayer::text}, "+5", zeroTextProperties, container);
}

void NoLivesDialogView::CreateGlowEffect(Pht::SceneObject& parentObject, float scale) {
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
    auto glowEffect =
        particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                       particleEmitterSettings,
                                                       Pht::RenderMode::Triangles);
    auto& material = glowEffect->GetRenderable()->GetMaterial();
    material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
    
    auto& transform = glowEffect->GetTransform();
    transform.SetPosition({0.0f, 0.2f, UiLayer::panel});
    transform.SetScale(scale);
    parentObject.AddChild(*glowEffect);
    
    mParticleEffects.push_back(std::move(glowEffect));
}

void NoLivesDialogView::CreateParticles(Pht::SceneObject& parentObject, float scale) {
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
    auto particles =
        particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                       particleEmitterSettings,
                                                       Pht::RenderMode::Triangles);
    auto& transform = particles->GetTransform();
    transform.SetPosition({0.0f, 0.2f, UiLayer::panel});
    transform.SetScale(scale);
    parentObject.AddChild(*particles);
    
    mParticleEffects.push_back(std::move(particles));
    
}

void NoLivesDialogView::SetUp() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
    
    for (auto& particleEffect: mParticleEffects) {
        particleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    }

    mAnimationTime = 0.0f;
}

void NoLivesDialogView::Update() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    for (auto& particleEffect: mParticleEffects) {
        particleEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    }

    UpdateCountdownText();
    UpdateHeartAnimation(dt);
    UpdateHeartBeatAnimation(dt);
}

void NoLivesDialogView::UpdateCountdownText() {
    auto secondsUntilNewLife = mUserServices.GetLifeService().GetDurationUntilNewLife();
    
    if (secondsUntilNewLife != mSecondsUntilNewLife) {
        StaticStringBuffer countdownBuffer;
        StringUtils::FormatToMinutesAndSeconds(countdownBuffer, secondsUntilNewLife);
        
        auto numChars = std::strlen(countdownBuffer.data());
        assert(numChars == countdownNumChars);
        
        auto& text = mCountdownText->GetText();
        auto textLength = text.size();
        assert(textLength == countdownNumChars);
        
        StringUtils::StringCopy(text, 0, countdownBuffer, countdownNumChars);
        
        mSecondsUntilNewLife = secondsUntilNewLife;
    }
}

void NoLivesDialogView::UpdateHeartAnimation(float dt) {
    mAnimationTime += dt;

    if (mAnimationTime > animationDuration) {
        mAnimationTime = 0.0f;
    }

    auto t = mAnimationTime * 2.0f * 3.1415f / animationDuration;
    auto yAngle = rotationAmplitude * std::sin(t);

    mHeartSceneObject->GetTransform().SetRotation({0.0f, yAngle, 0.0f});
};

void NoLivesDialogView::UpdateHeartBeatAnimation(float dt) {
    mHeartBeatAnimationTime += dt;
    
    if (mHeartBeatAnimationTime > heartBeatAnimationDuration) {
        mHeartBeatAnimationTime = 0.0f;
    }
    
    auto t = mHeartBeatAnimationTime * 2.0f * 3.1415f / heartBeatAnimationDuration;
    mHeartSceneObject->GetTransform().SetScale(1.0f + heartBeatAmplitude * std::sin(t));
}

#include "LivesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "ObjMesh.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"

// Game includes.
#include "UserServices.hpp"
#include "CommonResources.hpp"
#include "StringUtils.hpp"
#include "UiLayer.hpp"
#include "IGuiLightProvider.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

namespace {
    constexpr auto countdownNumChars = 5;
    constexpr auto heartScale = 3.0f;
    constexpr auto animationDuration = 1.0f;
    constexpr auto heartBeatAmplitude = 0.1f;
}

LivesDialogView::LivesDialogView(Pht::IEngine& engine,
                                 const CommonResources& commonResources,
                                 const UserServices& userServices,
                                 IGuiLightProvider& guiLightProvider) :
    mEngine {engine},
    mUserServices {userServices},
    mGuiLightProvider {guiLightProvider} {
    
    auto zoom = PotentiallyZoomedScreen::No;
    auto& guiResources = commonResources.GetGuiResources();
    
    auto& menuWindow = guiResources.GetMediumDarkMenuWindow();
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    mCaptionText = &CreateText({-1.7f, 4.95f, UiLayer::text},
                               "0 LIVES",
                               guiResources.GetLargeWhiteTextProperties(zoom));

    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);
    GuiUtils::CreateTitleBarLine(engine, *this);
    
    auto& sceneManager = engine.GetSceneManager();
    mHeartSceneObject = &CreateSceneObject(Pht::ObjMesh {"heart_392.obj", 3.25f},
                                           commonResources.GetMaterials().GetRedMaterial(),
                                           sceneManager);
    auto& heartTransform = mHeartSceneObject->GetTransform();
    heartTransform.SetPosition({0.0f, 1.5f, UiLayer::root});
    heartTransform.SetScale(heartScale);
    GetRoot().AddChild(*mHeartSceneObject);
    
    Pht::TextProperties zeroTextProperties {
        commonResources.GetHussarFontSize35(zoom),
        1.1f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.27f, 0.27f, 0.27f, 0.5f}
    };
    
    mNumLivesText = &CreateText({-0.4, 1.2f, UiLayer::text}, "0", zeroTextProperties);
    
    CreateGlowEffect({0.0f, 1.5f, UiLayer::panel}, GetRoot(), 1.35f);
    
    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    
    CreateText({-4.4f, -1.5f, UiLayer::text}, "Time until next life:", textProperties);
    mCountdownText = &CreateText({2.4f, -1.5f, UiLayer::text}, "00:00", textProperties);
    mFullText = &CreateText({2.5f, -1.5f, UiLayer::text}, "full", textProperties);

    Pht::Vec2 okButtonInputSize {194.0f, 43.0f};

    MenuButton::Style okButtonStyle;
    okButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    okButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);
    okButtonStyle.mPressedScale = 1.05f;

    mOkButton = std::make_unique<MenuButton>(engine,
                                             *this,
                                             Pht::Vec3 {0.0f, -4.7f, UiLayer::textRectangle},
                                             okButtonInputSize,
                                             okButtonStyle);
    mOkButton->CreateText({-0.5f, -0.23f, UiLayer::buttonText},
                          "OK",
                          guiResources.GetWhiteButtonTextProperties(zoom));
}

void LivesDialogView::CreateGlowEffect(const Pht::Vec3& position,
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
    
    auto& transform = mGlowEffect->GetTransform();
    transform.SetPosition(position);
    transform.SetScale(scale);
    parentObject.AddChild(*mGlowEffect);
}

void LivesDialogView::SetUp() {
    mGuiLightProvider.SetDefaultGuiLightDirections();
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
}

void LivesDialogView::Update() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    UpdateNumLivesTexts();
    UpdateCountdownText();
    UpdateHeart(dt);
}

void LivesDialogView::UpdateNumLivesTexts() {
    auto numLives = mUserServices.GetLifeService().GetNumLives();
    if (numLives != mNumLives) {
        mNumLives = numLives;
        auto numLivesString = std::to_string(numLives);
        if (numLives == 1) {
            mCaptionText->GetText() = "1 LIFE";
            mCaptionText->GetSceneObject().GetTransform().SetPosition({-1.2f, 4.95f, UiLayer::text});
        } else {
            mCaptionText->GetText() = numLivesString + " LIVES";
            mCaptionText->GetSceneObject().GetTransform().SetPosition({-1.6f, 4.95f, UiLayer::text});
        }
        
        mNumLivesText->GetText() = numLivesString;
    }
}

void LivesDialogView::UpdateCountdownText() {
    auto& lifeService = mUserServices.GetLifeService();
    if (lifeService.HasFullNumLives()) {
        mCountdownText->GetSceneObject().SetIsVisible(false);
        mFullText->GetSceneObject().SetIsVisible(true);
    } else {
        mCountdownText->GetSceneObject().SetIsVisible(true);
        mFullText->GetSceneObject().SetIsVisible(false);
        
        auto secondsUntilNewLife = lifeService.GetDurationUntilNewLife();
        if (secondsUntilNewLife != mSecondsUntilNewLife) {
            mSecondsUntilNewLife = secondsUntilNewLife;
            
            StaticStringBuffer countdownBuffer;
            StringUtils::FormatToMinutesAndSeconds(countdownBuffer, secondsUntilNewLife);
            
            auto numChars = std::strlen(countdownBuffer.data());
            assert(numChars == countdownNumChars);
            
            auto& text = mCountdownText->GetText();
            auto textLength = text.size();
            assert(textLength == countdownNumChars);
            
            StringUtils::StringCopy(text, 0, countdownBuffer, countdownNumChars);
        }
    }
}

void LivesDialogView::UpdateHeart(float dt) {
    mAnimationTime += dt;
    if (mAnimationTime > animationDuration) {
        mAnimationTime = 0.0f;
    }
    
    auto t = mAnimationTime * 2.0f * 3.1415f / animationDuration;
    mHeartSceneObject->GetTransform().SetScale(heartScale + heartBeatAmplitude * std::sin(t));
}

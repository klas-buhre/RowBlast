#include "LivesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"

// Game includes.
#include "UserServices.hpp"
#include "CommonResources.hpp"
#include "StringUtils.hpp"
#include "UiLayer.hpp"
#include "IGuiLightProvider.hpp"

using namespace RowBlast;

namespace {
    constexpr auto countdownNumChars {5};
    const Pht::Vec3 uiLightDirection {0.9f, 1.0f, 1.0f};
    constexpr auto heartScale {3.0f};
    constexpr auto animationDuration {1.0f};
    constexpr auto heartBeatAmplitude {0.1f};
}

LivesDialogView::LivesDialogView(Pht::IEngine& engine,
                                 const CommonResources& commonResources,
                                 const UserServices& userServices,
                                 IGuiLightProvider& guiLightProvider) :
    mEngine {engine},
    mUserServices {userServices},
    mGuiLightProvider {guiLightProvider} {
    
    auto zoom {PotentiallyZoomedScreen::No};
    
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumDarkMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    mCaptionText = &CreateText({-1.7f, 4.95f, UiLayer::text},
                               "0 LIVES",
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
    
    mHeartSceneObject = &CreateSceneObject(Pht::ObjMesh {"heart_112.obj", 3.25f},
                                           commonResources.GetMaterials().GetRedMaterial(),
                                           sceneManager);
    auto& heartTransform {mHeartSceneObject->GetTransform()};
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
    
    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    
    CreateText({-4.5f, -1.5f, UiLayer::text}, "Time until next life:", textProperties);
    mCountdownText = &CreateText({2.3f, -1.5f, UiLayer::text}, "00:00", textProperties);
    mFullText = &CreateText({2.3f, -1.5f, UiLayer::text}, "full", textProperties);

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

void LivesDialogView::SetUp() {
    mGuiLightProvider.SetGuiLightDirections(uiLightDirection, uiLightDirection);
    Update();
}

void LivesDialogView::Update() {
    UpdateNumLivesTexts();
    UpdateCountdownText();
    UpdateHeart();
}

void LivesDialogView::UpdateNumLivesTexts() {
    auto numLives {mUserServices.GetLifeService().GetNumLives()};

    if (numLives != mNumLives) {
        mNumLives = numLives;
        auto numLivesString {std::to_string(numLives)};
        
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
    auto& lifeService {mUserServices.GetLifeService()};
    
    if (lifeService.HasFullNumLives()) {
        mCountdownText->GetSceneObject().SetIsVisible(false);
        mFullText->GetSceneObject().SetIsVisible(true);
    } else {
        mCountdownText->GetSceneObject().SetIsVisible(true);
        mFullText->GetSceneObject().SetIsVisible(false);
        
        auto secondsUntilNewLife {lifeService.GetDurationUntilNewLife()};

        if (secondsUntilNewLife != mSecondsUntilNewLife) {
            mSecondsUntilNewLife = secondsUntilNewLife;
            
            StaticStringBuffer countdownBuffer;
            StringUtils::FormatToMinutesAndSeconds(countdownBuffer, secondsUntilNewLife);
            
            auto numChars {std::strlen(countdownBuffer.data())};
            assert(numChars == countdownNumChars);
            
            auto& text {mCountdownText->GetText()};
            auto textLength {text.size()};
            assert(textLength == countdownNumChars);
            
            StringUtils::StringCopy(text, 0, countdownBuffer, countdownNumChars);
        }
    }
}

void LivesDialogView::UpdateHeart() {
    mAnimationTime += mEngine.GetLastFrameSeconds();
    
    if (mAnimationTime > animationDuration) {
        mAnimationTime = 0.0f;
    }
    
    auto t {mAnimationTime * 2.0f * 3.1415f / animationDuration};
    mHeartSceneObject->GetTransform().SetScale(heartScale + heartBeatAmplitude * std::sin(t));
}

void LivesDialogView::OnDeactivate() {
    mGuiLightProvider.SetDefaultGuiLightDirections();
}

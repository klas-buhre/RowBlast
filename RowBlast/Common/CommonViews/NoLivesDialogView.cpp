#include "NoLivesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "MenuQuad.hpp"
#include "UserData.hpp"
#include "StringUtils.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto countdownNumChars {5};
}

NoLivesDialogView::NoLivesDialogView(Pht::IEngine& engine,
                                     const CommonResources& commonResources,
                                     const UserData& userData,
                                     PotentiallyZoomedScreen zoom) :
    mUserData {userData} {
    
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-2.2f, 5.0f, UiLayer::text},
               "NO LIVES",
               guiResources.GetCaptionTextProperties(zoom));
    
    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.5f,
        GetSize().y / 2.0f - 1.5f,
        UiLayer::textRectangle
    };
    
    Pht::Vec2 closeButtonInputSize {78.0f, 43.0f};
    
    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mMeshFilename = GuiResources::mCloseButtonMeshFilename;
    closeButtonStyle.mColor = GuiResources::mYellowButtonColor;
    closeButtonStyle.mSelectedColor = GuiResources::mYellowSelectedButtonColor;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mHasShadow = true;
    
    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);
    mCloseButton->CreateText({-0.33f, -0.33f, UiLayer::text},
                             "X",
                             guiResources.GetLargeBlackButtonTextProperties(zoom));
      
    Pht::Vec2 refillLivesInputSize {180.0f, 47.0f};
    
    MenuButton::Style refillLivesButtonStyle;
    refillLivesButtonStyle.mMeshFilename = GuiResources::mMediumButtonMeshFilename;
    refillLivesButtonStyle.mColor = GuiResources::mGreenButtonColor;
    refillLivesButtonStyle.mSelectedColor = GuiResources::mGreenSelectedButtonColor;
    refillLivesButtonStyle.mPressedScale = 1.05f;
    refillLivesButtonStyle.mHasShadow = true;

    Pht::TextProperties whiteButtonTextProperties {
        commonResources.GetHussarFontSize35(zoom),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    };

    mRefillLivesButton = std::make_unique<MenuButton>(engine,
                                                      *this,
                                                      Pht::Vec3 {0.0f, -2.6f, UiLayer::textRectangle},
                                                      refillLivesInputSize,
                                                      refillLivesButtonStyle);
    mRefillLivesButton->CreateText({-2.5f, -0.23f, UiLayer::buttonText},
                                   "Refill Lives",
                                   whiteButtonTextProperties);
    mRefillLivesButton->GetSceneObject().GetTransform().SetScale({1.1f, 1.4f, 1.0f});
    
    auto& textProperties {guiResources.GetSmallTextProperties(zoom)};
    
    CreateText({-1.4f, 2.5f, UiLayer::text}, "Lives: 0", textProperties);
    CreateText({-3.0f, 1.0f, UiLayer::text}, "Refill lives for $1", textProperties);
    CreateText({-4.0f, 0.0f, UiLayer::text}, "Time to next life:", textProperties);
    mCountdownText = &CreateText({1.9f, 0.0f, UiLayer::text}, "00:00", textProperties);
}

void NoLivesDialogView::Update() {
    auto secondsUntilNewLife {mUserData.GetLifeManager().GetDurationUntilNewLife()};
    
    if (secondsUntilNewLife != mSecondsUntilNewLife) {
        StaticStringBuffer countdownBuffer;
        StringUtils::FormatToMinutesAndSeconds(countdownBuffer, secondsUntilNewLife);
        
        auto numChars {std::strlen(countdownBuffer.data())};
        assert(numChars == countdownNumChars);
        
        auto& text {mCountdownText->GetText()};
        auto textLength {text.size()};
        assert(textLength == countdownNumChars);
        
        StringUtils::StringCopy(text, 0, countdownBuffer, countdownNumChars);
        
        mSecondsUntilNewLife = secondsUntilNewLife;
    }
}

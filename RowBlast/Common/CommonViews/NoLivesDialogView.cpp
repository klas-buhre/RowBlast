#include "NoLivesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "TextComponent.hpp"

// Game includes.
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
    
    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};
    
    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mMeshFilename = GuiResources::mCloseButtonMeshFilename;
    closeButtonStyle.mColor = GuiResources::mBlackButtonColor;
    closeButtonStyle.mSelectedColor = GuiResources::mBlackSelectedButtonColor;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mHasShadow = true;
    
    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);
    mCloseButton->CreateText({-0.34f, -0.35f, UiLayer::text},
                             "X",
                             guiResources.GetLargeWhiteButtonTextProperties(zoom));
      
    Pht::Vec2 refillLivesInputSize {205.0f, 59.0f};
    
    MenuButton::Style refillLivesButtonStyle;
    refillLivesButtonStyle.mMeshFilename = GuiResources::mBigButtonMeshFilename;
    refillLivesButtonStyle.mColor = GuiResources::mGreenButtonColor;
    refillLivesButtonStyle.mSelectedColor = GuiResources::mGreenSelectedButtonColor;
    refillLivesButtonStyle.mPressedScale = 1.05f;
    refillLivesButtonStyle.mHasShadow = true;

    mRefillLivesButton = std::make_unique<MenuButton>(engine,
                                                      *this,
                                                      Pht::Vec3 {0.0f, -2.6f, UiLayer::textRectangle},
                                                      refillLivesInputSize,
                                                      refillLivesButtonStyle);
    mRefillLivesButton->CreateText({-1.4f, -0.31f, UiLayer::buttonText},
                                   "$0.99",
                                   guiResources.GetLargeWhiteButtonTextProperties(zoom));
    
    auto& textProperties {guiResources.GetSmallTextProperties(zoom)};
    
    CreateText({-1.4f, 2.5f, UiLayer::text}, "Lives: 0", textProperties);
    CreateText({-4.5f, 1.5f, UiLayer::text}, "Time until next life:", textProperties);
    mCountdownText = &CreateText({2.3f, 1.5f, UiLayer::text}, "00:00", textProperties);

    CreateText({-4.4f, -0.1f, UiLayer::text}, "Purchase 5 lives for $0.99", textProperties);
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

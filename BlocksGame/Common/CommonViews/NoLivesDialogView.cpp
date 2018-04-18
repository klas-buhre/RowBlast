#include "NoLivesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "CommonResources.hpp"
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
                                     const UserData& userData) :
    mUserData {userData} {
    
    Pht::TextProperties textProperties {commonResources.GetHussarFontSize27()};
    Pht::Vec2 size {engine.GetRenderer().GetHudFrustumSize().x, 9.0f};
    SetSize(size);
    SetPosition({0.0f, 0.0f});
    
    auto quad {MenuQuad::CreateGreen(engine, GetSceneResources(), size)};
    quad->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(quad));
    
    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.0f,
        GetSize().y / 2.0f - 1.0f,
        UiLayer::textRectangle
    };
    mCloseButton = std::make_unique<CloseButton>(engine, *this, closeButtonPosition, textProperties);
      
    Pht::Vec2 buttonSize {6.5f, 2.1f};
    Pht::Vec2 buttonInputSize {180.0f, 47.0f};
    MenuButton::Style buttonStyle;
    buttonStyle.mColor = Pht::Color {0.2f, 0.82f, 0.2f};
    buttonStyle.mSelectedColor = Pht::Color {0.23f, 1.0f, 0.23f};

    mRefillLivesButton = std::make_unique<MenuButton>(engine,
                                                      *this,
                                                      Pht::Vec3 {0.0f, -2.0f, UiLayer::textRectangle},
                                                      buttonSize,
                                                      buttonInputSize,
                                                      buttonStyle);
    mRefillLivesButton->CreateText({-2.0f, -0.23f, UiLayer::buttonText}, "REFILL LIVES", textProperties);
    
    CreateText({-2.4f, 2.5f, UiLayer::text}, "No more lives!", textProperties);
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

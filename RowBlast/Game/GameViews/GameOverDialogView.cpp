#include "GameOverDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "MenuQuad.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

GameOverDialogView::GameOverDialogView(Pht::IEngine& engine,
                                       const CommonResources& commonResources) {
    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes)
    };

    Pht::Vec2 size {engine.GetRenderer().GetHudFrustumSize().x, 7.0f};
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
        
    Pht::Vec2 buttonInputSize {180.0f, 47.0f};
    MenuButton::Style buttonStyle;
    buttonStyle.mColor = Pht::Color {0.2f, 0.82f, 0.2f};
    buttonStyle.mSelectedColor = Pht::Color {0.23f, 1.0f, 0.23f};

    mRetryButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                Pht::Vec3 {0.0f, -1.0f, UiLayer::textRectangle},
                                                buttonInputSize,
                                                buttonStyle);
    mRetryButton->CreateText({-1.05f, -0.23f, UiLayer::buttonText}, "RETRY", textProperties);
    
    CreateText({-2.0f, 1.5f, UiLayer::text}, "Game Over!", textProperties);
}

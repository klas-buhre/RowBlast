#include "GameOverDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "MenuQuad.hpp"

using namespace BlocksGame;

GameOverDialogView::GameOverDialogView(Pht::IEngine& engine,
                                       const CommonResources& commonResources) :
    Pht::GuiView {false} {
    
    Pht::TextProperties textProperties {commonResources.GetHussarFontSize27()};
    Pht::Vec2 size {engine.GetRenderer().GetHudFrustumSize().x, 7.0f};
    SetSize(size);
    SetPosition({0.0f, 0.0f});
    
    auto quad {MenuQuad::CreateGreen(engine, GetSceneResources(), size)};
    quad->SetPosition({0.0f, 0.0f, -1.0f});
    AddSceneObject(std::move(quad));
    
    Pht::Vec3 closeButtonPosition {GetSize().x / 2.0f - 1.0f, GetSize().y / 2.0f - 1.0f, -0.5f};
    mCloseButton = std::make_unique<CloseButton>(engine, *this, closeButtonPosition);
    mCloseButton->SetText(
        std::make_unique<Pht::Text>(Pht::Vec2 {-0.23f, -0.23f}, "X", textProperties));
        
    Pht::Vec2 buttonSize {6.5f, 2.1f};
    Pht::Vec2 buttonInputSize {180.0f, 47.0f};
    MenuButton::Style buttonStyle;
    buttonStyle.mColor = Pht::Color {0.2f, 0.82f, 0.2f};
    buttonStyle.mSelectedColor = Pht::Color {0.23f, 1.0f, 0.23f};

    mRetryButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                Pht::Vec3 {0.0f, -1.0f, -0.5f},
                                                buttonSize,
                                                buttonInputSize,
                                                buttonStyle);
    mRetryButton->SetText(
        std::make_unique<Pht::Text>(Pht::Vec2 {-1.05f, -0.23f}, "RETRY", textProperties));
    
    auto textLine {
        std::make_unique<Pht::Text>(Pht::Vec2 {-2.0f, 1.5f}, "Game Over!", textProperties)
    };
    
    AddText(std::move(textLine));
}

#include "NoMoreLevelsDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "GuiUtils.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

NoMoreLevelsDialogView::NoMoreLevelsDialogView(Pht::IEngine& engine,
                                               const CommonResources& commonResources) {
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetMediumDarkMenuWindow();
    auto zoom = PotentiallyZoomedScreen::No;
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));
    
    SetSize(menuWindow.GetSize());
    
    auto textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    textProperties.mAlignment = Pht::TextAlignment::CenterX;
    CreateText({0.0f, 5.05f, UiLayer::text}, "CONGRATULATIONS!", textProperties);
    
    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);
    GuiUtils::CreateTitleBarLine(engine, *this);
    
    auto& icon = GuiUtils::CreateIconWithShadow(engine,
                                                GetSceneResources(),
                                                "checkmark.png",
                                                {0.0f, 1.9f, UiLayer::text},
                                                {3.5f, 3.5f},
                                                GetRoot(),
                                                {0.325f, 0.76f, 0.325f, 1.0f},
                                                Pht::Vec4{0.0f, 0.0f, 0.0f, 0.35f},
                                                Pht::Vec3{-0.05f, -0.15f, UiLayer::textShadow});
    GetRoot().AddChild(icon);
    
    CreateText({0.0f, -1.0f, UiLayer::text}, "You have completed all levels!", textProperties);
    CreateText({0.0f, -2.075f, UiLayer::text}, "Stay tuned for updates.", textProperties);
    
    Pht::Vec2 okButtonInputSize {194.0f, 43.0f};
    
    MenuButton::Style okButtonStyle;
    okButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    okButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);
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

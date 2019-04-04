#include "PurchaseFailedDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

PurchaseFailedDialogView::PurchaseFailedDialogView(Pht::IEngine& engine,
                                                   const CommonResources& commonResources,
                                                   PotentiallyZoomedScreen zoom) {
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetSmallDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-3.0f, 3.825f, UiLayer::text}, "PURCHASE FAILED", textProperties);
    
    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);
    GuiUtils::CreateTitleBarLine(engine, *this);
    
    CreateText({-4.95f, 0.5f, UiLayer::text}, "No money was removed from", textProperties);
    CreateText({-2.3f, -0.575f, UiLayer::text}, " your account.", textProperties);

    Pht::Vec2 okButtonInputSize {194.0f, 43.0f};

    MenuButton::Style okButtonStyle;
    okButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    okButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);
    okButtonStyle.mPressedScale = 1.05f;

    mOkButton = std::make_unique<MenuButton>(engine,
                                             *this,
                                             Pht::Vec3 {0.0f, -3.5f, UiLayer::textRectangle},
                                             okButtonInputSize,
                                             okButtonStyle);
    mOkButton->CreateText({-0.5f, -0.23f, UiLayer::buttonText},
                          "OK",
                          guiResources.GetWhiteButtonTextProperties(zoom));
}

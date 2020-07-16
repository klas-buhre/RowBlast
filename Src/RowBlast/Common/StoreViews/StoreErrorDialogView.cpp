#include "StoreErrorDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

StoreErrorDialogView::StoreErrorDialogView(Pht::IEngine& engine,
                                           const CommonResources& commonResources,
                                           const std::string& caption,
                                           const std::vector<std::string>& textLines,
                                           PotentiallyZoomedScreen zoom) {
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetMediumDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    textProperties.mAlignment = Pht::TextAlignment::CenterX;
    CreateText({0.0f, 5.05f, UiLayer::text}, caption, textProperties);
    
    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);
    GuiUtils::CreateTitleBarLine(engine, *this);
    
    GuiUtils::CreateIcon(engine,
                         *this,
                         "error.png",
                         {0.0f, 1.9f, UiLayer::text},
                         {3.5f, 3.5f},
                         GetRoot(),
                         {1.0f, 0.43f, 0.43f, 1.0f});
    
    switch (textLines.size()) {
        case 2:
            CreateText({0.0f, -1.0f, UiLayer::text}, textLines[0], textProperties);
            CreateText({0.0f, -2.075f, UiLayer::text}, textLines[1], textProperties);
            break;
        default:
            break;
    }

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

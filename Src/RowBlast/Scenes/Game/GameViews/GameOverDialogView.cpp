#include "GameOverDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

GameOverDialogView::GameOverDialogView(Pht::IEngine& engine,
                                       const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetSmallDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-2.8f, 3.75f, UiLayer::text},
               "GAME OVER",
               guiResources.GetLargeWhiteTextProperties(zoom));
    
    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);
    GuiUtils::CreateTitleBarLine(engine, *this);

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-3.1f, 0.0f, UiLayer::text}, "No room to spawn.", textProperties);

    Pht::Vec2 retryButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style retryButtonStyle;
    retryButtonStyle.mRenderableObject = &guiResources.GetLargeGreenGlossyButton(zoom);
    retryButtonStyle.mSelectedRenderableObject = &guiResources.GetLargeDarkGreenGlossyButton(zoom);
    retryButtonStyle.mPressedScale = 1.05f;
    retryButtonStyle.mTextScale = 1.1f;

    mRetryButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                Pht::Vec3 {0.0f, -3.45f, UiLayer::textRectangle},
                                                retryButtonInputSize,
                                                retryButtonStyle);

    Pht::TextProperties buttonTextProperties {
        commonResources.GetHussarFontSize27(zoom),
        1.1f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };

    mRetryButton->CreateText({-1.2f, -0.25f, UiLayer::buttonText}, "RETRY", buttonTextProperties);
}

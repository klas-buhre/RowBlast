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
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetSmallMenuWindowPotentiallyZoomedScreen()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    CreateText({-2.8f, 3.5f, UiLayer::text},
               "GAME OVER",
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

    Pht::Vec2 retryButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style retryButtonStyle;
    retryButtonStyle.mMeshFilename = GuiResources::mBigButtonMeshFilename;
    retryButtonStyle.mColor = GuiResources::mGreenButtonColor;
    retryButtonStyle.mSelectedColor = GuiResources::mGreenSelectedButtonColor;
    retryButtonStyle.mPressedScale = 1.05f;
    retryButtonStyle.mHasShadow = true;

    mRetryButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                Pht::Vec3 {0.0f, -1.0f, UiLayer::textRectangle},
                                                retryButtonInputSize,
                                                retryButtonStyle);
    mRetryButton->CreateText({-1.25f, -0.31f, UiLayer::buttonText},
                            "Retry",
                            guiResources.GetLargeWhiteButtonTextProperties(zoom));
}

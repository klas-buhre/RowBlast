#include "NoMovesDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

NoMovesDialogView::NoMovesDialogView(Pht::IEngine& engine, const CommonResources& commonResources) {
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetSmallMenuWindowPotentiallyZoomedScreen()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    CreateText({-2.5f, 3.5f, UiLayer::text},
               "NO MOVES",
               guiResources.GetCaptionTextProperties(zoom));
    
    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.5f,
        GetSize().y / 2.0f - 1.5f,
        UiLayer::textRectangle
    };
    
    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};
    
    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mRenderableObject = &guiResources.GetCloseButton(zoom);
    
    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);

    auto& textProperties {guiResources.GetSmallTextProperties(zoom)};
    CreateText({-5.5f, 1.0f, UiLayer::text}, "Purchase 5 more moves for $0.99", textProperties);
    
    Pht::Vec2 playOnButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style playOnButtonStyle;
    playOnButtonStyle.mMeshFilename = GuiResources::mBigButtonMeshFilename;
    playOnButtonStyle.mColor = GuiResources::mGreenButtonColor;
    playOnButtonStyle.mSelectedColor = GuiResources::mGreenSelectedButtonColor;
    playOnButtonStyle.mPressedScale = 1.05f;
    playOnButtonStyle.mHasShadow = true;

    mPlayOnButton = std::make_unique<MenuButton>(engine,
                                                 *this,
                                                 Pht::Vec3 {0.0f, -1.5f, UiLayer::textRectangle},
                                                 playOnButtonInputSize,
                                                 playOnButtonStyle);
    mPlayOnButton->CreateText({-1.4f, -0.31f, UiLayer::buttonText},
                             "$0.99",
                             guiResources.GetLargeWhiteButtonTextProperties(zoom));
}

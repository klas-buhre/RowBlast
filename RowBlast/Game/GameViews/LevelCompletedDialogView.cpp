#include "LevelCompletedDialogView.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

LevelCompletedDialogView::LevelCompletedDialogView(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-3.4f, 5.1f, UiLayer::text},
               "LEVEL CLEARED",
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

    Pht::Vec2 nextButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style nextButtonStyle;
    nextButtonStyle.mMeshFilename = GuiResources::mBigButtonMeshFilename;
    nextButtonStyle.mColor = GuiResources::mBlueButtonColor;
    nextButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    nextButtonStyle.mPressedScale = 1.05f;
    nextButtonStyle.mHasShadow = true;

    mNextButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -2.9f, UiLayer::textRectangle},
                                               nextButtonInputSize,
                                               nextButtonStyle);
    mNextButton->CreateText({-1.1f, -0.31f, UiLayer::buttonText},
                            "Next",
                            guiResources.GetLargeWhiteButtonTextProperties(zoom));
}

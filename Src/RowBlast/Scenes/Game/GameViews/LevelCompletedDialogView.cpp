#include "LevelCompletedDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

LevelCompletedDialogView::LevelCompletedDialogView(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-3.7f, 4.95f, UiLayer::text},
               "LEVEL CLEARED!",
               guiResources.GetLargeWhiteTextProperties(zoom));

    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);
    GuiUtils::CreateTitleBarLine(engine, *this);

    Pht::Vec2 nextButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style nextButtonStyle;
    nextButtonStyle.mRenderableObject = &guiResources.GetLargerBlueGlossyButton(zoom);
    nextButtonStyle.mSelectedRenderableObject = &guiResources.GetLargerDarkBlueGlossyButton(zoom);
    nextButtonStyle.mPressedScale = 1.05f;

    mNextButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -4.3f, UiLayer::textRectangle},
                                               nextButtonInputSize,
                                               nextButtonStyle);
    mNextButton->CreateText({-1.1f, -0.31f, UiLayer::buttonText},
                            "NEXT",
                            guiResources.GetLargeWhiteButtonTextProperties(zoom));
}

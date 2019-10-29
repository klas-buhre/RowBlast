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
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetSmallDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetPosition({0.0f, -2.5f});
    SetSize(menuWindow.GetSize());
    
    CreateText({-3.5f, 3.95f, UiLayer::text},
               "LEVEL CLEARED!",
               guiResources.GetLargeWhiteTextProperties(zoom));

    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);

    Pht::Vec2 nextButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style nextButtonStyle;
    nextButtonStyle.mRenderableObject = &guiResources.GetLargerBlueGlossyButton(zoom);
    nextButtonStyle.mSelectedRenderableObject = &guiResources.GetLargerDarkBlueGlossyButton(zoom);
    nextButtonStyle.mPressedScale = 1.05f;

    mNextButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -3.45f, UiLayer::textRectangle},
                                               nextButtonInputSize,
                                               nextButtonStyle);
    mNextButton->CreateIcon("play.png",
                            {-1.42f, 0.0f, UiLayer::buttonText},
                            {0.8f, 0.8f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                            Pht::Vec3 {-0.05f, -0.05f, UiLayer::textShadow});
    mNextButton->CreateText({-0.8f, -0.31f, UiLayer::buttonText},
                            "NEXT",
                            guiResources.GetLargeWhiteButtonTextProperties(zoom));
}

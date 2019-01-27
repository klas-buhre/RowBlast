#include "SwitchPiece2WindowView.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

SwitchPiece2WindowView::SwitchPiece2WindowView(const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetSmallestDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetPosition({0.0f, 4.3f, UiLayer::tutorialWindow});
    SetSize(menuWindow.GetSize());

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-5.25f, 0.30f, UiLayer::text}, "Try the yellow piece by tapping", textProperties);
    CreateText({-2.1f, -0.775f, UiLayer::text}, "switch again", textProperties);
}

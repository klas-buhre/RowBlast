#include "FillRowsWindowView.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

FillRowsWindowView::FillRowsWindowView(const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetSmallestDarkMenuWindowPotentiallyZoomedScreen()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetPosition({0.0f, 3.0f});
    SetSize(menuWindow.GetSize());

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.15f, 0.8375f, UiLayer::text}, "Blocks are cleared when", textProperties);
    CreateText({-5.8f, -0.2375f, UiLayer::text}, "horizontal rows are filled. Tap the", textProperties);
    CreateText({-5.6f, -1.3125f, UiLayer::text}, "suggested move to fill three rows", textProperties);
}


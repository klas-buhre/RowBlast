#include "TutorialWindowView.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

TutorialWindowView::TutorialWindowView(const CommonResources& commonResources,
                                       const std::vector<std::string>& textLines,
                                       float yPosition) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetSmallestDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));
    
    SetPosition({0.0f, yPosition, UiLayer::tutorialWindow});
    SetSize(menuWindow.GetSize());
    
    auto textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    textProperties.mAlignment = Pht::TextAlignment::CenterX;
    
    switch (textLines.size()) {
        case 1:
            CreateText({0.0f, -0.2375f, UiLayer::text}, textLines[0], textProperties);
            break;
        case 2:
            CreateText({0.0f, 0.30f, UiLayer::text}, textLines[0], textProperties);
            CreateText({0.0f, -0.775f, UiLayer::text}, textLines[1], textProperties);
            break;
        case 3:
            CreateText({0.0f, 0.9075f, UiLayer::text}, textLines[0], textProperties);
            CreateText({0.0f, -0.1675f, UiLayer::text}, textLines[1], textProperties);
            CreateText({0.0f, -1.2425f, UiLayer::text}, textLines[2], textProperties);
            break;
        default:
            assert(false);
            break;
    }
}

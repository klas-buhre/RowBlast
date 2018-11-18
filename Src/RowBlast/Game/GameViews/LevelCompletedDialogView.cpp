#include "LevelCompletedDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

LevelCompletedDialogView::LevelCompletedDialogView(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumDarkMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-3.7f, 4.95f, UiLayer::text},
               "LEVEL CLEARED!",
               guiResources.GetLargeWhiteTextProperties(zoom));

    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.3f,
        GetSize().y / 2.0f - 1.3f,
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

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);

    Pht::Vec2 nextButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style nextButtonStyle;
    nextButtonStyle.mRenderableObject = &guiResources.GetLargerBlueGlossyButton(zoom);
    nextButtonStyle.mSelectedRenderableObject = &guiResources.GetLargerDarkBlueGlossyButton(zoom);
    nextButtonStyle.mPressedScale = 1.05f;

    mNextButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -4.2f, UiLayer::textRectangle},
                                               nextButtonInputSize,
                                               nextButtonStyle);
    mNextButton->CreateText({-1.1f, -0.31f, UiLayer::buttonText},
                            "NEXT",
                            guiResources.GetLargeWhiteButtonTextProperties(zoom));
}

#include "GameOverDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

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
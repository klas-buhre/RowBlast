#include "AboutMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

AboutMenuView::AboutMenuView(Pht::IEngine& engine, const CommonResources& commonResources) {
    auto zoom {PotentiallyZoomedScreen::No};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-1.7f, 4.95f, UiLayer::text},
               "ABOUT",
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
    
    auto& buttonTextProperties {guiResources.GetWhiteButtonTextProperties(zoom)};
    
    MenuButton::Style buttonStyle;
    buttonStyle.mPressedScale = 1.05f;
    buttonStyle.mRenderableObject = &guiResources.GetMediumThinBlueGlossyButton();
    buttonStyle.mSelectedRenderableObject = &guiResources.GetMediumThinDarkBlueGlossyButton();

    Pht::Vec2 buttonInputSize {194.0f, 43.0f};

    mTermsOfServiceButton = std::make_unique<MenuButton>(engine,
                                                         *this,
                                                         Pht::Vec3 {0.0f, 2.7f, UiLayer::textRectangle},
                                                         buttonInputSize,
                                                         buttonStyle);
    mTermsOfServiceButton->CreateText({-2.8f, -0.23f, UiLayer::buttonText},
                                      "Terms Of Service",
                                      buttonTextProperties);

    mPrivacyPolicyButton = std::make_unique<MenuButton>(engine,
                                                        *this,
                                                        Pht::Vec3 {0.0f, 0.5f, UiLayer::textRectangle},
                                                        buttonInputSize,
                                                        buttonStyle);
    mPrivacyPolicyButton->CreateText({-2.4f, -0.23f, UiLayer::buttonText},
                                      "Privacy Policy",
                                      buttonTextProperties);

    mCreditsButton = std::make_unique<MenuButton>(engine,
                                                  *this,
                                                  Pht::Vec3 {0.0f, -1.7f, UiLayer::textRectangle},
                                                  buttonInputSize,
                                                  buttonStyle);
    mCreditsButton->CreateText({-1.15f, -0.23f, UiLayer::buttonText},
                               "Credits",
                               buttonTextProperties);

    mBackButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -5.0f, UiLayer::textRectangle},
                                               buttonInputSize,
                                               buttonStyle);
    mBackButton->CreateIcon("home.png",
                            {-1.17f, 0.06f, UiLayer::buttonText},
                            {0.7f, 0.7f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                            Pht::Vec3 {-0.05f, -0.05f, -0.1f});
    mBackButton->CreateText({-0.46f, -0.23f, UiLayer::buttonText}, "Back", buttonTextProperties);
}

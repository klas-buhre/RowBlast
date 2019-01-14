#include "AcceptTermsDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

AcceptTermsDialogView::AcceptTermsDialogView(Pht::IEngine& engine,
                                             const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::No};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-3.0f, 8.25f, UiLayer::text}, "AGREEMENTS", largeTextProperties);

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.6f, 5.15f, UiLayer::text},  "By clicking Agree you agree", textProperties);
    CreateText({-4.6f, 4.075f, UiLayer::text}, "to our Terms of Service and", textProperties);
    CreateText({-4.6f, 3.0f, UiLayer::text},   "acknowledge that you have", textProperties);
    CreateText({-4.0f, 1.925f, UiLayer::text}, "read our Privacy Policy.", textProperties);

    Pht::Material underlineMaterial {Pht::Color{0.95f, 0.95f, 0.95f}};
    auto& termsOfServiceUnderlineSceneObject {
        CreateSceneObject(Pht::QuadMesh {5.5f, 0.05f}, underlineMaterial, sceneManager)
    };
    termsOfServiceUnderlineSceneObject.GetTransform().SetPosition({0.0f, -0.35f, UiLayer::textRectangle});
    auto& termsOfServiceButtonSceneObject {CreateSceneObject()};
    termsOfServiceButtonSceneObject.AddChild(termsOfServiceUnderlineSceneObject);
    GetRoot().AddChild(termsOfServiceButtonSceneObject);

    auto& buttonTextProperties {guiResources.GetWhiteButtonTextProperties(zoom)};
    Pht::Vec2 termsButtonInputSize  {150.0f, 35.0f};

    MenuButton::Style termsOfServiceButtonStyle;
    termsOfServiceButtonStyle.mSceneObject = &termsOfServiceButtonSceneObject;
    termsOfServiceButtonStyle.mPressedScale = 1.05f;

    mTermsOfServiceButton = std::make_unique<MenuButton>(engine,
                                                         *this,
                                                         Pht::Vec3 {0.0f, -1.0f, UiLayer::textRectangle},
                                                         termsButtonInputSize,
                                                         termsOfServiceButtonStyle);
    mTermsOfServiceButton->CreateText({-2.75f, -0.23f, UiLayer::buttonText},
                                      "Terms of Service",
                                      buttonTextProperties);
    
    auto& privacyPolicyUnderlineSceneObject {
        CreateSceneObject(Pht::QuadMesh {4.75f, 0.05f}, underlineMaterial, sceneManager)
    };
    privacyPolicyUnderlineSceneObject.GetTransform().SetPosition({0.0f, -0.36f, UiLayer::textRectangle});
    auto& privacyPolicyButtonSceneObject {CreateSceneObject()};
    privacyPolicyButtonSceneObject.AddChild(privacyPolicyUnderlineSceneObject);
    GetRoot().AddChild(privacyPolicyButtonSceneObject);

    MenuButton::Style privacyPolicyButtonStyle;
    privacyPolicyButtonStyle.mSceneObject = &privacyPolicyButtonSceneObject;
    privacyPolicyButtonStyle.mPressedScale = 1.05f;

    mPrivacyPolicyButton = std::make_unique<MenuButton>(engine,
                                                        *this,
                                                        Pht::Vec3 {0.0f, -3.0f, UiLayer::textRectangle},
                                                        termsButtonInputSize,
                                                        privacyPolicyButtonStyle);
    mPrivacyPolicyButton->CreateText({-2.35f, -0.23f, UiLayer::buttonText},
                                      "Privacy Policy",
                                      buttonTextProperties);

    Pht::Vec2 agreeButtonInputSize {194.0f, 50.0f};

    MenuButton::Style agreeButtonStyle;
    agreeButtonStyle.mRenderableObject = &guiResources.GetLargeGreenGlossyButton(zoom);
    agreeButtonStyle.mSelectedRenderableObject = &guiResources.GetLargeDarkGreenGlossyButton(zoom);
    agreeButtonStyle.mPressedScale = 1.05f;

    mAgreeButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                Pht::Vec3 {0.0f, -7.9f, UiLayer::textRectangle},
                                                agreeButtonInputSize,
                                                agreeButtonStyle);
    mAgreeButton->CreateText({-1.1f, -0.23f, UiLayer::buttonText}, "AGREE", buttonTextProperties);
}

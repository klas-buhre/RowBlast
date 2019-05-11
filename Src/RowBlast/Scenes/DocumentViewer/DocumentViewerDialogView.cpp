#include "DocumentViewerDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "IRenderer.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "CommonResources.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

DocumentViewerDialogView::DocumentViewerDialogView(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) {
    auto zoom = PotentiallyZoomedScreen::No;
    auto& guiResources = commonResources.GetGuiResources();
    auto& sceneManager = engine.GetSceneManager();
    
    SetSize({engine.GetRenderer().GetHudFrustumSize().x, 26.0f});
    
    Pht::Material backgroundMaterial {"space.jpg"};
    auto& backgroundSceneObject =
        CreateSceneObject(Pht::QuadMesh {53.0f, 53.0f}, backgroundMaterial, sceneManager);
    backgroundSceneObject.GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    GetRoot().AddChild(backgroundSceneObject);

    Pht::Vec3 captionPosition {-7.1f, 11.1f, UiLayer::text};
    auto& largeTextProperties = guiResources.GetLargeWhiteTextProperties(zoom);
    mTermsOfServiceCaption = &CreateText(captionPosition,
                                         "TERMS OF SERVICE",
                                         largeTextProperties).GetSceneObject();
    mPrivacyPolicyCaption = &CreateText(captionPosition,
                                        "PRIVACY POLICY",
                                        largeTextProperties).GetSceneObject();
    mCreditsCaption = &CreateText(captionPosition,
                                  "CREDITS",
                                  largeTextProperties).GetSceneObject();

    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.6f);
    auto& lineSceneObject =
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 0.8f, 0.06f}, lineMaterial, sceneManager);
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);

    Pht::Vec2 backButtonInputSize {194.0f, 43.0f};

    MenuButton::Style backButtonStyle;
    backButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    backButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);
    backButtonStyle.mPressedScale = 1.05f;

    mBackButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f,  -GetSize().y / 2.0f + 1.3f, UiLayer::textRectangle},
                                               backButtonInputSize,
                                               backButtonStyle);
    mBackButton->CreateIcon("back.png",
                            {-1.17f, 0.0f, UiLayer::buttonText},
                            {0.7f, 0.7f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                            Pht::Vec3 {-0.05f, -0.05f, UiLayer::textShadow});
    mBackButton->CreateText({-0.46f, -0.23f, UiLayer::buttonText},
                            "Back",
                            guiResources.GetWhiteButtonTextProperties(zoom));

}

void DocumentViewerDialogView::SetTermsOfServiceCaption() {
    HideAllCaptions();
    mTermsOfServiceCaption->SetIsVisible(true);
}

void DocumentViewerDialogView::SetPrivacyPolicyCaption() {
    HideAllCaptions();
    mPrivacyPolicyCaption->SetIsVisible(true);
}

void DocumentViewerDialogView::SetCreditsCaption() {
    HideAllCaptions();
    mCreditsCaption->SetIsVisible(true);
}

void DocumentViewerDialogView::HideAllCaptions() {
    mTermsOfServiceCaption->SetIsVisible(false);
    mPrivacyPolicyCaption->SetIsVisible(false);
    mCreditsCaption->SetIsVisible(false);
}

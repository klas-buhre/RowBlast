#include "DocumentViewerDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "IRenderer.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

DocumentViewerDialogView::DocumentViewerDialogView(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::No};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& sceneManager {engine.GetSceneManager()};
    
    SetSize({engine.GetRenderer().GetHudFrustumSize().x, 26.0f});
    
    Pht::Material backgroundMaterial {"space.jpg"};
    auto& backgroundSceneObject {
        CreateSceneObject(Pht::QuadMesh {53.0f, 53.0f}, backgroundMaterial, sceneManager)
    };
    backgroundSceneObject.GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    GetRoot().AddChild(backgroundSceneObject);

    Pht::Vec3 captionPosition {-7.1f, 11.1f, UiLayer::text};
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    mTermsOfServiceCaption = &CreateText(captionPosition,
                                         "TERMS OF SERVICE",
                                         largeTextProperties).GetSceneObject();
    mPrivacyPolicyCaption = &CreateText(captionPosition,
                                        "PRIVACY POLICY",
                                        largeTextProperties).GetSceneObject();
    mCreditsCaption = &CreateText(captionPosition,
                                  "CREDITS",
                                  largeTextProperties).GetSceneObject();

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
    lineMaterial.SetOpacity(0.6f);
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 0.8f, 0.06f}, lineMaterial, sceneManager)
    };
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
    mBackButton->CreateText({-0.85f, -0.23f, UiLayer::buttonText},
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

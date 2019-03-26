#include "DocumentViewerController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "InputEvent.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "TextDocumentLoader.hpp"
#include "UiLayer.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto lineSpacing = 0.65f;
    constexpr auto maxLineWidth = 49;
    const Pht::Vec3 upperLeft {-7.1f, 9.0f, UiLayer::text};
    const Pht::Vec2 scrollPanelSize {15.0f, 22.0};
    constexpr auto panelCutoffVelocity = 0.1f;
    constexpr auto dampingCoefficient = 2.2f;
    
    std::string ToFilename(DocumentId documentId) {
        switch (documentId) {
            case DocumentId::TermsOfService:
                return "terms_of_service_english.txt";
            case DocumentId::PrivacyPolicy:
                return "privacy_policy_english.txt";
            case DocumentId::Credits:
                return "credits_english.txt";
        }
    }
}

DocumentViewerController::DocumentViewerController(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) :
    mEngine {engine},
    mCommonResources {commonResources},
    mScene {engine},
    mDialogView {engine, commonResources} {}

void DocumentViewerController::Init(DocumentId documentId) {
    mScene.Init();
    mScene.GetUiViewsContainer().AddChild(mDialogView.GetRoot());
    
    Pht::TextProperties textProperties {
        mCommonResources.GetHussarFontSize20(PotentiallyZoomedScreen::No),
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };
    
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;

    mScrollPanel = std::make_unique<Pht::ScrollPanel>(mEngine,
                                                      scrollPanelSize,
                                                      dampingCoefficient,
                                                      panelCutoffVelocity);
    TextDocumentLoader::Load(mScene.GetScene(),
                             *mScrollPanel,
                             ToFilename(documentId),
                             textProperties,
                             upperLeft,
                             lineSpacing,
                             maxLineWidth);
    mScrollPanel->Init();
    mScene.GetScrollPanelContainer().AddChild(mScrollPanel->GetRoot());
    SetDialogCaption(documentId);
    
    mEngine.GetSceneManager().InitRenderer();
}

void DocumentViewerController::SetDialogCaption(DocumentId documentId) {
    switch (documentId) {
        case DocumentId::TermsOfService:
            mDialogView.SetTermsOfServiceCaption();
            break;
        case DocumentId::PrivacyPolicy:
            mDialogView.SetPrivacyPolicyCaption();
            break;
        case DocumentId::Credits:
            mDialogView.SetCreditsCaption();
            break;
    }
}

DocumentViewerController::Command DocumentViewerController::Update() {
    auto command = Command::None;
    auto& input = mEngine.GetInput();
    
    while (input.HasEvents()) {
        auto& event = input.GetNextEvent();
        switch (event.GetKind()) {
            case Pht::InputKind::Touch: {
                auto& touchEvent = event.GetTouchEvent();
                switch (OnTouch(touchEvent)) {
                    case Result::None:
                        mScrollPanel->OnTouch(touchEvent);
                        break;
                    case Result::Close:
                        command = Command::Close;
                        break;
                    case Result::TouchStartedOverButton:
                        break;
                }
                break;
            }
            default:
                assert(false);
                break;
        }
        
        input.PopNextEvent();
    }
    
    mScrollPanel->Update();
    
    return command;
}

DocumentViewerController::Result
DocumentViewerController::OnTouch(const Pht::TouchEvent& touchEvent) {
    auto& closeButton = mDialogView.GetCloseButton();
    auto& backButton = mDialogView.GetBackButton();
    
    if (closeButton.IsClicked(touchEvent) || backButton.IsClicked(touchEvent)) {
        return Result::Close;
    }
    
    if (closeButton.GetButton().StateIsDownOrMovedOutside() ||
        backButton.GetButton().StateIsDownOrMovedOutside()) {

        return Result::TouchStartedOverButton;
    }

    return Result::None;
}

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
    constexpr auto lineSpacing {0.65f};
    constexpr auto maxLineWidth {49};
    const Pht::Vec3 upperLeft {-7.1f, 9.0f, UiLayer::text};
    constexpr auto panelCutoffVelocity {0.1f};
    constexpr auto dampingCoefficient {5.0f};
    
    std::string ToFilename(DocumentId document) {
        switch (document) {
            case DocumentId::TermsOfService:
                return "terms_of_service_english.txt";
            case DocumentId::PrivacyPolicy:
                return "privacy_policy_english.txt";
        }
    }
}

DocumentViewerController::DocumentViewerController(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) :
    mEngine {engine},
    mCommonResources {commonResources},
    mScene {engine},
    mDialogView {engine, commonResources} {}

void DocumentViewerController::Init(DocumentId document) {
    mScene.Init();
    mScene.GetUiViewsContainer().AddChild(mDialogView.GetRoot());

    mScrollPanel = std::make_unique<Pht::ScrollPanel>(mEngine,
                                                      dampingCoefficient,
                                                      panelCutoffVelocity);
    
    Pht::TextProperties textProperties {
        mCommonResources.GetHussarFontSize20(PotentiallyZoomedScreen::No),
        1.0f,
        Pht::Vec4{0.95f, 0.95f, 0.95f, 1.0f}
    };
    
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;

    TextDocumentLoader::Load(mScene.GetScene(),
                             *mScrollPanel,
                             ToFilename(document),
                             textProperties,
                             upperLeft,
                             lineSpacing,
                             maxLineWidth);
    
    mScene.GetScrollPanelContainer().AddChild(mScrollPanel->GetRoot());
    
    mEngine.GetSceneManager().InitRenderer();
}

DocumentViewerController::Command DocumentViewerController::Update() {
    auto command {Command::None};
    auto& input {mEngine.GetInput()};
    
    while (input.HasEvents()) {
        auto& event {input.GetNextEvent()};
        switch (event.GetKind()) {
            case Pht::InputKind::Touch: {
                auto& touchEvent {event.GetTouchEvent()};
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
    auto& closeButton {mDialogView.GetCloseButton()};
    auto& backButton {mDialogView.GetBackButton()};
    
    if (closeButton.IsClicked(touchEvent) || backButton.IsClicked(touchEvent)) {
        return Result::Close;
    }
    
    if (closeButton.GetButton().StateIsDownOrMovedOutside() ||
        backButton.GetButton().StateIsDownOrMovedOutside()) {

        return Result::TouchStartedOverButton;
    }

    return Result::None;
}

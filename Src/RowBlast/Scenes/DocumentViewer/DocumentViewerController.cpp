#include "DocumentViewerController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "ISceneManager.hpp"
#include "InputUtil.hpp"

// Game includes.
#include "TextDocumentLoader.hpp"
#include "UiLayer.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto lineSpacing {0.65f};
    constexpr auto maxLineWidth {46};
    const Pht::Vec3 upperLeft {-6.7f, 9.0f, UiLayer::text};
}

DocumentViewerController::DocumentViewerController(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) :
    mEngine {engine},
    mCommonResources {commonResources},
    mScene {engine},
    mDialogView {engine, commonResources} {}

void DocumentViewerController::Init() {
    mScene.Init();
    mScene.GetUiViewsContainer().AddChild(mDialogView.GetRoot());

    mScrollPanel = std::make_unique<Pht::ScrollPanel>();
    
    Pht::TextProperties textProperties {
        mCommonResources.GetHussarFontSize20(PotentiallyZoomedScreen::No),
        1.0f,
        Pht::Vec4{0.95f, 0.95f, 0.95f, 1.0f}
    };

    TextDocumentLoader::Load(mScene.GetScene(),
                             mScrollPanel->GetPanel(),
                             // "privacy_policy_english.txt",
                             "terms_of_service_english.txt",
                             textProperties,
                             upperLeft,
                             lineSpacing,
                             maxLineWidth);
    
    mScene.GetScrollPanelContainer().AddChild(mScrollPanel->GetRoot());
    
    mEngine.GetSceneManager().InitRenderer();
}

DocumentViewerController::Command DocumentViewerController::Update() {
    return InputUtil::HandleInput<Command>(mEngine.GetInput(),
                                           Command::None,
                                           [this] (const Pht::TouchEvent& touch) {
                                               return OnTouch(touch);
                                           });
}

DocumentViewerController::Command
DocumentViewerController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mDialogView.GetCloseButton().IsClicked(touchEvent) ||
        mDialogView.GetBackButton().IsClicked(touchEvent)) {

        return Command::Close;
    }

    return Command::None;
}

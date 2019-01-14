#include "DocumentViewerController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "ISceneManager.hpp"
#include "InputUtil.hpp"

using namespace RowBlast;

DocumentViewerController::DocumentViewerController(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) :
    mEngine {engine},
    mScene {engine},
    mDialogView {engine, commonResources} {}

void DocumentViewerController::Init() {
    mScene.Init();
    mScene.GetUiViewsContainer().AddChild(mDialogView.GetRoot());
    
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

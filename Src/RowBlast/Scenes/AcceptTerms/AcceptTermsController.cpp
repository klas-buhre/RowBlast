#include "AcceptTermsController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "ISceneManager.hpp"
#include "InputUtil.hpp"
#include "FileStorage.hpp"
#include "IAnalytics.hpp"

using namespace RowBlast;

namespace {
    const std::string termsAcceptedFilename {"terms_accepted.dat"};
}

AcceptTermsController::AcceptTermsController(Pht::IEngine& engine,
                                             const CommonResources& commonResources) :
    mEngine {engine},
    mScene {engine},
    mDialogView {engine, commonResources} {}

bool AcceptTermsController::IsTermsAccepted() const {
#ifdef ANALYTICS_ENABLED
    std::string data;
    return Pht::FileStorage::Load(termsAcceptedFilename, data);
#else
    return false;
#endif
}

void AcceptTermsController::Init() {
    mScene.Init();
    mScene.GetUiViewsContainer().AddChild(mDialogView.GetRoot());
    
    mEngine.GetSceneManager().InitRenderer();
}

AcceptTermsController::Command AcceptTermsController::Update() {
    return InputUtil::HandleInput<Command>(mEngine.GetInput(),
                                           Command::None,
                                           [this] (const Pht::TouchEvent& touch) {
                                               return OnTouch(touch);
                                           });
}

AcceptTermsController::Command
AcceptTermsController::OnTouch(const Pht::TouchEvent& touchEvent) {
    if (mDialogView.GetTermsOfServiceButton().IsClicked(touchEvent)) {
        return Command::ViewTermsOfService;
    }

    if (mDialogView.GetPrivacyPolicyButton().IsClicked(touchEvent)) {
        return Command::ViewPrivacyPolicy;
    }
    
    if (mDialogView.GetAgreeButton().IsClicked(touchEvent)) {
        Pht::FileStorage::Save(termsAcceptedFilename, "termsAccepted");
        return Command::Accept;
    }

    return Command::None;
}

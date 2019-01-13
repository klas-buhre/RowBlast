#include "AcceptTermsController.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"

using namespace RowBlast;

AcceptTermsController::AcceptTermsController(Pht::IEngine& engine) :
    mEngine {engine},
    mScene {engine} {}

bool AcceptTermsController::IsTermsAccepted() const {
    return false;
}

void AcceptTermsController::Init() {
    mScene.Init();
}

AcceptTermsController::Command AcceptTermsController::Update() {
    auto command {Command::None};
    
    if (mEngine.GetInput().ConsumeWholeTouch()) {
        command = Command::GoToTitle;
    }
    
    return command;
}

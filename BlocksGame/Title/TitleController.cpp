#include "TitleController.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "InputEvent.hpp"

using namespace BlocksGame;

TitleController::TitleController(Pht::IEngine& engine, const CommonResources& commonResources) :
    mEngine {engine},
    mScene {engine, commonResources},
    mRenderer {engine.GetRenderer(), mScene} {}

void TitleController::Reset() {
    mScene.Reset();
}

TitleController::Command TitleController::Update() {
    auto command {Command::None};
    
    if (mEngine.GetInput().ConsumeWholeTouch()) {
        command = Command::GoToMap;
    }
    
    mScene.Update();
    mRenderer.RenderFrame();
    
    return command;
}

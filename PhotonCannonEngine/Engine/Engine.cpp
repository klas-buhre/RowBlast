#include "Engine.hpp"

#include <cstdlib>

#include "IApplication.hpp"
#include "RenderableObject.hpp"

using namespace Pht;

namespace {
    const auto maxFrameTimeSeconds {0.4f};
}

Engine::Engine(bool createRenderBuffers, const Vec2& screenInputSize) :
    mRenderer {createRenderBuffers},
    mInputHandler {screenInputSize} {
    
    std::srand(static_cast<int>(std::time(0)));
}

void Engine::Initialize(bool createRenderBuffers) {
    mRenderer.Initialize(createRenderBuffers);

    // Create and init the application.
    mApplication = CreateApplication(*this);
    mApplication->Initialize();
}

void Engine::Update(float frameSeconds) {
    if (frameSeconds < maxFrameTimeSeconds) {
        mLastFrameSeconds = frameSeconds;
    } else {
        mLastFrameSeconds = maxFrameTimeSeconds;
    }

    mRenderer.ClearBuffers();
    mApplication->Update();
    mParticleSystem.Update(mLastFrameSeconds);
}

IRenderer& Engine::GetRenderer() {
    return mRenderer;
}

IInput& Engine::GetInput() {
    return mInputHandler;
}

IAudio& Engine::GetAudio() {
    return mAudio;
}

IParticleSystem& Engine::GetParticleSystem() {
    return mParticleSystem;
}

float Engine::GetLastFrameSeconds() const {
    return mLastFrameSeconds;
}

std::unique_ptr<RenderableObject> Engine::CreateRenderableObject(const IMesh& mesh,
                                                                 const Material& material) {
    return mRenderer.CreateRenderableObject(mesh, material);
}

#include "Engine.hpp"

#include <cstdlib>

#include "IApplication.hpp"
#include "RenderableObject.hpp"
#include "SceneObject.hpp"
#include "Analytics.hpp"

using namespace Pht;

namespace {
    const auto maxFrameTimeSeconds = 0.4f;
}

Engine::Engine(bool createRenderBuffers, const Vec2& screenInputSize) :
    mRenderer {createRenderBuffers},
    mInputHandler {screenInputSize},
    mSceneManager {mRenderer, mInputHandler},
    mAnalytics {CreateAnalyticsApi()} {
    
    std::srand(static_cast<int>(std::time(0)));
}

void Engine::Init(bool createRenderBuffers) {
    mRenderer.Init(createRenderBuffers);
    mInputHandler.Init(mRenderer);

    // Create and init the application.
    mApplication = CreateApplication(*this);
    mApplication->OnInitialize();
}

void Engine::Update(float frameSeconds) {
    if (frameSeconds < maxFrameTimeSeconds) {
        mLastFrameSeconds = frameSeconds;
    } else {
        mLastFrameSeconds = maxFrameTimeSeconds;
    }

    mApplication->OnUpdate();
    
    mAnimationSystem.Update(mLastFrameSeconds);
    auto* scene = mSceneManager.GetActiveScene();
    if (scene) {
        scene->GetRoot().Update(false);
    }
    
    mParticleSystem.Update(mLastFrameSeconds);
    mAudio.Update(mLastFrameSeconds);
    
    if (scene) {
        mRenderer.ClearBuffers();
        mRenderer.RenderScene(*scene);
    }
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

ISceneManager& Engine::GetSceneManager() {
    return mSceneManager;
}

IAnimationSystem& Engine::GetAnimationSystem() {
    return mAnimationSystem;
}

IParticleSystem& Engine::GetParticleSystem() {
    return mParticleSystem;
}

IAnalytics& Engine::GetAnalytics() {
    return *mAnalytics;
}

float Engine::GetLastFrameSeconds() const {
    return mLastFrameSeconds;
}

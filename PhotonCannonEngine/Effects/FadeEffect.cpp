#include "FadeEffect.hpp"

#include "IEngine.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"
#include "IRenderer.hpp"

using namespace Pht;

namespace {
    const Mat4 quadMatrix;
}

FadeEffect::FadeEffect(IEngine& engine, float duration, float midFade) :
    mEngine {engine},
    mMidFade {midFade},
    mFadeSpeed {midFade / duration} {
    
    Pht::Material quadMaterial;
    
    auto& hudFrustumSize {engine.GetRenderer().GetHudFrustumSize()};
    auto width {hudFrustumSize.x + 0.1f};
    auto height {hudFrustumSize.y + 0.1f};
    
    Vec4 color {0.0f, 0.0f, 0.0f, 1.0f};

    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, color},
        {{width / 2.0f, -height / 2.0f, 0.0f}, color},
        {{width / 2.0f, height / 2.0f, 0.0f}, color},
        {{-width / 2.0f, height / 2.0f, 0.0f}, color},
    };
    
    mQuad = engine.CreateRenderableObject(Pht::QuadMesh {vertices}, quadMaterial);
}

void FadeEffect::Reset() {
    mFade = 0.0f;
    mState = State::Idle;
}

void FadeEffect::Start() {
    mFade = 0.0f;
    mState = State::FadingOut;
}

FadeEffect::State FadeEffect::Update() {
    switch (mState) {
        case State::FadingOut:
            mFade += mFadeSpeed * mEngine.GetLastFrameSeconds();
            if (mFade >= mMidFade) {
                mFade = mMidFade;
                mState = State::Transition;
            }
            break;
        case State::Transition:
            mState = State::FadingIn;
            break;
        case State::FadingIn:
            mFade -= mFadeSpeed * mEngine.GetLastFrameSeconds();
            if (mFade <= 0.0f) {
                mFade = 0.0f;
                mState = State::Idle;
            }
            break;
        case State::Idle:
            break;
    }
    
    return mState;
}

FadeEffect::State FadeEffect::UpdateAndRender() {
    if (Update() == State::Idle) {
        return State::Idle;
    }
    
    Render();
    
    return mState;
}

void FadeEffect::Render() const {
    auto& renderer {mEngine.GetRenderer()};
    renderer.SetHudMode(true);
    renderer.SetDepthTest(false);
    mQuad->GetMaterial().SetOpacity(mFade);
    renderer.Render(*mQuad, quadMatrix);
    renderer.SetDepthTest(true);
    renderer.SetHudMode(false);
}

bool FadeEffect::IsFadingOut() const {
    return mState == State::FadingOut;
}

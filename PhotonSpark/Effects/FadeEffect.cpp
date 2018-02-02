#include "FadeEffect.hpp"

#include "ISceneManager.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"
#include "IRenderer.hpp"

using namespace Pht;

FadeEffect::FadeEffect(ISceneManager& sceneManager,
                       IRenderer& renderer,
                       float duration,
                       float midFade) :
    mRenderer {renderer},
    mMidFade {midFade},
    mFadeSpeed {midFade / duration} {
    
    Pht::Material quadMaterial;
    quadMaterial.GetDepthState().mDepthTest = false;
    
    auto& hudFrustumSize {mRenderer.GetHudFrustumSize()};
    auto width {hudFrustumSize.x + 0.1f};
    auto height {hudFrustumSize.y + 0.1f};
    
    Vec4 color {0.0f, 0.0f, 0.0f, 1.0f};

    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, color},
        {{width / 2.0f, -height / 2.0f, 0.0f}, color},
        {{width / 2.0f, height / 2.0f, 0.0f}, color},
        {{-width / 2.0f, height / 2.0f, 0.0f}, color},
    };
    
    mQuad = sceneManager.CreateRenderableObject(Pht::QuadMesh {vertices}, quadMaterial);
    mSceneObject = std::make_unique<SceneObject>(mQuad.get());
    mQuad->GetMaterial().SetOpacity(mFade);
}

void FadeEffect::Reset() {
    mFade = 0.0f;
    mState = State::Idle;
}

void FadeEffect::Start() {
    mFade = 0.0f;
    mState = State::FadingOut;
}

FadeEffect::State FadeEffect::Update(float dt) {
    switch (mState) {
        case State::FadingOut:
            mFade += mFadeSpeed * dt;
            if (mFade >= mMidFade) {
                mFade = mMidFade;
                mState = State::Transition;
            }
            mQuad->GetMaterial().SetOpacity(mFade);
            break;
        case State::Transition:
            mState = State::FadingIn;
            break;
        case State::FadingIn:
            mFade -= mFadeSpeed * dt;
            if (mFade <= 0.0f) {
                mFade = 0.0f;
                mState = State::Idle;
            }
            mQuad->GetMaterial().SetOpacity(mFade);
            break;
        case State::Idle:
            break;
    }
    
    return mState;
}

void FadeEffect::Render() const {
    mRenderer.SetHudMode(true);
    mRenderer.RenderObject(*mQuad, mSceneObject->GetMatrix());
    mRenderer.SetHudMode(false);
}

bool FadeEffect::IsFadingOut() const {
    return mState == State::FadingOut;
}

#include "StarsAnimation.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "ObjMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto scaleTime {0.4f};
    constexpr auto zRotationSpeed {12.0f};
    const Pht::Vec3 leftMostStarPosition {-3.8f, 1.3f, UiLayer::block};
    const Pht::Vec3 leftStarPosition {-2.0f, 1.3f, UiLayer::block};
    const Pht::Vec3 middleStarPosition {0.0f, 1.8f, UiLayer::block};
    const Pht::Vec3 rightStarPosition {2.0f, 1.3f, UiLayer::block};
    const Pht::Vec3 rightMostStarPosition {3.8f, 1.3f, UiLayer::block};
    const Pht::Vec3 shadowOffset {-0.3f, -0.3f, -0.3f};
}

StarsAnimation::StarsAnimation(Pht::IEngine& engine,
                               GameScene& scene,
                               const CommonResources& commonResources) :
    mEngine {engine},
    mScene {scene} {
    
    mContainer = std::make_unique<Pht::SceneObject>();
    
    auto& sceneManager {mEngine.GetSceneManager()};
    Pht::Material shaddowMaterial {Pht::Color{0.4f, 0.4f, 0.4f}};
    shaddowMaterial.SetOpacity(0.075f);
    mShadowRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"star_1428.obj", 0.26f},
                                                            shaddowMaterial);

    for (auto& star: mStarAnimations) {
        star = std::make_unique<StarAnimation>(engine,
                                               *mContainer,
                                               *mShadowRenderable,
                                               commonResources);
    }
}

void StarsAnimation::Init() {
    mScene.GetUiViewsContainer().AddChild(*mContainer);
    
    mContainer->SetIsStatic(true);
    mContainer->SetIsVisible(false);
}

void StarsAnimation::Start(int numStars) {
    mNumStars = numStars;

    for (auto& star: mStarAnimations) {
        star->Reset();
    }

    mContainer->SetIsStatic(false);
    mContainer->SetIsVisible(true);

    switch (numStars) {
        case 1:
            mStarAnimations[0]->Start(middleStarPosition, 0.0f);
            break;
        case 2:
            mStarAnimations[0]->Start(leftStarPosition, 0.0f);
            mStarAnimations[1]->Start(rightStarPosition, scaleTime);
            break;
        case 3:
            mStarAnimations[0]->Start(leftMostStarPosition, 0.0f);
            mStarAnimations[1]->Start(middleStarPosition, scaleTime);
            mStarAnimations[2]->Start(rightMostStarPosition, scaleTime * 2.0f);
            break;
        default:
            assert(!"Unsupported number of stars");
            break;
    }
}

StarsAnimation::State StarsAnimation::Update() {
    State state {State::Rotating};
    auto dt {mEngine.GetLastFrameSeconds()};
    
    for (auto i {0}; i < mNumStars; ++i) {
        if (mStarAnimations[i]->Update(dt) != StarAnimation::State::Rotating) {
            state = State::Ongoing;
        }
    }
    
    return state;
}

StarsAnimation::StarAnimation::StarAnimation(Pht::IEngine& engine,
                                             Pht::SceneObject& parentObject,
                                             Pht::RenderableObject& shadowRenderable,
                                             const CommonResources& commonResources) {
    auto& sceneManager {engine.GetSceneManager()};
    mStarRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"star_1428.obj", 0.26f},
                                                          commonResources.GetMaterials().GetGoldMaterial());
    mStar = std::make_unique<Pht::SceneObject>(mStarRenderable.get());
    parentObject.AddChild(*mStar);
    
    mShadow = std::make_unique<Pht::SceneObject>(&shadowRenderable);
    mShadow->GetTransform().SetPosition(shadowOffset);
    mStar->AddChild(*mShadow);
}

void StarsAnimation::StarAnimation::Reset() {
     mStar->SetIsVisible(false);
     mShadow->SetIsVisible(false);
}

void StarsAnimation::StarAnimation::Start(const Pht::Vec3& position, float waitTime) {
    mStar->GetTransform().SetPosition(position);
    mStar->SetIsVisible(true);

    mState = State::Waiting;
    mElapsedTime = 0.0f;
    mWaitTime = waitTime;
    mStarZAngle = 0.0f;
}

StarsAnimation::StarAnimation::State StarsAnimation::StarAnimation::Update(float dt) {
    switch (mState) {
        case State::Waiting:
            UpdateInWaitingState(dt);
            break;
        case State::ScalingIn:
            mState = State::Flashing;
            break;
        case State::Flashing:
            mState = State::Rotating;
            mShadow->SetIsVisible(true);
            break;
        case State::Rotating:
            UpdateInRotatingState(dt);
            break;
    }
    
    return mState;
}

void StarsAnimation::StarAnimation::UpdateInWaitingState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > mWaitTime) {
        mState = State::ScalingIn;
        mElapsedTime = 0.0f;
    }
}

void StarsAnimation::StarAnimation::UpdateInRotatingState(float dt) {
    auto& transform {mStar->GetTransform()};
    transform.SetRotation({0.0f, 0.0f, mStarZAngle});

    mStarZAngle -= zRotationSpeed * dt;
}

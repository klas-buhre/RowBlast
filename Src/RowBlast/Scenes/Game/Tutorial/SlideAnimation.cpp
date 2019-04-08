#include "SlideAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "GuiView.hpp"
#include "ISceneManager.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto borderWidth = 0.1f;
}

SlideAnimation::SlideAnimation(Pht::IEngine& engine,
                               Pht::GuiView& view,
                               const Pht::Vec3& position,
                               float size,
                               float frameTime) :
    mEngine {engine},
    mSlideSceneObject {view.CreateSceneObject()},
    mSize {size},
    mFrameTime {frameTime} {
    
    auto& container = view.CreateSceneObject();
    container.GetTransform().SetPosition(position);
    view.GetRoot().AddChild(container);
    
    container.AddChild(mSlideSceneObject);

    Pht::Material lineMaterial {Pht::Color{0.0f, 0.0f, 0.0f}};
    lineMaterial.SetOpacity(0.5f);
    auto& sceneManager = engine.GetSceneManager();
    
    auto& lineSceneObject =
        view.CreateSceneObject(Pht::QuadMesh {size + borderWidth, borderWidth},
                               lineMaterial,
                               sceneManager);
    lineSceneObject.GetTransform().SetPosition({0.0f, size / 2.0f, UiLayer::buttonText});
    container.AddChild(lineSceneObject);
    
    auto& lineSceneObject2 =
        view.CreateSceneObject(Pht::QuadMesh {size + borderWidth, borderWidth},
                               lineMaterial,
                               sceneManager);
    lineSceneObject2.GetTransform().SetPosition({0.0f, -size / 2.0f, UiLayer::buttonText});
    container.AddChild(lineSceneObject2);

    auto& lineSceneObject3 =
        view.CreateSceneObject(Pht::QuadMesh {borderWidth, size - borderWidth},
                               lineMaterial,
                               sceneManager);
    lineSceneObject3.GetTransform().SetPosition({-size / 2.0f, 0.0f, UiLayer::buttonText});
    container.AddChild(lineSceneObject3);

    auto& lineSceneObject4 =
        view.CreateSceneObject(Pht::QuadMesh {borderWidth, size - borderWidth},
                               lineMaterial,
                               sceneManager);
    lineSceneObject4.GetTransform().SetPosition({size / 2.0f, 0.0f, UiLayer::buttonText});
    container.AddChild(lineSceneObject4);
}

void SlideAnimation::Init(const std::vector<std::string>& frameFilenames, Pht::Scene& scene) {
    mFrameRenderables.clear();
    auto& sceneManager = mEngine.GetSceneManager();
    
    for (auto& frameFilename: frameFilenames) {
        Pht::Material frameMaterial {frameFilename};
        
        auto frameRenderable =
            sceneManager.CreateRenderableObject(Pht::QuadMesh {mSize, mSize}, frameMaterial);
        
        mFrameRenderables.push_back(frameRenderable.get());
        scene.AddRenderableObject(std::move(frameRenderable));
    }
    
    SetFrame(0);
}

void SlideAnimation::Update() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    if (mElapsedTime > mFrameTime) {
        ++mFrameIndex;
        if (mFrameIndex >= mFrameRenderables.size()) {
            mFrameIndex = 0;
        }
        
        SetFrame(mFrameIndex);
    }
}

void SlideAnimation::SetFrame(int frameIndex) {
    assert(frameIndex < mFrameRenderables.size());
    mFrameIndex = frameIndex;
    mSlideSceneObject.SetRenderable(mFrameRenderables[mFrameIndex]);
    mElapsedTime = 0.0f;
}

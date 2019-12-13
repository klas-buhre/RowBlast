#include "SlotsFilledAnimationSystem.hpp"

// Engine includes.
#include "SceneObject.hpp"

// Game includes.
#include "Field.hpp"
#include "LevelResources.hpp"
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto duration = 0.35f;
    constexpr auto scaleUpSpeed = 3.2f;
    const auto fadeSpeed = SlotFillAnimationComponent::mInitialOpacity / duration;
}

SlotsFilledAnimationSystem::SlotsFilledAnimationSystem(Field& field,
                                                       GameScene& scene,
                                                       const LevelResources& levelResources) :
    mField {field},
    mScene {scene} {
    
    auto containerSceneObject = std::make_unique<Pht::SceneObject>();
    mContainerSceneObject = containerSceneObject.get();
    mSceneResources.AddSceneObject(std::move(containerSceneObject));
    
    auto& animationRenderable = levelResources.GetBlueprintSlotAnimationRenderable();
    
    for (auto i = 0; i < numSceneObjects; ++i) {
        auto sceneObject = std::make_unique<Pht::SceneObject>();
        auto renderable = std::make_unique<Pht::RenderableObject>(animationRenderable);
        
        sceneObject->SetRenderable(renderable.get());
        mContainerSceneObject->AddChild(*sceneObject);
        mSceneObjects[i] = sceneObject.get();
        
        mSceneResources.AddSceneObject(std::move(sceneObject));
        mSceneResources.AddRenderableObject(std::move(renderable));
    }
}

void SlotsFilledAnimationSystem::Init() {
    mNextAvailableSceneObject = 0;
    
    mScene.GetPieceDropEffectsContainer().AddChild(*mContainerSceneObject);
    
    for (auto* sceneObject: mSceneObjects) {
        sceneObject->SetIsVisible(false);
    }
}

void SlotsFilledAnimationSystem::Update(float dt) {
    auto* blueprintGrid = mField.GetBlueprintGrid();
    if (blueprintGrid == nullptr) {
        return;
    }
    
    for (auto row = 0; row < mField.GetNumRows(); ++row) {
        for (auto column = 0; column < mField.GetNumColumns(); ++column) {
            auto& slotFilledAnimation = (*blueprintGrid)[row][column].mAnimation;
            
            if (slotFilledAnimation.mIsActive) {
                AnimateSlot(row, column, slotFilledAnimation, dt);
            }
        }
    }
}

void SlotsFilledAnimationSystem::AnimateSlot(int row,
                                             int column,
                                             SlotFillAnimationComponent& animation,
                                             float dt) {
    if (animation.mElapsedTime == 0.0f) {
        animation.mSceneObject = mSceneObjects[mNextAvailableSceneObject % numSceneObjects];
        ++mNextAvailableSceneObject;
        
        auto cellSize = mScene.GetCellSize();
        
        Pht::Vec3 slotPosition {
            column * cellSize + cellSize / 2.0f,
            row * cellSize + cellSize / 2.0f,
            mScene.GetBlueprintAnimationZ()
        };
        
        animation.mSceneObject->GetTransform().SetPosition(slotPosition);
        animation.mSceneObject->SetIsVisible(true);
    }
    
    animation.mElapsedTime += dt;
    
    animation.mScale += scaleUpSpeed * dt;
    Pht::Vec3 scale {animation.mScale, animation.mScale, animation.mScale};
    animation.mSceneObject->GetTransform().SetScale(scale);
    
    animation.mOpacity -= fadeSpeed * dt;
    auto& material = animation.mSceneObject->GetRenderable()->GetMaterial();
    material.SetOpacity(animation.mOpacity);
    material.SetDepthWrite(true);
    
    if (animation.mOpacity < 0.0f || animation.mElapsedTime > duration) {
        animation.mSceneObject->SetIsVisible(false);
        animation = SlotFillAnimationComponent {};
    }
}

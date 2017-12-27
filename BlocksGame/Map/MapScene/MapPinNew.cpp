#include "MapPinNew.hpp"

#include <array>
#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "RenderableObject.hpp"
#include "SphereMesh.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace BlocksGame;

namespace {
    const Pht::Vec2 buttonSize {40.0f, 40.0f};
    const Pht::Color selectedColorAdd {0.3f, 0.3f, 0.3f};
    
    const std::array<Pht::Vec3, 3> starOffsets {
        Pht::Vec3{-0.75f, 1.0f, 1.0f},
        Pht::Vec3{0.0f, 1.2f, 1.0f},
        Pht::Vec3{0.75f, 1.0f, 1.0f}
    };
}

MapPinNew::MapPinNew(Pht::IEngine& engine,
                     const CommonResources& commonResources,
                     Pht::Scene& scene,
                     Pht::SceneObject& containerObject,
                     std::shared_ptr<Pht::RenderableObject> starRenderable,
                     const Pht::Vec3& position,
                     int level,
                     int numStars,
                     bool isClickable) :
    mBlueMaterial {commonResources.GetMaterials().GetBlueMaterial()},
    mLevel {level},
    mIsClickable {isClickable} {
    
    const auto& material {
        isClickable ? mBlueMaterial :
        commonResources.GetMaterials().GetLightGrayMaterial()
    };
    
    Pht::SphereMesh pinMesh {0.85f, std::string{"mapPin"}};
    
    mSceneObject = &scene.CreateSceneObject(pinMesh, material);
    mSceneObject->GetTransform().SetPosition(position);
    containerObject.AddChild(*mSceneObject);
    
    assert(numStars <= 3);
    
    for (auto i {0}; i < numStars; ++i) {
        auto& star {scene.CreateSceneObject()};
        star.SetRenderable(starRenderable);
        
        auto& transform {star.GetTransform()};
        transform.SetRotation({-90.0f, 0.0f, 0.0f});
        transform.SetPosition(starOffsets[i]);
        
        mSceneObject->AddChild(star);
    }
    
    mButton = std::make_unique<Pht::Button>(*mSceneObject, buttonSize, engine);
}

void MapPinNew::SetIsSelected(bool isSelected) {
    auto& material {mSceneObject->GetRenderable()->GetMaterial()};
    const auto& ambient {mBlueMaterial.GetAmbient()};
    const auto& diffuse {mBlueMaterial.GetDiffuse()};
    const auto& specular {mBlueMaterial.GetSpecular()};
    
    if (isSelected) {
        material.SetAmbient(ambient + selectedColorAdd);
        material.SetDiffuse(diffuse + selectedColorAdd);
        material.SetSpecular(specular + selectedColorAdd);
    } else {
        material.SetAmbient(ambient);
        material.SetDiffuse(diffuse);
        material.SetSpecular(specular);
    }
}

const Pht::Vec3& MapPinNew::GetPosition() const {
    return mSceneObject->GetTransform().GetPosition();
}

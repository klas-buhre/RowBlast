#include "MapPin.hpp"

#include <array>
#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "RenderableObject.hpp"
#include "SphereMesh.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec2 buttonSize {62.0f, 62.0f};
    const Pht::Color selectedColorAdd {0.3f, 0.3f, 0.3f};
    const Pht::Vec3 textOffset {-0.025f, -0.2f, 0.4f};
    
    const std::array<Pht::Vec3, 3> starOffsets {
        Pht::Vec3{-0.75f, 0.9f, 0.5f},
        Pht::Vec3{0.0f, 1.1f, 0.5f},
        Pht::Vec3{0.75f, 0.9f, 0.5f}
    };
}

MapPin::MapPin(Pht::IEngine& engine,
               const CommonResources& commonResources,
               const Pht::Font& font,
               Pht::Scene& scene,
               Pht::SceneObject& containerObject,
               Pht::RenderableObject& starRenderable,
               const Pht::Vec3& position,
               int level,
               int numStars,
               bool isClickable,
               const MapPlace& place) :
    mBlueMaterial {commonResources.GetMaterials().GetBlueMaterial()},
    mLevel {level},
    mIsClickable {isClickable},
    mPlace {place} {
    
    const auto& material =
        isClickable ? mBlueMaterial :
        commonResources.GetMaterials().GetLightGrayMaterial();
    
    Pht::SphereMesh pinMesh {0.85f, std::string{"mapPin"}};
    
    mSceneObject = &scene.CreateSceneObject(pinMesh, material);
    mSceneObject->GetTransform().SetPosition(position);
    containerObject.AddChild(*mSceneObject);
    
    mButton = std::make_unique<Pht::Button>(*mSceneObject, buttonSize, engine);
    
    if (place.GetKind() == MapPlace::Kind::MapLevel) {
        CreateStars(numStars, starRenderable, scene);
        CreateText(level, font, scene);
    }
}

void MapPin::CreateStars(int numStars, Pht::RenderableObject& starRenderable, Pht::Scene& scene) {
    assert(numStars <= 3);

    for (auto i = 0; i < numStars; ++i) {
        auto& star = scene.CreateSceneObject();
        star.SetRenderable(&starRenderable);
        
        auto& transform = star.GetTransform();
        transform.SetRotation({90.0f, 0.0f, 0.0f});
        transform.SetPosition(starOffsets[i]);
        
        mSceneObject->AddChild(star);
    }
}

void MapPin::CreateText(int level, const Pht::Font& font, Pht::Scene& scene) {
    Pht::TextProperties textProperties {
        font,
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        Pht::Vec2{0.1f, 0.1f},
        Pht::Vec4{0.32f, 0.32f, 0.32f, 0.5f},
        Pht::SnapToPixel::No
    };
    textProperties.mAlignment = Pht::TextAlignment::CenterX;

    auto& text = scene.CreateText(std::to_string(level), textProperties);
    mTextSceneObject = &text.GetSceneObject();
    mTextSceneObject->GetTransform().SetPosition(textOffset);
    mSceneObject->AddChild(*mTextSceneObject);
}

void MapPin::SetIsSelected(bool isSelected) {
    auto& material = mSceneObject->GetRenderable()->GetMaterial();
    const auto& ambient = mBlueMaterial.GetAmbient();
    const auto& diffuse = mBlueMaterial.GetDiffuse();
    const auto& specular = mBlueMaterial.GetSpecular();
    
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

const Pht::Vec3& MapPin::GetPosition() const {
    return mSceneObject->GetTransform().GetPosition();
}

Pht::Vec3 MapPin::GetUfoPosition() const {
    switch (mPlace.GetKind()) {
        case MapPlace::Kind::MapLevel:
            return mPlace.GetMapLevel().mUfoOffset + GetPosition();
        case MapPlace::Kind::Portal:
            return GetPosition();
    }
}

void MapPin::HideText() {
    if (mTextSceneObject) {
        mTextSceneObject->SetIsVisible(false);
    }
}

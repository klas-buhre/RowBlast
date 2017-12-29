#include "MapPin.hpp"

#include <assert.h>
#include <array>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"

using namespace BlocksGame;

namespace {
    const Pht::Vec2 buttonSize {40.0f, 40.0f};
    const Pht::Vec4 textOffset {-0.33f, -0.2f, 0.0f, 1.0f};
    const std::array<Pht::Vec3, 3> starOffsets {
        Pht::Vec3{-0.75f, 1.0f, 1.0f},
        Pht::Vec3{0.0f, 1.2f, 1.0f},
        Pht::Vec3{0.75f, 1.0f, 1.0f}
    };
}

Pht::Color MapPin::selectedColorAdd {0.3f, 0.3f, 0.3f};

MapPin::MapPin(std::shared_ptr<Pht::RenderableObject> renderable,
               const Pht::Vec3& position,
               int level,
               const Pht::Text& text,
               Pht::IEngine& engine) :
    mRenderer {engine.GetRenderer()},
    mPosition {position},
    mSceneObject {renderable},
    mButton {mSceneObject, buttonSize, engine},
    mLevel {level},
    mText {text} {
    
    mSceneObject.SetPosition(position);
}

void MapPin::Update() {
    mRenderer.SetProjectionMode(Pht::ProjectionMode::Perspective);
    
    auto adjustedTextOffset {textOffset};
    
    if (mLevel > 9) {
        adjustedTextOffset.x -= 0.17f;
    }
    
    auto modelView {mSceneObject.GetMatrix() * mRenderer.GetViewMatrix()};
    auto modelViewProjection {modelView * mRenderer.GetProjectionMatrix()};
    
    // Since the matrix is row-major it has to be transposed in order to multiply with the vector.
    auto clipSpacePos {modelViewProjection.Transposed() * adjustedTextOffset};
    auto normProjPos {clipSpacePos / clipSpacePos.w};
    auto& hudSize {mRenderer.GetHudFrustumSize()};

    Pht::Vec2 textPosition {
        normProjPos.x * hudSize.x / 2.0f,
        normProjPos.y * hudSize.y / 2.0f
    };

    mText.mPosition = textPosition;
}

void MapPin::SetNumStars(int numStars, std::shared_ptr<Pht::RenderableObject> starRenderable) {
    assert(numStars >= 0 && numStars <= 3);
    
    mStars.clear();
    
    for (auto i {0}; i < numStars; ++i) {
        auto star {std::make_unique<Pht::SceneObject>(starRenderable)};
        star->SetRotationX(90.0f);
        star->SetPosition(mPosition + starOffsets[i]);
        mStars.push_back(std::move(star));
    }
}

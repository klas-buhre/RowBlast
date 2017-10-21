#include "BlastRadiusAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "OfflineRasterizer.hpp"
#include "IImage.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace BlocksGame;

BlastRadiusAnimation::BlastRadiusAnimation(Pht::IEngine& engine, const GameScene& scene) :
    mScene {scene} {
    
    auto cellSize {scene.GetCellSize()};
    const auto edgeWidth {0.1f};
    const auto edgeLength {cellSize * 1.5f};
    auto squareSide {cellSize * 5.0f};
    Pht::Vec2 coordinateSystemSize {squareSide, squareSide};
    auto& renderer {engine.GetRenderer()};

    auto& renderBufferSize {renderer.GetRenderBufferSize()};
    auto& frustumSize {renderer.GetOrthographicFrustumSize()};
    
    auto xScaleFactor {static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)};
    auto yScaleFactor {static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)};
    
    Pht::IVec2 imageSize {
        static_cast<int>(squareSide * xScaleFactor) * 2,
        static_cast<int>(squareSide * yScaleFactor) * 2
    };
    
    auto rasterizer {std::make_unique<Pht::OfflineRasterizer>(coordinateSystemSize, imageSize)};
        
    const Pht::Vec4 edgeColor {1.0f, 1.0f, 1.0f, 0.82f};
    
    Pht::Vec2 lowerLeft1 {0.0f, 0.0f};
    Pht::Vec2 upperRight1 {squareSide, edgeWidth};
    rasterizer->DrawRectangle(upperRight1, lowerLeft1, edgeColor);

    Pht::Vec2 lowerLeft2 {squareSide - edgeWidth, 0.0f};
    Pht::Vec2 upperRight2 {squareSide, squareSide};
    rasterizer->DrawRectangle(upperRight2, lowerLeft2, edgeColor);

    Pht::Vec2 lowerLeft3 {0.0f, squareSide - edgeWidth};
    Pht::Vec2 upperRight3 {squareSide, squareSide};
    rasterizer->DrawRectangle(upperRight3, lowerLeft3, edgeColor);

    Pht::Vec2 lowerLeft4 {0.0f, 0.0f};
    Pht::Vec2 upperRight4 {edgeWidth, squareSide};
    rasterizer->DrawRectangle(upperRight4, lowerLeft4, edgeColor);

    auto image {rasterizer->ProduceImage()};
    
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    std::shared_ptr<Pht::RenderableObject> renderableObject {
        engine.CreateRenderableObject(Pht::QuadMesh {squareSide, squareSide}, imageMaterial)
    };
    
    mSceneObject = std::make_unique<Pht::SceneObject>(renderableObject);
}

void BlastRadiusAnimation::Start() {
    mState = State::Active;
}

void BlastRadiusAnimation::Stop() {
    mState = State::Inactive;
}

void BlastRadiusAnimation::SetPosition(const Pht::Vec2& position) {
    auto cellSize {mScene.GetCellSize()};
    auto& fieldLowerLeft {mScene.GetFieldLoweLeft()};

    Pht::Vec3 translation {
        position.x * cellSize + cellSize / 2.0f + fieldLowerLeft.x,
        position.y * cellSize + cellSize / 2.0f + fieldLowerLeft.y,
        mScene.GetBlastRadiusAnimationZ()
    };
    
    mSceneObject->ResetMatrix();
    mSceneObject->Translate(translation);
}

void BlastRadiusAnimation::Update(float dt) {
    if (mState == State::Inactive) {
        return;
    }
}

const Pht::SceneObject* BlastRadiusAnimation::GetSceneObject() const {
    switch (mState) {
        case State::Active:
            return mSceneObject.get();
        case State::Inactive:
            return nullptr;
    }
}

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

namespace {
    void DrawEdge(Pht::OfflineRasterizer& rasterizer, float squareSide, float cellSize) {
        const auto edgeLength {cellSize * 1.1f};
        const auto edgeWidth {0.09f};
        const Pht::Vec4 edgeColor {1.0f, 1.0f, 1.0f, 1.0f};
        
        Pht::Vec2 lowerLeft1 {squareSide - edgeLength, 0.0f};
        Pht::Vec2 upperRight1 {squareSide, edgeWidth};
        rasterizer.DrawRectangle(upperRight1, lowerLeft1, edgeColor);

        Pht::Vec2 lowerLeft2 {squareSide - edgeWidth, 0.0f};
        Pht::Vec2 upperRight2 {squareSide, edgeLength};
        rasterizer.DrawRectangle(upperRight2, lowerLeft2, edgeColor);

        Pht::Vec2 lowerLeft3 {squareSide - edgeWidth, squareSide - edgeLength};
        Pht::Vec2 upperRight3 {squareSide, squareSide};
        rasterizer.DrawRectangle(upperRight3, lowerLeft3, edgeColor);

        Pht::Vec2 lowerLeft4 {squareSide - edgeLength, squareSide - edgeWidth};
        Pht::Vec2 upperRight4 {squareSide, squareSide};
        rasterizer.DrawRectangle(upperRight4, lowerLeft4, edgeColor);

        Pht::Vec2 lowerLeft5 {0.0f, squareSide - edgeWidth};
        Pht::Vec2 upperRight5 {edgeLength, squareSide};
        rasterizer.DrawRectangle(upperRight5, lowerLeft5, edgeColor);

        Pht::Vec2 lowerLeft6 {0.0f, squareSide - edgeLength};
        Pht::Vec2 upperRight6 {edgeWidth, squareSide};
        rasterizer.DrawRectangle(upperRight6, lowerLeft6, edgeColor);

        Pht::Vec2 lowerLeft7 {0.0f, 0.0f};
        Pht::Vec2 upperRight7 {edgeWidth, edgeLength};
        rasterizer.DrawRectangle(upperRight7, lowerLeft7, edgeColor);

        Pht::Vec2 lowerLeft8 {0.0f, 0.0f};
        Pht::Vec2 upperRight8 {edgeLength, edgeWidth};
        rasterizer.DrawRectangle(upperRight8, lowerLeft8, edgeColor);
    }
    
    void DrawStripes(Pht::OfflineRasterizer& rasterizer, float squareSide) {
        const Pht::Vec4 fillColor {1.0f, 1.0f, 1.0f, 0.15f};
        const auto numStripes {3.5f};
        const auto stripeStep {squareSide / numStripes};
        const auto stripeWidth {(stripeStep / 2.0f) / std::sqrt(2.0f)};
        
        for (auto pos {0.0f}; pos < squareSide; pos += stripeStep) {
            Pht::Vec2 trapezoidLowerLeft {0.0f, squareSide - pos};
            Pht::Vec2 trapezoidUpperRight {pos, squareSide};
            rasterizer.DrawTiltedTrapezoid315(trapezoidUpperRight,
                                              trapezoidLowerLeft,
                                              stripeWidth,
                                              fillColor);
        }

        for (auto pos {0.0f}; pos < squareSide; pos += stripeStep) {
            Pht::Vec2 trapezoidLowerLeft {pos, 0.0f};
            Pht::Vec2 trapezoidUpperRight {squareSide, squareSide - pos};
            rasterizer.DrawTiltedTrapezoid45(trapezoidUpperRight,
                                             trapezoidLowerLeft,
                                             stripeWidth,
                                             fillColor);
        }
    }
}

BlastRadiusAnimation::BlastRadiusAnimation(Pht::IEngine& engine, const GameScene& scene) :
    mScene {scene} {
    
    auto cellSize {scene.GetCellSize()};
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
    
    DrawEdge(*rasterizer, squareSide, cellSize);
    DrawStripes(*rasterizer, squareSide);

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
    mTime = 0.0f;
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
    
    mTime += dt;
    const auto opacityCurveAmplitude {0.2f};
    const auto frequency {0.7f};
    
    auto opacity {
        1.0f - opacityCurveAmplitude + opacityCurveAmplitude * std::sin(2.0f * 3.1415f * frequency * mTime)
    };
    
    auto& material {mSceneObject->GetRenderable().GetMaterial()};
    material.SetOpacity(opacity);
}

const Pht::SceneObject* BlastRadiusAnimation::GetSceneObject() const {
    switch (mState) {
        case State::Active:
            return mSceneObject.get();
        case State::Inactive:
            return nullptr;
    }
}

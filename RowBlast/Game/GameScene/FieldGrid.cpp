#include "FieldGrid.hpp"

// Engine incudes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "OfflineRasterizer.hpp"
#include "IImage.hpp"
#include "ISceneManager.hpp"
#include "Vector.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto gridSegmentSideNumCells {9};
    constexpr auto lineWidth {0.07f};
    const Pht::Vec4 lineColor {1.0f, 1.0f, 1.0f, 0.045f};

    std::unique_ptr<Pht::OfflineRasterizer> CreateRasterizer(Pht::IEngine& engine,
                                                             const Pht::Vec2& coordinateSystemSize,
                                                             const CommonResources& commonResources) {
        auto& renderer {engine.GetRenderer()};
        auto& renderBufferSize {renderer.GetRenderBufferSize()};
        auto& frustumSize {commonResources.GetOrthographicFrustumSizePotentiallyZoomedScreen()};

        auto xScaleFactor {
            static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)
        };
    
        auto yScaleFactor {
            static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)
        };

        Pht::IVec2 imageSize {
            static_cast<int>(coordinateSystemSize.x * xScaleFactor * 2.0f),
            static_cast<int>(coordinateSystemSize.y * yScaleFactor * 2.0f)
        };

        return std::make_unique<Pht::OfflineRasterizer>(coordinateSystemSize, imageSize);
    }
}

FieldGrid::FieldGrid(Pht::IEngine& engine,
                     GameScene& scene,
                     const CommonResources& commonResources) :
    mScene {scene} {
    
    auto cellSize {scene.GetCellSize()};
    auto gridSegmentSide {static_cast<float>(gridSegmentSideNumCells) * cellSize + lineWidth};
    Pht::Vec2 gridSegmentSize {gridSegmentSide, gridSegmentSide};
    auto rasterizer {CreateRasterizer(engine, gridSegmentSize, commonResources)};
    
    for (auto column {0}; column < gridSegmentSideNumCells + 1; ++column) {
        auto x {column * cellSize};
        Pht::Vec2 lowerLeft {x, 0.0f};
        Pht::Vec2 upperRight {x + lineWidth, gridSegmentSide - lineWidth};
        rasterizer->DrawRectangle(upperRight, lowerLeft, lineColor, Pht::DrawOver::Yes);
    }

    for (auto row {0}; row < gridSegmentSideNumCells; ++row) {
        auto y {row * cellSize};
        Pht::Vec2 lowerLeft {0.0, y};
        Pht::Vec2 upperRight {gridSegmentSide, y + lineWidth};
        rasterizer->DrawRectangle(upperRight, lowerLeft, lineColor, Pht::DrawOver::Yes);
    }
    
    auto image {rasterizer->ProduceImage()};

    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    auto& sceneManager {engine.GetSceneManager()};
    mGridSegmentRenderable =
        sceneManager.CreateRenderableObject(Pht::QuadMesh {gridSegmentSide, gridSegmentSide},
                                            imageMaterial);
}

void FieldGrid::Init(int numRows) {
    auto& container {mScene.GetFieldQuadContainer()};
    auto& fieldLowerLeft {mScene.GetFieldLoweLeft()};
    auto segmentSide {mScene.GetCellSize() * static_cast<float>(gridSegmentSideNumCells)};
    auto numSegments {numRows / gridSegmentSideNumCells + 1};
    
    for (auto i {0}; i < numSegments; ++i) {
        Pht::Vec3 gridSegmentPosition {
            fieldLowerLeft.x + segmentSide / 2.0f,
            fieldLowerLeft.y + segmentSide / 2.0f + static_cast<float>(i) * segmentSide,
            mScene.GetFieldGridZ()
        };
        
        auto& gridSegmentSceneObject {mScene.GetScene().CreateSceneObject()};
        gridSegmentSceneObject.SetRenderable(mGridSegmentRenderable.get());
        gridSegmentSceneObject.GetTransform().SetPosition(gridSegmentPosition);
        
        container.AddChild(gridSegmentSceneObject);
    }
}

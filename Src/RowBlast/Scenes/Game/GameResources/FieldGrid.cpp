#include "FieldGrid.hpp"

#include <assert.h>

// Engine incudes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "SoftwareRasterizer.hpp"
#include "IImage.hpp"
#include "ISceneManager.hpp"
#include "Vector.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"
#include "Level.hpp"
#include "FieldBorder.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec4 transparentColor {0.0f, 0.0f, 0.0f, 0.0f};
    const Pht::Vec4 darkerColor {0.0f, 0.0f, 0.2f, 0.05f};
    constexpr auto gridSegmentHeightNumCells = 8;
    
    std::unique_ptr<Pht::SoftwareRasterizer>
    CreateRasterizer(Pht::IEngine& engine,
                     const Pht::Vec2& coordinateSystemSize,
                     const CommonResources& commonResources) {
        auto& renderer = engine.GetRenderer();
        auto& renderBufferSize = renderer.GetRenderBufferSize();
        auto& frustumSize = commonResources.GetOrthographicFrustumSizePotentiallyZoomedScreen();

        auto xScaleFactor =
            static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x);
    
        auto yScaleFactor =
            static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y);

        Pht::IVec2 imageSize {
            static_cast<int>(coordinateSystemSize.x * xScaleFactor * 2.0f),
            static_cast<int>(coordinateSystemSize.y * yScaleFactor * 2.0f)
        };

        return std::make_unique<Pht::SoftwareRasterizer>(coordinateSystemSize, imageSize);
    }

    std::unique_ptr<Pht::RenderableObject>
    CreateSegmentRenderable(Pht::IEngine& engine,
                            GameScene& scene,
                            const CommonResources& commonResources,
                            int gridSegmentWidthNumCells) {
        auto cellSize = scene.GetCellSize();
        auto gridSegmentWidth = static_cast<float>(gridSegmentWidthNumCells) * cellSize;
        auto gridSegmentHeight = static_cast<float>(gridSegmentHeightNumCells) * cellSize;
        Pht::Vec2 gridSegmentSize {gridSegmentWidth, gridSegmentHeight};
        auto rasterizer = CreateRasterizer(engine, gridSegmentSize, commonResources);

        for (auto row = 0; row < gridSegmentHeightNumCells; ++row) {
            auto y = row * cellSize;
            auto counter = row % 2;
            for (auto column = 0; column < gridSegmentWidthNumCells; ++column) {
                auto x = column * cellSize;
                Pht::Vec2 lowerLeft {x, y};
                Pht::Vec2 upperRight {x + cellSize, y + cellSize};
                auto color = counter % 2 ? darkerColor : transparentColor;
                rasterizer->DrawRectangle(upperRight, lowerLeft, color, Pht::DrawOver::Yes);
                ++counter;
            }
        }
        
        auto image = rasterizer->ProduceImage();

        Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
        imageMaterial.SetBlend(Pht::Blend::Yes);
        auto& sceneManager = engine.GetSceneManager();
        return sceneManager.CreateRenderableObject(Pht::QuadMesh {gridSegmentWidth, gridSegmentHeight},
                                                   imageMaterial);
    }
}

FieldGrid::FieldGrid(Pht::IEngine& engine,
                     GameScene& scene,
                     const CommonResources& commonResources) :
    mScene {scene},
    mGridSegmentSize7Renderable {CreateSegmentRenderable(engine, scene, commonResources, 7)},
    mGridSegmentSize8Renderable {CreateSegmentRenderable(engine, scene, commonResources, 8)},
    mGridSegmentSize9Renderable {CreateSegmentRenderable(engine, scene, commonResources, 9)} {}

void FieldGrid::Init(const Level& level) {
    auto& container = mScene.GetFieldQuadContainer();
    auto& fieldLowerLeft = mScene.GetFieldLoweLeft();
    auto gridSegmentWidthNumCells = level.GetNumColumns();
    auto segmentWidth = mScene.GetCellSize() * static_cast<float>(gridSegmentWidthNumCells);
    auto segmentHeight = mScene.GetCellSize() * static_cast<float>(gridSegmentHeightNumCells);
    auto numSegments = level.GetNumRows() / gridSegmentHeightNumCells + 1;
    
    mSegments.clear();
    
    for (auto i = 0; i < numSegments; ++i) {
        Pht::Vec3 gridSegmentPosition {
            fieldLowerLeft.x + segmentWidth / 2.0f,
            fieldLowerLeft.y + segmentHeight / 2.0f + static_cast<float>(i) * segmentHeight,
            mScene.GetFieldGridZ()
        };
        
        auto& gridSegmentSceneObject = mScene.GetScene().CreateSceneObject();
        gridSegmentSceneObject.GetTransform().SetPosition(gridSegmentPosition);
        gridSegmentSceneObject.SetRenderable(&GetGridSegmentRenderable(gridSegmentWidthNumCells));
        container.AddChild(gridSegmentSceneObject);

        Segment gridSegment {i * gridSegmentHeightNumCells, gridSegmentSceneObject};
        mSegments.push_back(gridSegment);
    }
    
    AddBottomRow(container, level.GetNumRows(), gridSegmentWidthNumCells);
}

Pht::RenderableObject& FieldGrid::GetGridSegmentRenderable(int gridSegmentWidthNumCells) {
    switch (gridSegmentWidthNumCells) {
        case 7:
            return *mGridSegmentSize7Renderable;
        case 8:
            return *mGridSegmentSize8Renderable;
        case 9:
            return *mGridSegmentSize9Renderable;
        default:
            assert(!"Unsupported number of columns");
            break;
    }
}

void FieldGrid::AddBottomRow(Pht::SceneObject& container,
                             int numLevelRows,
                             int gridSegmentWidthNumCells) {
    Pht::Material rowBelowMaterial;

    auto width = gridSegmentWidthNumCells * mScene.GetCellSize() + mScene.GetFieldPadding() +
                 FieldBorder::borderThickness + FieldBorder::frameThickness;
    auto height = mScene.GetRowBelowSliceHeightInCells() * mScene.GetCellSize();

    Pht::Vec4 upperColor {0.8f, 0.9f, 1.0f, 1.0f};
    Pht::Vec4 lowerColor {0.8f, 0.9f, 1.0f, 0.7f};

    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, lowerColor},
        {{width / 2.0f, -height / 2.0f, 0.0f}, lowerColor},
        {{width / 2.0f, height / 2.0f, 0.0f}, upperColor},
        {{-width / 2.0f, height / 2.0f, 0.0f}, upperColor},
    };

    auto& quad = mScene.GetScene().CreateSceneObject(Pht::QuadMesh {vertices}, rowBelowMaterial);
    auto& fieldLowerLeft = mScene.GetFieldLoweLeft();
    auto segmentWidth = mScene.GetCellSize() * static_cast<float>(gridSegmentWidthNumCells);
    
    Pht::Vec3 position {
        fieldLowerLeft.x + segmentWidth / 2.0f,
        fieldLowerLeft.y - height / 2.0f,
        mScene.GetFieldGridZ() + 0.05f
    };
    
    quad.GetTransform().SetPosition(position);
    container.AddChild(quad);
}

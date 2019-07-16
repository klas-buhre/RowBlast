#include "GhostPieceBlocks.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ISceneManager.hpp"
#include "QuadMesh.hpp"
#include "SoftwareRasterizer.hpp"
#include "IImage.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto borderWidth = 0.04f;
    constexpr auto padding = 0.02f;
    constexpr auto borderColorAlpha = 0.83f;
    constexpr auto innerBorderColorAlpha = 0.55f;
    constexpr auto innerColorAlpha = 0.3525f;

    const Pht::Vec4 redBorderColor {1.0f, 0.5f, 0.5f, borderColorAlpha};
    const Pht::Vec4 greenBorderColor {0.5f, 0.79f, 0.5f, borderColorAlpha};
    const Pht::Vec4 blueBorderColor {0.3f, 0.72f, 1.0f, borderColorAlpha};
    const Pht::Vec4 yellowBorderColor {0.875f, 0.75f, 0.0f, borderColorAlpha};

    Pht::Vec4 ToBorderColor(BlockColor blockColor) {
        switch (blockColor) {
            case BlockColor::Red:
                return redBorderColor;
            case BlockColor::Green:
                return greenBorderColor;
            case BlockColor::Blue:
                return blueBorderColor;
            case BlockColor::Yellow:
                return yellowBorderColor;
            default:
                assert(false);
                break;
        }
    }

    std::unique_ptr<Pht::RenderableObject>
    CreateBlockRenderable(BlockColor blockColor,
                          const Pht::Vec2& size,
                          Pht::SoftwareRasterizer& rasterizer,
                          Pht::IEngine& engine) {
        rasterizer.ClearBuffer();
        
        auto borderColor = ToBorderColor(blockColor);
        Pht::Vec2 lowerLeft {padding, padding};
        Pht::Vec2 upperRight {size.x - padding, size.y - padding};
        rasterizer.DrawRectangle(upperRight, lowerLeft, borderColor);

        auto innerBorderColor = borderColor;
        innerBorderColor.w = innerBorderColorAlpha;
        Pht::Vec2 lowerLeft2 {padding + borderWidth, padding + borderWidth};
        Pht::Vec2 upperRight2 {size.x - borderWidth - padding, size.y - borderWidth - padding};
        rasterizer.DrawRectangle(upperRight2, lowerLeft2, innerBorderColor, Pht::DrawOver::Yes);

        auto innerColor = borderColor;
        innerColor.w = innerColorAlpha;
        Pht::Vec2 lowerLeft3 {padding + borderWidth * 2.0f, padding + borderWidth * 2.0f};
        Pht::Vec2 upperRight3 {size.x - borderWidth * 2.0f - padding, size.y - borderWidth * 2.0f - padding};
        rasterizer.DrawRectangle(upperRight3, lowerLeft3, innerColor, Pht::DrawOver::Yes);

        auto image = rasterizer.ProduceImage();
        
        Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
        imageMaterial.SetBlend(Pht::Blend::Yes);
        
        auto& sceneManager = engine.GetSceneManager();
        return sceneManager.CreateRenderableObject(Pht::QuadMesh {size.x, size.y},
                                                   imageMaterial);
    }
}

GhostPieceBlocks::GhostPieceBlocks(Pht::IEngine& engine, const CommonResources& commonResources) {
    auto numBlocks = Quantities::numBlockRenderables * Quantities::numBlockColors;
    mBlocks.resize(numBlocks);
    
    auto cellSize = commonResources.GetCellSize();
    Pht::Vec2 coordinateSystemSize {cellSize, cellSize};
        
    auto& renderer = engine.GetRenderer();
    auto& renderBufferSize = renderer.GetRenderBufferSize();
    auto& frustumSize = commonResources.GetOrthographicFrustumSizePotentiallyZoomedScreen();
    
    auto xScaleFactor = static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x);
    auto yScaleFactor = static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y);
    
    Pht::IVec2 imageSize {
        static_cast<int>(cellSize * xScaleFactor) * 2,
        static_cast<int>(cellSize * yScaleFactor) * 2
    };
        
    auto rasterizer = std::make_unique<Pht::SoftwareRasterizer>(coordinateSystemSize, imageSize);

    for (auto colorIndex = 0; colorIndex < Quantities::numBlockColors; ++colorIndex) {
        auto color = static_cast<BlockColor>(colorIndex);
        auto blockIndex = CalcBlockIndex(BlockKind::Full, color);
        
        auto renderableObject = CreateBlockRenderable(color,
                                                      coordinateSystemSize,
                                                      *rasterizer,
                                                      engine);
        
        mBlocks[blockIndex] = std::move(renderableObject);
    }
}

Pht::RenderableObject& GhostPieceBlocks::GetBlockRenderableObject(BlockKind blockKind,
                                                                  BlockColor color) const {
    return *(mBlocks[CalcBlockIndex(blockKind, color)]);
}

int GhostPieceBlocks::CalcBlockIndex(BlockKind blockKind, BlockColor color) const {
    auto blockKindIndex = static_cast<int>(blockKind);
    auto colorIndex = static_cast<int>(color);
    
    assert(blockKindIndex >= 0 && blockKindIndex < Quantities::numBlockRenderables &&
           colorIndex >= 0 && colorIndex < Quantities::numBlockColors);
    
    auto index = colorIndex * Quantities::numBlockRenderables + blockKindIndex;
    
    assert(index < mBlocks.size());
    return index;
}

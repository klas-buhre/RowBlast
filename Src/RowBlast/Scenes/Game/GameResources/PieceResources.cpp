#include "PieceResources.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "Material.hpp"
#include "ObjMesh.hpp"
#include "QuadMesh.hpp"
#include "BoxMesh.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "FlashingBlocksAnimationSystem.hpp"

using namespace RowBlast;

namespace {
    constexpr auto numBondRenderables = 3;
    constexpr auto diagonalBondAlpha = 0.5f;
    constexpr auto ghostPieceOpacity = 0.55f;
    constexpr auto cellSize = 1.25f;
    
    std::string ToMeshName(BlockKind blockKind) {
        switch (blockKind) {
            case BlockKind::LowerRightHalf:
                return "triangle_320.obj";
            case BlockKind::UpperRightHalf:
                return "triangle_320_r270.obj";
            case BlockKind::UpperLeftHalf:
                return "triangle_320_r180.obj";
            case BlockKind::LowerLeftHalf:
                return "triangle_320_r90.obj";
            case BlockKind::Full:
                return "cube_428.obj";
            default:
                assert(!"Unsupported block kind");
                break;
        }
    }
    
    const Pht::Material& ToMaterial(BlockColor color, const CommonResources& commonResources) {
        switch (color) {
            case BlockColor::Red:
                return commonResources.GetMaterials().GetRedFieldBlockMaterial();
            case BlockColor::Green:
                return commonResources.GetMaterials().GetGreenFieldBlockMaterial();
            case BlockColor::Blue:
                return commonResources.GetMaterials().GetBlueFieldBlockMaterial();
            case BlockColor::Yellow:
                return commonResources.GetMaterials().GetYellowFieldBlockMaterial();
            case BlockColor::None:
                assert(!"Not a color");
                break;
        }
    }
    
    void AddColorToMaterial(Pht::Material& material, const Pht::Color& colorAdd) {
        material.SetAmbient(material.GetAmbient() + colorAdd);
        material.SetDiffuse(material.GetDiffuse() + colorAdd);
        material.SetSpecular(material.GetSpecular() + colorAdd);
    }
    
    Pht::Material ToMaterial(BlockColor color,
                             BlockBrightness brightness,
                             const CommonResources& commonResources) {
        auto material = ToMaterial(color, commonResources);
        
        switch (brightness) {
            case BlockBrightness::Normal:
                break;
            case BlockBrightness::Flashing:
                AddColorToMaterial(material, FlashingBlocksAnimationSystem::colorAdd);
                break;
            case BlockBrightness::BlueprintFillFlashing:
                AddColorToMaterial(material, FlashingBlocksAnimationSystem::brightColorAdd);
                break;
            case BlockBrightness::SemiFlashing:
                AddColorToMaterial(material, FlashingBlocksAnimationSystem::semiFlashingColorAdd);
                break;
        }
        
        return material;
    }
    
    std::unique_ptr<Pht::RenderableObject>
    ToBondRenderableObject(BondRenderableKind bondRenderableKind,
                           const Pht::Material& material,
                           Pht::ISceneManager& sceneManager) {
        switch (bondRenderableKind) {
            case BondRenderableKind::Normal:
                return sceneManager.CreateRenderableObject(Pht::QuadMesh {0.127f,
                                                                          0.95f,
                                                                          std::string{"normalBond"}},
                                                           material);
            case BondRenderableKind::Aslope:
                return sceneManager.CreateRenderableObject(Pht::QuadMesh {0.19f,
                                                                          0.68f,
                                                                          std::string{"aslopeBond"}},
                                                           material);
            case BondRenderableKind::Diagonal:
                return sceneManager.CreateRenderableObject(Pht::ObjMesh {"bond_76.obj", 0.95f},
                                                           material);
        }
    }
    
    
}

PieceResources::PieceResources(Pht::IEngine& engine, const CommonResources& commonResources) {
    auto& sceneManager = engine.GetSceneManager();
    
    CreateBlocks(sceneManager, commonResources);
    CreateBonds(sceneManager, commonResources);
    CreatePreviewAslopeBonds(sceneManager, commonResources);
    CreateBombs(sceneManager);
}

Pht::RenderableObject& PieceResources::GetBlockRenderableObject(BlockKind blockKind,
                                                                BlockColor color,
                                                                BlockBrightness brightness) const {
    return *(mBlocks[CalcBlockIndex(blockKind, color, brightness)]);
}

int PieceResources::CalcBlockIndex(BlockKind blockKind,
                                   BlockColor color,
                                   BlockBrightness brightness) const {
    auto blockKindIndex = static_cast<int>(blockKind);
    auto colorIndex = static_cast<int>(color);
    auto brightnessIndex = static_cast<int>(brightness);
    
    assert(blockKindIndex >= 0 && blockKindIndex < Quantities::numBlockRenderables &&
           colorIndex >= 0 && colorIndex < Quantities::numBlockColors &&
           brightnessIndex >= 0 && brightnessIndex < Quantities::numBlockBrightness);

    auto index =
        brightnessIndex * Quantities::numBlockRenderables * Quantities::numBlockColors +
        colorIndex * Quantities::numBlockRenderables + blockKindIndex;
    
    assert(index < mBlocks.size());
    return index;
}

Pht::RenderableObject& PieceResources::GetBondRenderableObject(BondRenderableKind bondRenderable,
                                                               BlockColor color,
                                                               BlockBrightness brightness) const {
    return *(mBonds[CalcBondIndex(bondRenderable, color, brightness)]);
}

int PieceResources::CalcBondIndex(BondRenderableKind bondRenderable,
                                  BlockColor color,
                                  BlockBrightness brightness) const {
    auto bondRenderableIndex = static_cast<int>(bondRenderable);
    auto colorIndex = static_cast<int>(color);
    auto brightnessIndex = static_cast<int>(brightness);
    
    assert(bondRenderableIndex >= 0 && bondRenderableIndex < numBondRenderables &&
           colorIndex >= 0 && colorIndex < Quantities::numBlockColors &&
           brightnessIndex >= 0 && brightnessIndex < Quantities::numBondBrightness);

    auto index =
        brightnessIndex * numBondRenderables * Quantities::numBlockColors +
        colorIndex * numBondRenderables + bondRenderableIndex;
    
    assert(index < mBonds.size());
    return index;
}

Pht::RenderableObject& PieceResources::GetPreviewAslopeBondRenderableObject(BlockColor color) const {
    auto colorIndex = static_cast<int>(color);
    assert(colorIndex >= 0 && colorIndex < Quantities::numBlockColors);
    return *(mPreviewAslopeBonds[static_cast<int>(color)]);
}

void PieceResources::CreateBlocks(Pht::ISceneManager& sceneManager,
                                  const CommonResources& commonResources) {
    auto numBlocks =
        Quantities::numBlockRenderables * Quantities::numBlockColors * Quantities::numBlockBrightness;
    
    mBlocks.resize(numBlocks);
    
    for (auto blockKindIndex = 0;
         blockKindIndex < Quantities::numBlockRenderables;
         ++blockKindIndex) {
        
        for (auto colorIndex = 0; colorIndex < Quantities::numBlockColors; ++colorIndex) {
        
            for (auto brightnessIndex = 0;
                 brightnessIndex < Quantities::numBlockBrightness;
                 ++brightnessIndex) {
                
                auto blockKind = static_cast<BlockKind>(blockKindIndex);
                auto color = static_cast<BlockColor>(colorIndex);
                auto brightness = static_cast<BlockBrightness>(brightnessIndex);
                
                auto meshName = ToMeshName(blockKind);
                auto material = ToMaterial(color, brightness, commonResources);
                auto blockIndex = CalcBlockIndex(blockKind, color, brightness);
                
                auto renderableObject =
                    sceneManager.CreateRenderableObject(Pht::ObjMesh {meshName, cellSize}, material);
                
                mBlocks[blockIndex] = std::move(renderableObject);
            }
        }
    }
}

void PieceResources::CreateBonds(Pht::ISceneManager& sceneManager,
                                 const CommonResources& commonResources) {
    auto numBonds = numBondRenderables * Quantities::numBlockColors * Quantities::numBondBrightness;
    mBonds.resize(numBonds);
    
    for (auto bondRenderableIndex = 0;
         bondRenderableIndex < numBondRenderables;
         ++bondRenderableIndex) {
        
        for (auto colorIndex = 0; colorIndex < Quantities::numBlockColors; ++colorIndex) {
        
            for (auto brightnessIndex = 0;
                 brightnessIndex < Quantities::numBondBrightness;
                 ++brightnessIndex) {
                
                auto bondRenderableKind = static_cast<BondRenderableKind>(bondRenderableIndex);
                auto color = static_cast<BlockColor>(colorIndex);
                auto brightness = static_cast<BlockBrightness>(brightnessIndex);
                
                auto material = ToMaterial(color, brightness, commonResources);
                
                if (bondRenderableKind == BondRenderableKind::Diagonal) {
                    material.SetOpacity(diagonalBondAlpha);
                }
                
                auto bondIndex = CalcBondIndex(bondRenderableKind, color, brightness);
                mBonds[bondIndex] = ToBondRenderableObject(bondRenderableKind,
                                                           material,
                                                           sceneManager);
            }
        }
    }
}

void PieceResources::CreatePreviewAslopeBonds(Pht::ISceneManager& sceneManager,
                                              const CommonResources& commonResources) {
    mPreviewAslopeBonds.resize(Quantities::numBlockColors);
    
    for (auto colorIndex = 0; colorIndex < Quantities::numBlockColors; ++colorIndex) {
        auto color = static_cast<BlockColor>(colorIndex);
        auto material = ToMaterial(color, BlockBrightness::Normal, commonResources);
        
        auto renderable = sceneManager.CreateRenderableObject(Pht::BoxMesh {0.14f,
                                                                            0.6f,
                                                                            cellSize,
                                                                            std::string{"previewAslopeBond"}},
                                                              material);
        mPreviewAslopeBonds[colorIndex] = std::move(renderable);
    }
}

void PieceResources::CreateBombs(Pht::ISceneManager& sceneManager) {
    Pht::Material bombMaterial {
        "bomb_798.jpg",
        "bomb_798_emission.jpg",
        1.0f,
        0.87f,
        1.0f,
        1.0f,
        30.0f
    };

    bombMaterial.SetEmissive(Pht::Color {3.0f, 3.0f, 3.0f});
    bombMaterial.SetDepthTestAllowedOverride(true);
    mBomb = sceneManager.CreateRenderableObject(Pht::ObjMesh {"bomb_798.obj", 16.2f}, bombMaterial);
    
    bombMaterial.SetOpacity(ghostPieceOpacity);
    mTransparentBomb = sceneManager.CreateRenderableObject(Pht::ObjMesh {"bomb_798.obj", 16.2f},
                                                           bombMaterial);

    Pht::Material rowBombMaterial {
        "laser_bomb_diffuse.jpg",
        "laser_bomb_emission.png",
        0.55f,
        0.65f,
        0.05f,
        1.0f,
        20.0f
    };
    
    rowBombMaterial.SetAmbient(Pht::Color {0.5f, 0.5f, 0.85f});
    rowBombMaterial.SetEmissive(Pht::Color {2.0f, 2.0f, 2.0f});
    rowBombMaterial.SetDepthTestAllowedOverride(true);
    mRowBomb = sceneManager.CreateRenderableObject(Pht::ObjMesh {"laser_bomb_224.obj", 0.6f},
                                                   rowBombMaterial);

    rowBombMaterial.SetOpacity(ghostPieceOpacity);
    mTransparentRowBomb = sceneManager.CreateRenderableObject(Pht::ObjMesh {"laser_bomb_224.obj", 0.6f},
                                                              rowBombMaterial);

}

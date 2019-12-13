#include "LevelResources.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ObjMesh.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "SoftwareRasterizer.hpp"
#include "IImage.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "SmallTrianglePiece.hpp"
#include "TrianglePiece.hpp"
#include "BigTrianglePiece.hpp"
#include "DiamondPiece.hpp"
#include "PyramidPiece.hpp"
#include "LongIPiece.hpp"
#include "MiddleIPiece.hpp"
#include "ShortIPiece.hpp"
#include "LPiece.hpp"
#include "BPiece.hpp"
#include "DPiece.hpp"
#include "SevenPiece.hpp"
#include "MirroredSevenPiece.hpp"
#include "FPiece.hpp"
#include "MirroredFPiece.hpp"
#include "BigLPiece.hpp"
#include "ZPiece.hpp"
#include "MirroredZPiece.hpp"
#include "TPiece.hpp"
#include "PlusPiece.hpp"
#include "Bomb.hpp"
#include "RowBomb.hpp"

using namespace RowBlast;

LevelResources::LevelResources(Pht::IEngine& engine, const CommonResources& commonResources) {
    CreatePieceTypes(engine, commonResources);
    CreateGreyBlockRenderables(engine.GetSceneManager(), commonResources);
    CreateBlueprintRenderables(engine, commonResources);
    CreateLevelBombRenderable(engine);
    CreateBigAsteroidRenderable(engine);
    CreateSmallAsteroidRenderable(engine);
    CreateAsteroidFragmentRenderable(engine);
}

void LevelResources::CreatePieceTypes(Pht::IEngine& engine,
                                      const CommonResources& commonResources) {
    mPieceTypes["LongI"] = std::make_unique<LongIPiece>(engine, commonResources);
    mPieceTypes["I"] = std::make_unique<MiddleIPiece>(engine, commonResources);
    mPieceTypes["ShortI"] = std::make_unique<ShortIPiece>(engine, commonResources);
    mPieceTypes["L"] = std::make_unique<LPiece>(engine, commonResources);
    mPieceTypes["B"] = std::make_unique<BPiece>(engine, commonResources);
    mPieceTypes["D"] = std::make_unique<DPiece>(engine, commonResources);
    mPieceTypes["Seven"] = std::make_unique<SevenPiece>(engine, commonResources);
    mPieceTypes["MirroredSeven"] = std::make_unique<MirroredSevenPiece>(engine, commonResources);
    mPieceTypes["F"] = std::make_unique<FPiece>(engine, commonResources);
    mPieceTypes["MirroredF"] = std::make_unique<MirroredFPiece>(engine, commonResources);
    mPieceTypes["BigL"] = std::make_unique<BigLPiece>(engine, commonResources);
    mPieceTypes["Z"] = std::make_unique<ZPiece>(engine, commonResources);
    mPieceTypes["MirroredZ"] = std::make_unique<MirroredZPiece>(engine, commonResources);
    mPieceTypes["T"] = std::make_unique<TPiece>(engine, commonResources);
    mPieceTypes["Plus"] = std::make_unique<PlusPiece>(engine, commonResources);
    mPieceTypes["SmallTriangle"] = std::make_unique<SmallTrianglePiece>(engine, commonResources);
    mPieceTypes["Triangle"] = std::make_unique<TrianglePiece>(engine, commonResources);
    mPieceTypes["BigTriangle"] = std::make_unique<BigTrianglePiece>(engine, commonResources);
    mPieceTypes["Diamond"] = std::make_unique<DiamondPiece>(engine, commonResources);
    mPieceTypes["Pyramid"] = std::make_unique<PyramidPiece>(engine, commonResources);
    mPieceTypes["Bomb"] = std::make_unique<Bomb>();
    mPieceTypes["RowBomb"] = std::make_unique<RowBomb>();
}

void LevelResources::CreateGreyBlockRenderables(Pht::ISceneManager& sceneManager,
                                                const CommonResources& commonResources) {
    auto cellSize = commonResources.GetCellSize();
    auto& grayFieldBlockMaterial = commonResources.GetMaterials().GetGrayFieldBlockMaterial();
    
    mGrayCube = sceneManager.CreateRenderableObject(
        Pht::ObjMesh {"cube_428.obj", cellSize},
        grayFieldBlockMaterial
    );
    
    mGrayTriangle = sceneManager.CreateRenderableObject(
        Pht::ObjMesh {"triangle_320.obj", cellSize},
        grayFieldBlockMaterial
    );
}

void LevelResources::CreateBlueprintRenderables(Pht::IEngine& engine,
                                                const CommonResources& commonResources) {
    const auto edgeWidth = 0.07f;
    auto cellSize = commonResources.GetCellSize();
    auto squareSide = cellSize + edgeWidth;
    Pht::Vec2 coordinateSystemSize {squareSide, squareSide};
    auto& renderer = engine.GetRenderer();

    auto& renderBufferSize = renderer.GetRenderBufferSize();
    auto& frustumSize = commonResources.GetOrthographicFrustumSizePotentiallyZoomedScreen();
    
    auto xScaleFactor = static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x);
    auto yScaleFactor = static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y);
    
    Pht::IVec2 imageSize {
        static_cast<int>(squareSide * xScaleFactor) * 2,
        static_cast<int>(squareSide * yScaleFactor) * 2
    };
    
    auto rasterizer = std::make_unique<Pht::SoftwareRasterizer>(coordinateSystemSize, imageSize);
    
    Pht::Vec4 fillColor {1.0f, 1.0f, 1.0f, 0.192f};
    
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {squareSide, squareSide};
    rasterizer->DrawRectangle(upperRight, lowerLeft, fillColor);
    
    Pht::Vec4 edgeColor {1.0f, 1.0f, 1.0f, 0.265f};
    
    Pht::Vec2 lowerLeft1 {0.0f, 0.0f};
    Pht::Vec2 upperRight1 {squareSide, edgeWidth};
    rasterizer->DrawRectangle(upperRight1, lowerLeft1, edgeColor, Pht::DrawOver::Yes);

    Pht::Vec2 lowerLeft2 {squareSide - edgeWidth, 0.0f};
    Pht::Vec2 upperRight2 {squareSide, squareSide};
    rasterizer->DrawRectangle(upperRight2, lowerLeft2, edgeColor, Pht::DrawOver::Yes);

    Pht::Vec2 lowerLeft3 {0.0f, squareSide - edgeWidth};
    Pht::Vec2 upperRight3 {squareSide, squareSide};
    rasterizer->DrawRectangle(upperRight3, lowerLeft3, edgeColor, Pht::DrawOver::Yes);

    Pht::Vec2 lowerLeft4 {0.0f, 0.0f};
    Pht::Vec2 upperRight4 {edgeWidth, squareSide};
    rasterizer->DrawRectangle(upperRight4, lowerLeft4, edgeColor, Pht::DrawOver::Yes);

    auto image = rasterizer->ProduceImage();
    
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    imageMaterial.SetDepthWrite(true);
    auto& sceneManager = engine.GetSceneManager();
    mBlueprintSquare = sceneManager.CreateRenderableObject(Pht::QuadMesh {squareSide, squareSide},
                                                           imageMaterial);

    Pht::Material imageMaterialNonDepthWriting {*image, Pht::GenerateMipmap::Yes};
    imageMaterialNonDepthWriting.SetBlend(Pht::Blend::Yes);
    mBlueprintSquareNonDepthWriting =
        sceneManager.CreateRenderableObject(Pht::QuadMesh {squareSide, squareSide},
                                            imageMaterialNonDepthWriting);

    Pht::Material animationMaterial {Pht::Color {1.0f, 1.0f, 1.0f}};
    animationMaterial.SetOpacity(SlotFillAnimationComponent::mInitialOpacity);

    mBlueprintSquareAnimation = sceneManager.CreateRenderableObject(
        Pht::QuadMesh {cellSize, cellSize},
        animationMaterial
    );

    auto f = 0.9f;
    Pht::Vec4 fieldColor {0.52f * f, 0.42f * f, 0.752f * f, 1.0f};
    Pht::QuadMesh::Vertices fieldCellVertices  {
        {{-squareSide / 2.0f, -squareSide / 2.0f, 0.0f}, fieldColor},
        {{squareSide / 2.0f, -squareSide / 2.0f, 0.0f}, fieldColor},
        {{squareSide / 2.0f, squareSide / 2.0f, 0.0f}, fieldColor},
        {{-squareSide / 2.0f, squareSide / 2.0f, 0.0f}, fieldColor},
    };

    Pht::Material fieldMaterial;
    fieldMaterial.SetOpacity(0.96f);
    mFieldCell = sceneManager.CreateRenderableObject(Pht::QuadMesh {fieldCellVertices},
                                                     fieldMaterial);
}

void LevelResources::CreateLevelBombRenderable(Pht::IEngine& engine) {
    Pht::Material bombMaterial {
        "bomb_798.jpg",
        "bomb_798_emission.jpg",
        1.2f,
        1.0f,
        1.0f,
        1.0f,
        50.0f
    };

    bombMaterial.SetAmbient(Pht::Color {1.18f, 1.18f, 1.7f});
    bombMaterial.SetDiffuse(Pht::Color {0.8f, 0.8f, 1.6f});
    bombMaterial.SetEmissive(Pht::Color {3.0f, 3.0f, 3.0f});
    auto& sceneManager = engine.GetSceneManager();
    mLevelBomb = sceneManager.CreateRenderableObject(Pht::ObjMesh {"bomb_798.obj", 16.2f},
                                                     bombMaterial);
}

void LevelResources::CreateBigAsteroidRenderable(Pht::IEngine& engine) {
    Pht::Material asteroidMaterial {"gray_asteroid.jpg", 0.84f, 1.23f, 0.0f, 1.0f};
    auto& sceneManager = engine.GetSceneManager();
    mBigAsteroid = sceneManager.CreateRenderableObject(Pht::ObjMesh {"asteroid_2000.obj", 38.0f, Pht::MoveMeshToOrigin::Yes},
                                                       asteroidMaterial);
}

void LevelResources::CreateSmallAsteroidRenderable(Pht::IEngine& engine) {
    Pht::Material asteroidMaterial {"gray_asteroid.jpg", 0.84f, 1.23f, 0.0f, 1.0f};
    asteroidMaterial.SetDepthTestAllowedOverride(true);
    auto& sceneManager = engine.GetSceneManager();
    mSmallAsteroid = sceneManager.CreateRenderableObject(Pht::ObjMesh {"asteroid_2000.obj", 19.0f, Pht::MoveMeshToOrigin::Yes},
                                                         asteroidMaterial);
}

void LevelResources::CreateAsteroidFragmentRenderable(Pht::IEngine& engine) {
    Pht::Material asteroidMaterial {"gray_asteroid.jpg", 0.84f, 1.23f, 0.0f, 1.0f};
    auto& sceneManager = engine.GetSceneManager();
    mAsteroidFragment = sceneManager.CreateRenderableObject(Pht::ObjMesh {"asteroid_fragment_500.obj", 5.25f, Pht::MoveMeshToOrigin::Yes},
                                                            asteroidMaterial);
}

Pht::RenderableObject& LevelResources::GetLevelBlockRenderable(BlockKind blockKind) const {
    switch (blockKind) {
        case BlockKind::LowerRightHalf:
        case BlockKind::UpperRightHalf:
        case BlockKind::UpperLeftHalf:
        case BlockKind::LowerLeftHalf:
            return *mGrayTriangle;
        case BlockKind::Full:
            return *mGrayCube;
        default:
            assert(!"Unsupported block kind");
            break;
    }
}

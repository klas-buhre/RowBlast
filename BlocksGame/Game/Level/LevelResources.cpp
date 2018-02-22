#include "LevelResources.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ObjMesh.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "OfflineRasterizer.hpp"
#include "IImage.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "GameScene.hpp"
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

using namespace BlocksGame;

LevelResources::LevelResources(Pht::IEngine& engine, const GameScene& scene) {
    CreatePieceTypes(engine, scene);
    CreateCellRenderables(engine.GetSceneManager(), scene);
    CreateBlueprintRenderables(engine, scene);
}

void LevelResources::CreatePieceTypes(Pht::IEngine& engine, const GameScene& scene) {
    mPieceTypes["LongI"] = std::make_unique<LongIPiece>(engine, scene);
    mPieceTypes["I"] = std::make_unique<MiddleIPiece>(engine, scene);
    mPieceTypes["ShortI"] = std::make_unique<ShortIPiece>(engine, scene);
    mPieceTypes["L"] = std::make_unique<LPiece>(engine, scene);
    mPieceTypes["B"] = std::make_unique<BPiece>(engine, scene);
    mPieceTypes["D"] = std::make_unique<DPiece>(engine, scene);
    mPieceTypes["Seven"] = std::make_unique<SevenPiece>(engine, scene);
    mPieceTypes["MirroredSeven"] = std::make_unique<MirroredSevenPiece>(engine, scene);
    mPieceTypes["F"] = std::make_unique<FPiece>(engine, scene);
    mPieceTypes["MirroredF"] = std::make_unique<MirroredFPiece>(engine, scene);
    mPieceTypes["BigL"] = std::make_unique<BigLPiece>(engine, scene);
    mPieceTypes["Z"] = std::make_unique<ZPiece>(engine, scene);
    mPieceTypes["MirroredZ"] = std::make_unique<MirroredZPiece>(engine, scene);
    mPieceTypes["T"] = std::make_unique<TPiece>(engine, scene);
    mPieceTypes["Plus"] = std::make_unique<PlusPiece>(engine, scene);
    mPieceTypes["SmallTriangle"] = std::make_unique<SmallTrianglePiece>(engine, scene);
    mPieceTypes["Triangle"] = std::make_unique<TrianglePiece>(engine, scene);
    mPieceTypes["BigTriangle"] = std::make_unique<BigTrianglePiece>(engine, scene);
    mPieceTypes["Diamond"] = std::make_unique<DiamondPiece>(engine, scene);
    mPieceTypes["Pyramid"] = std::make_unique<PyramidPiece>(engine, scene);
    mPieceTypes["Bomb"] = std::make_unique<Bomb>();
    mPieceTypes["RowBomb"] = std::make_unique<RowBomb>();
}

void LevelResources::CreateCellRenderables(Pht::ISceneManager& sceneManager,
                                           const GameScene& scene) {
    auto cellSize {scene.GetCellSize()};
    
    mGrayCube = sceneManager.CreateRenderableObject(
        Pht::ObjMesh {"cube_428.obj", cellSize},
        scene.GetGrayMaterial()
    );
    
    mGrayTriangle = sceneManager.CreateRenderableObject(
        Pht::ObjMesh {"triangle_320.obj", cellSize},
        scene.GetGrayMaterial()
    );
}

void LevelResources::CreateBlueprintRenderables(Pht::IEngine& engine, const GameScene& scene) {
    const auto edgeWidth {0.07f};
    auto cellSize {scene.GetCellSize()};
    auto squareSide {cellSize + edgeWidth};
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
    
    Pht::Vec4 fillColor {1.0f, 1.0f, 1.0f, 0.21f};
    
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {squareSide, squareSide};
    rasterizer->DrawRectangle(upperRight, lowerLeft, fillColor);
    
    Pht::Vec4 edgeColor {1.0f, 1.0f, 1.0f, 0.29f};
    
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

    auto image {rasterizer->ProduceImage()};
    
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    imageMaterial.GetDepthState().mDepthWrite = true;
    auto& sceneManager {engine.GetSceneManager()};
    mBlueprintSquare = sceneManager.CreateRenderableObject(Pht::QuadMesh {squareSide, squareSide},
                                                           imageMaterial);
    
    Pht::Material animationMaterial {Pht::Color {1.0f, 1.0f, 1.0f}};
    animationMaterial.SetOpacity(BlueprintSlotFillAnimation::mInitialOpacity);

    mBlueprintSquareAnimation = sceneManager.CreateRenderableObject(
        Pht::QuadMesh {cellSize, cellSize},
        animationMaterial
    );
}

Pht::RenderableObject& LevelResources::GetLevelBlockRenderable(BlockRenderableKind renderableKind) const {
    switch (renderableKind) {
        case BlockRenderableKind::LowerRightHalf:
        case BlockRenderableKind::UpperRightHalf:
        case BlockRenderableKind::UpperLeftHalf:
        case BlockRenderableKind::LowerLeftHalf:
            return *mGrayTriangle;
        case BlockRenderableKind::Full:
            return *mGrayCube;
        case BlockRenderableKind::None:
            assert(!"Not a mesh");
            break;
    }
}

const Piece& LevelResources::GetLPiece() const {
    auto i {mPieceTypes.find("L")};
    assert(i != std::end(mPieceTypes));
    return *(i->second);
}

#include "LevelStartDialogView.hpp"

// Engine includes.
#include "TextComponent.hpp"
#include "IEngine.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "PieceResources.hpp"
#include "UiLayer.hpp"
#include "GradientRectangle.hpp"

using namespace RowBlast;

namespace {
    constexpr auto cellSize {1.25f};
    constexpr auto previewPieceSpacing {2.0f};
    constexpr auto numPieceTypeRows {2};
}

LevelStartDialogView::LevelStartDialogView(Pht::IEngine& engine,
                                           const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::No};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    mCaption = &CreateText({-2.1f, 7.6f, UiLayer::text},
                           "LEVEL 1",
                           guiResources.GetCaptionTextProperties(zoom));

    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.5f,
        GetSize().y / 2.0f - 1.5f,
        UiLayer::textRectangle
    };

    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};

    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mRenderableObject = &guiResources.GetCloseButton(zoom);
    
    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);

    CreateText({-1.45f, 5.0f, UiLayer::text}, "PIECES", guiResources.GetLargeTextProperties());
    CreateText({-1.3f, -1.9f, UiLayer::text}, "GOAL", guiResources.GetLargeTextProperties());

    auto& textProperties {guiResources.GetSmallTextProperties(zoom)};
    mClearObjective = &CreateText({-3.6f, -3.1f, UiLayer::text},
                                  "Clear all gray blocks",
                                  textProperties);
    mClearObjective->GetSceneObject().SetIsVisible(false);
    mBuildObjective = &CreateText({-2.95f, -3.1f, UiLayer::text},
                                  "Fill all gray slots",
                                  textProperties);
    mBuildObjective->GetSceneObject().SetIsVisible(false);

    Pht::Vec2 playButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style playButtonStyle;
    playButtonStyle.mMeshFilename = GuiResources::mBigButtonMeshFilename;
    playButtonStyle.mColor = GuiResources::mBlueButtonColor;
    playButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    playButtonStyle.mPressedScale = 1.05f;
    playButtonStyle.mHasShadow = true;

    mPlayButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -5.5f, UiLayer::textRectangle},
                                               playButtonInputSize,
                                               playButtonStyle);
    mPlayButton->CreateText({-1.1f, -0.31f, UiLayer::buttonText},
                            "Play",
                            guiResources.GetLargeWhiteButtonTextProperties(zoom));
    
    CreatePreviewPiecesContainer(engine);
}

void LevelStartDialogView::CreatePreviewPiecesContainer(Pht::IEngine& engine) {
    auto container {std::make_unique<Pht::SceneObject>()};
    container->GetTransform().SetPosition({0.0f, 2.3f, UiLayer::block});
    
    for (auto& previewPiece: mPreviewPieces) {
        previewPiece.mBlockSceneObjects =
            std::make_unique<SceneObjectPool>(SceneObjectPoolKind::PreviewPieceBlocks, *container);
        previewPiece.mBlockSceneObjects->SetIsActive(false);
        
        auto& blocksTransform {
            previewPiece.mBlockSceneObjects->GetContainerSceneObject().GetTransform()
        };

        blocksTransform.SetRotation({-30.0f, -30.0f, 0.0f});
    }
    
    CreatePreviewPiecesRectangle(*container, engine);
    AddSceneObject(std::move(container));
}

void LevelStartDialogView::CreatePreviewPiecesRectangle(Pht::SceneObject& parentObject,
                                                        Pht::IEngine& engine) {
    Pht::Vec2 size {GetSize().x - 1.0f, 4.8f};
    auto leftQuadWidth {4.0f};
    auto rightQuadWidth {4.0f};
    
    GradientRectangleColors colors {
        .mLeft = {0.6f, 0.3f, 0.75f, 0.0f},
        .mMid = {0.6f, 0.3f, 0.75f, 0.8f},
        .mRight = {0.6f, 0.3f, 0.75f, 0.0f}
    };

    CreateGradientRectangle(engine.GetSceneManager(),
                            *this,
                            parentObject,
                            {0.0f, 0.0f, -0.9f},
                            size,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            colors,
                            colors);
}

void LevelStartDialogView::Init(const LevelInfo& levelInfo, const PieceResources& pieceResources) {
    mCaption->GetText() = "LEVEL " + std::to_string(levelInfo.mIndex);
    
    mClearObjective->GetSceneObject().SetIsVisible(false);
    mBuildObjective->GetSceneObject().SetIsVisible(false);
    
    switch (levelInfo.mObjective) {
        case Level::Objective::Clear:
            mClearObjective->GetSceneObject().SetIsVisible(true);
            break;
        case Level::Objective::Build:
            mBuildObjective->GetSceneObject().SetIsVisible(true);
            break;
    }
    
    for (auto& previewPiece: mPreviewPieces) {
        previewPiece.mBlockSceneObjects->SetIsActive(false);
    }
    
    auto numPieceTypes {levelInfo.mPieceTypes.size()};
    auto numPieceTypesUpperRow {
        static_cast<int>(std::ceil(static_cast<float>(numPieceTypes) /
                                   static_cast<float>(numPieceTypeRows)))
    };
    
    Pht::Vec3 previewPiecePosition {
        -previewPieceSpacing * numPieceTypesUpperRow / 2.0f + previewPieceSpacing / 2.0f,
        previewPieceSpacing / 2.0f,
        0.0f
    };
    
    for (auto i {0}; i < numPieceTypesUpperRow; ++i) {
        auto& previewPiece {mPreviewPieces[i]};
        auto* pieceType {levelInfo.mPieceTypes[i]};
        UpdatePreviewPiece(previewPiece, *pieceType, previewPiecePosition, pieceResources);
        previewPiecePosition.x += previewPieceSpacing;
    }
    
    auto numPieceTypesLowerRow {numPieceTypes - numPieceTypesUpperRow};

    previewPiecePosition = {
        -previewPieceSpacing * numPieceTypesLowerRow / 2.0f + previewPieceSpacing / 2.0f,
        -previewPieceSpacing / 2.0f,
        0.0f
    };

    for (auto i {numPieceTypesUpperRow}; i < numPieceTypes; ++i) {
        auto& previewPiece {mPreviewPieces[i]};
        auto* pieceType {levelInfo.mPieceTypes[i]};
        UpdatePreviewPiece(previewPiece, *pieceType, previewPiecePosition, pieceResources);
        previewPiecePosition.x += previewPieceSpacing;
    }
}

void LevelStartDialogView::UpdatePreviewPiece(LevelStartPreviewPiece& previewPiece,
                                              const Piece& pieceType,
                                              const Pht::Vec3& position,
                                              const PieceResources& pieceResources) {
    previewPiece.mBombSceneObject = nullptr;
    previewPiece.mRowBombSceneObject = nullptr;

    auto& containerObject {previewPiece.mBlockSceneObjects->GetContainerSceneObject()};
    auto& baseTransform {containerObject.GetTransform()};
    baseTransform.SetPosition(position);

    previewPiece.mBlockSceneObjects->SetIsActive(true);
    
    auto scale {pieceType.GetPreviewCellSize() / cellSize};
    
    baseTransform.SetScale(scale);
    previewPiece.mBlockSceneObjects->ReclaimAll();
    
    auto pieceNumRows {pieceType.GetGridNumRows()};
    auto pieceNumColumns {pieceType.GetGridNumColumns()};
    auto& grid {pieceType.GetGrid(Rotation::Deg0)};
    auto isBomb {pieceType.IsBomb()};
    auto isRowBomb {pieceType.IsRowBomb()};
    
    Pht::Vec3 lowerLeft {
        -static_cast<float>(pieceNumColumns) * cellSize / 2.0f + cellSize / 2.0f,
        -static_cast<float>(pieceNumRows) * cellSize / 2.0f + cellSize / 2.0f,
        0.0f
    };
    
    for (auto row {0}; row < pieceNumRows; row++) {
        for (auto column {0}; column < pieceNumColumns; column++) {
            auto& subCell {grid[row][column].mFirstSubCell};
            auto blockKind {subCell.mBlockKind};
 
            if (blockKind != BlockKind::None) {
                auto& blockSceneObject {previewPiece.mBlockSceneObjects->AccuireSceneObject()};
                
                if (isBomb) {
                    blockSceneObject.SetRenderable(&pieceResources.GetBombRenderableObject());
                    previewPiece.mBombSceneObject = &blockSceneObject;
                } else if (isRowBomb) {
                    blockSceneObject.SetRenderable(&pieceResources.GetRowBombRenderableObject());
                    previewPiece.mRowBombSceneObject = &blockSceneObject;
                } else {
                    auto& blockRenderable {
                        pieceResources.GetBlockRenderableObject(blockKind,
                                                                subCell.mColor,
                                                                BlockBrightness::Normal)
                    };
                    
                    blockSceneObject.SetRenderable(&blockRenderable);
                }
                
                Pht::Vec3 position {
                    static_cast<float>(column) * cellSize,
                    static_cast<float>(row) * cellSize,
                    0.0f
                };

                blockSceneObject.GetTransform().SetPosition(lowerLeft + position);
            }
        }
    }
}

#include "LevelLoader.hpp"

// Engine includes.
#include "JsonUtil.hpp"

// Game includes.
#include "LevelResources.hpp"

using namespace BlocksGame;

namespace {
    Level::Color ReadColor(const rapidjson::Document& document) {
        auto color {Pht::Json::ReadString(document, "color")};
        
        if (color == "pink") {
            return Level::Color::Pink;
        } else if (color == "green") {
            return Level::Color::Green;
        } else {
            assert(!"Unknown color");
        }
    }
    
    std::vector<const Piece*> ReadPieceTypes(const rapidjson::Document& document,
                                             const PieceTypes& pieceTypes) {
        assert(document.HasMember("pieces"));
        
        std::vector<const Piece*> levelPieceTypes;
        const auto& piecesArray {document["pieces"]};
        assert(piecesArray.IsArray());
        
        for (const auto& pieceStr: piecesArray.GetArray()) {
            assert(pieceStr.IsString());
            auto i {pieceTypes.find(pieceStr.GetString())};
            assert(i != std::end(pieceTypes));
            
            levelPieceTypes.push_back(i->second.get());
        }
        
        return levelPieceTypes;
    }
    
    Fill CellFill(char c) {
        switch (c) {
            case ' ':
                return Fill::Empty;
            case 'G':
            case 'O':
                return Fill::Full;
            case 'd':
            case 'D':
                return Fill::LowerRightHalf;
            case 'b':
            case 'B':
                return Fill::LowerLeftHalf;
            case 'p':
            case 'P':
                return Fill::UpperLeftHalf;
            case 'q':
            case 'Q':
                return Fill::UpperRightHalf;
            default:
                assert(!"Unknown cell type");
        }
    }
    
    Pht::RenderableObject* CellRenderable(char c, const LevelResources& levelResources) {
        switch (c) {
            case ' ':
                return nullptr;
            case 'G':
                return &levelResources.GetLevelBlockRenderable(BlockRenderableKind::Full);
            case 'd':
            case 'b':
            case 'p':
            case 'q':
                return &levelResources.GetLevelBlockRenderable(BlockRenderableKind::LowerLeftHalf);
            default:
                assert(!"Unknown cell type");
        }
    }
    
    Rotation CellRotation(char c) {
        switch (c) {
            case ' ':
            case 'G':
            case 'O':
            case 'd':
            case 'D':
                return Rotation::Deg0;
            case 'b':
            case 'B':
                return Rotation::Deg90;
            case 'p':
            case 'P':
                return Rotation::Deg180;
            case 'q':
            case 'Q':
                return Rotation::Deg270;
            default:
                assert(!"Unknown cell type");
        }
    }
    
    Cell CreateCell(char c, int column, int row, const LevelResources& levelResources) {
        Cell cell;
        
        cell.mFirstSubCell.mPosition = Pht::Vec2 {static_cast<float>(column), static_cast<float>(row)};
        cell.mFirstSubCell.mFill = CellFill(c);
        cell.mFirstSubCell.mBlockRenderableKind = ToBlockRenderableKind(cell.mFirstSubCell.mFill);
        cell.mFirstSubCell.mRenderableObject = CellRenderable(c, levelResources);
        cell.mFirstSubCell.mRotation = CellRotation(c);
        
        if (!cell.mFirstSubCell.IsEmpty()) {
            cell.mFirstSubCell.mIsLevel = true;
        }
        
        return cell;
    }
    
    std::unique_ptr<CellGrid> ReadClearGrid(const rapidjson::Document& document,
                                            const LevelResources& levelResources) {
        if (!document.HasMember("clearGrid")) {
            return nullptr;
        }
        
        const auto& rowArray {document["clearGrid"]};
        assert(rowArray.IsArray());
        
        auto numRows {rowArray.Size()};
        auto previousNumColumns {0};
        
        auto cellGrid {std::make_unique<CellGrid>()};
        cellGrid->resize(numRows);
        
        for (auto rowIndex {0}; rowIndex < numRows; ++rowIndex) {
            const auto& row {rowArray[numRows - 1 - rowIndex]};
            assert(row.IsString());
            
            std::string str {row.GetString()};
            auto numColumns {str.size()};
            assert(previousNumColumns == 0 || numColumns == previousNumColumns);
            
            previousNumColumns = static_cast<int>(numColumns);
            std::vector<Cell> cellRow(numColumns);
            
            for (auto columnIndex {0}; columnIndex < numColumns; ++columnIndex) {
                cellRow[columnIndex] = CreateCell(str[columnIndex],
                                                  columnIndex,
                                                  rowIndex,
                                                  levelResources);
            }
            
            (*cellGrid)[rowIndex] = cellRow;
        }
        
        return cellGrid;
    }
    
    std::unique_ptr<BlueprintCellGrid> ReadBlueprintGrid(const rapidjson::Document& document) {
        if (!document.HasMember("blueprintGrid")) {
            return nullptr;
        }
        
        const auto& rowArray {document["blueprintGrid"]};
        assert(rowArray.IsArray());
        
        auto numRows {rowArray.Size()};
        auto previousNumColumns {0};
        
        auto blueprintGrid {std::make_unique<BlueprintCellGrid>()};
        blueprintGrid->resize(numRows);
        
        for (auto rowIndex {0}; rowIndex < numRows; ++rowIndex) {
            const auto& row {rowArray[numRows - 1 - rowIndex]};
            assert(row.IsString());
            
            std::string str {row.GetString()};
            auto numColumns {str.size()};
            assert(previousNumColumns == 0 || numColumns == previousNumColumns);
            
            previousNumColumns = static_cast<int>(numColumns);
            std::vector<BlueprintCell> cellRow(numColumns);
            
            for (auto columnIndex {0}; columnIndex < numColumns; ++columnIndex) {
                cellRow[columnIndex] = BlueprintCell {.mFill = CellFill(str[columnIndex])};
            }
            
            (*blueprintGrid)[rowIndex] = cellRow;
        }
        
        return blueprintGrid;
    }
}

std::unique_ptr<Level> LevelLoader::Load(int levelIndex, const LevelResources& levelResources) {
    rapidjson::Document document;
    Pht::Json::ParseFile(document, "level" + std::to_string(levelIndex) + ".json");

    auto speed {Pht::Json::ReadFloat(document, "speed")};
    auto moves {Pht::Json::ReadInt(document, "moves")};
    
    StarLimits starLimits {
        .mTwo = Pht::Json::ReadInt(document, "twoStars"),
        .mThree = Pht::Json::ReadInt(document, "threeStars")
    };
    
    auto color {ReadColor(document)};
    auto levelPieces {ReadPieceTypes(document, levelResources.GetPieceTypes())};    
    auto clearGrid {ReadClearGrid(document, levelResources)};
    auto blueprintGrid {ReadBlueprintGrid(document)};
    
    assert(clearGrid || blueprintGrid);
    assert(!(clearGrid && blueprintGrid));

    Level::Objective objective;
    auto numColumns {0};
    auto numRows {0};
    
    if (clearGrid) {
        objective = Level::Objective::Clear;
        numColumns = static_cast<int>(clearGrid->front().size());
        numRows = static_cast<int>(clearGrid->size());
    } else {
        objective = Level::Objective::Build;
        numColumns = static_cast<int>(blueprintGrid->front().size());
        numRows = static_cast<int>(blueprintGrid->size());
    }
    
    auto level {
        std::make_unique<Level>(levelIndex,
                                objective,
                                numColumns,
                                numRows,
                                speed,
                                moves,
                                starLimits,
                                levelPieces,
                                color)
    };
    
    if (clearGrid) {
        level->SetClearGrid(std::move(clearGrid));
    } else {
        level->SetBlueprintGrid(std::move(blueprintGrid));
    }
    
    return level;
}

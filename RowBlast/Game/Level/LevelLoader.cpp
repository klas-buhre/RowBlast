#include "LevelLoader.hpp"

// Engine includes.
#include "JsonUtil.hpp"

// Game includes.
#include "LevelResources.hpp"

using namespace RowBlast;

namespace {
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
        
        assert(levelPieceTypes.size() <= Level::maxNumPieceTypes);
        return levelPieceTypes;
    }
    
    Fill CellFill(char c) {
        switch (c) {
            case ' ':
                return Fill::Empty;
            case 'G':
            case 'B':
            case 'L':
            case 'r':
            case 'g':
            case 'l':
            case 'y':
                return Fill::Full;
            case 'd':
                return Fill::LowerRightHalf;
            case 'b':
                return Fill::LowerLeftHalf;
            case 'p':
                return Fill::UpperLeftHalf;
            case 'q':
                return Fill::UpperRightHalf;
            default:
                assert(!"Unknown cell type");
        }
    }

    BlockKind CellBlockKind(char c) {
        switch (c) {
            case ' ':
                return BlockKind::None;
            case 'G':
            case 'r':
            case 'g':
            case 'l':
            case 'y':
                return BlockKind::Full;
            case 'B':
                return BlockKind::Bomb;
            case 'L':
                return BlockKind::RowBomb;
            case 'd':
                return BlockKind::LowerRightHalf;
            case 'b':
                return BlockKind::LowerLeftHalf;
            case 'p':
                return BlockKind::UpperLeftHalf;
            case 'q':
                return BlockKind::UpperRightHalf;
            default:
                assert(!"Unknown cell type");
        }
    }
    
    BlockColor CellBlockColor(char c) {
        switch (c) {
            case 'r':
                return BlockColor::Red;
            case 'g':
                return BlockColor::Green;
            case 'l':
                return BlockColor::Blue;
            case 'y':
                return BlockColor::Yellow;
            default:
                return BlockColor::None;
        }
    }

    Rotation CellRotation(char c) {
        switch (c) {
            case ' ':
            case 'G':
            case 'd':
            case 'B':
            case 'L':
            case 'r':
            case 'g':
            case 'l':
            case 'y':
                return Rotation::Deg0;
            case 'b':
                return Rotation::Deg90;
            case 'p':
                return Rotation::Deg180;
            case 'q':
                return Rotation::Deg270;
            default:
                assert(!"Unknown cell type");
        }
    }
    
    bool IsGrayLevelBlock(char c) {
        switch (c) {
            case 'G':
            case 'd':
            case 'b':
            case 'p':
            case 'q':
                return true;
            default:
                return false;
        }
    }
    
    Cell CreateCell(char c, int column, int row) {
        Cell cell;
        auto& firstSubCell {cell.mFirstSubCell};
        
        firstSubCell.mPosition = Pht::Vec2 {static_cast<float>(column), static_cast<float>(row)};
        firstSubCell.mFill = CellFill(c);
        firstSubCell.mBlockKind = CellBlockKind(c);
        firstSubCell.mColor = CellBlockColor(c);
        firstSubCell.mRotation = CellRotation(c);
        firstSubCell.mIsGrayLevelBlock = IsGrayLevelBlock(c);

        return cell;
    }
    
    std::unique_ptr<CellGrid> ReadClearGrid(const rapidjson::Document& document) {
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
                cellRow[columnIndex] = CreateCell(str[columnIndex], columnIndex, rowIndex);
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

std::unique_ptr<Level> LevelLoader::Load(int levelId, const LevelResources& levelResources) {
    rapidjson::Document document;
    Pht::Json::ParseFile(document, "level" + std::to_string(levelId) + ".json");

    auto speed {Pht::Json::ReadFloat(document, "speed")};
    auto moves {Pht::Json::ReadInt(document, "moves")};
    
    StarLimits starLimits {
        .mTwo = Pht::Json::ReadInt(document, "twoStars"),
        .mThree = Pht::Json::ReadInt(document, "threeStars")
    };
    
    auto backgroundTextureFilename {Pht::Json::ReadString(document, "background")};
    auto levelPieces {ReadPieceTypes(document, levelResources.GetPieceTypes())};    
    auto clearGrid {ReadClearGrid(document)};
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
        std::make_unique<Level>(levelId,
                                objective,
                                numColumns,
                                numRows,
                                speed,
                                moves,
                                starLimits,
                                levelPieces,
                                backgroundTextureFilename)
    };
    
    if (clearGrid) {
        level->SetClearGrid(std::move(clearGrid));
    } else {
        level->SetBlueprintGrid(std::move(blueprintGrid));
    }
    
    return level;
}

std::unique_ptr<LevelInfo> LevelLoader::LoadInfo(int levelId,
                                                 const LevelResources& levelResources) {
    rapidjson::Document document;
    Pht::Json::ParseFile(document, "level" + std::to_string(levelId) + ".json");

    auto levelPieces {ReadPieceTypes(document, levelResources.GetPieceTypes())};
    Level::Objective objective;
    
    if (document.HasMember("clearGrid")) {
        objective = Level::Objective::Clear;
    } else  if (document.HasMember("blueprintGrid")) {
        objective = Level::Objective::Build;
    } else {
        assert(!"Unknown objective");
    }
    
    return std::make_unique<LevelInfo>(levelId, objective, levelPieces);
}

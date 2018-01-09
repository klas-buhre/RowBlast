#include "ClickInputHandler.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "InputEvent.hpp"

// Game includes.
#include "FallingPiece.hpp"
#include "Field.hpp"
#include "GameScene.hpp"
#include "IGameLogic.hpp"

using namespace BlocksGame;

namespace {
    Pht::Mat4 identityMatrix;
    const float inputUnitsPerColumn {26.5f * 1.25f}; // {26.5f * 10.0f / Field::numColumns};
    
    bool CompareMoves(const Move* a, const Move* b) {
        return a->mScore > b->mScore;
    }
}

MoveButton::MoveButton(Pht::IEngine& engine) :
    mButton {mSceneObject, Pht::Vec2{0.0f, 0.0f}, engine} {}

void MoveButton::SetPosition(const Pht::Vec3& position) {
    mSceneObject.SetPosition(position);
}

void MoveButton::SetSize(const Pht::Vec2& size) {
    mButton.SetSize(size);
}

ClickInputHandler::ClickInputHandler(Pht::IEngine& engine,
                                     Field& field,
                                     const GameScene& gameScene,
                                     IGameLogic& gameLogic) :
    mEngine {engine},
    mField {field},
    mGameScene {gameScene},
    mGameLogic {gameLogic},
    mValidMovesSearch {field},
    mSwipeUpRecognizer {Pht::SwipeDirection::Up, inputUnitsPerColumn} {

    for (auto i {0}; i < maxNumVisibleMoves; ++i) {
        mMoveButtons.push_back(std::make_unique<MoveButton>(engine));
    }
}

void ClickInputHandler::Reset(const Level& level) {
    mLevel = &level;
    mState = State::Inactive;
    
    mValidMovesSearch.Reset();
    
    mNumClickGridRows = mField.GetNumRows() * 2 + 2;
    mNumClickGridColumns = mField.GetNumColumns() * 2 + 2;
    
    mClickGrid.clear();
    mClickGrid.reserve(mNumClickGridRows);
        
    std::vector<int> row(mNumClickGridColumns);
        
    for (auto rowIndex {0}; rowIndex < mNumClickGridRows; ++rowIndex) {
        mClickGrid.push_back(row);
    }
}

void ClickInputHandler::CalculateMoves(const FallingPiece& fallingPiece) {
    mValidMoves.Clear();
    mPieceType = &fallingPiece.GetPieceType();
    
    MovingPiece piece {
        fallingPiece.GetIntPosition(),
        fallingPiece.GetRotation(),
        fallingPiece.GetPieceType()
    };
    
    mValidMovesSearch.FindValidMoves(mValidMoves, piece);
    EvaluateMoves(fallingPiece.GetId());
    SortMoves();
}

void ClickInputHandler::EvaluateMoves(int pieceId) {
    auto pieceNumRows {mPieceType->GetGridNumRows()};
    auto pieceNumcolumns {mPieceType->GetGridNumColumns()};
    auto& moves {mValidMoves.mMoves};
    auto numMoves {moves.Size()};
    
    for (auto i {0}; i < numMoves; ++i) {
        auto& move {moves.At(i)};
        PieceBlocks pieceBlocks {mPieceType->GetGrid(move.mRotation), pieceNumRows, pieceNumcolumns};
        
        mField.LandPieceBlocks(pieceBlocks, pieceId, move.mPosition, false, false);
        EvaluateMove(move, pieceId);
        mField.RemovePiece(pieceId, move.mPosition, pieceNumRows, pieceNumcolumns);
    }
}

void ClickInputHandler::EvaluateMove(Move& move, int pieceId) {
    switch (mLevel->GetObjective()) {
        case Level::Objective::Clear:
            EvaluateMoveForClearObjective(move, pieceId);
            break;
        case Level::Objective::Build:
            EvaluateMoveForBuildObjective(move);
            break;
    }
}

void ClickInputHandler::EvaluateMoveForClearObjective(Move& move, int pieceId) {
    auto landingHeight {
        static_cast<float>(move.mPosition.y - mField.GetLowestVisibleRow()) +
        mPieceType->GetCenterPosition(move.mRotation).y
    };

    auto filledRowsMetric {0.0f};
    auto filledRowsResult {mField.MarkFilledRowsAndCountPieceCellsInFilledRows(pieceId)};
    auto numFilledRows {filledRowsResult.mFilledRowIndices.Size()};
    
    if (numFilledRows > 0) {
        filledRowsMetric = static_cast<float>(numFilledRows) * filledRowsResult.mPieceCellsInFilledRows;
    }
    
    auto burriedHolesArea {mField.GetBurriedHolesAreaInVisibleRows()};
    auto wellsArea {mField.GetWellsAreaInVisibleRows()};
    auto numTransitions {static_cast<float>(mField.GetNumTransitionsInVisibleRows())};
    
    move.mScore = -landingHeight
                  + filledRowsMetric
                  - 4.0f * burriedHolesArea
                  - wellsArea
                  - numTransitions;
    
    mField.UnmarkFilledRows(filledRowsResult.mFilledRowIndices);
}

void ClickInputHandler::EvaluateMoveForBuildObjective(Move& move) {
    auto landingHeight {
        static_cast<float>(move.mPosition.y - mField.GetLowestVisibleRow()) +
        mPieceType->GetCenterPosition(move.mRotation).y
    };
    
    auto numCellsAccordingToBlueprint {
        static_cast<float>(mField.GetNumCellsAccordingToBlueprintInVisibleRows())
    };

    auto buildHolesArea {mField.GetBuildHolesAreaInVisibleRows()};
    auto buildWellsArea {mField.GetBuildWellsAreaInVisibleRows()};
    
    move.mScore = -landingHeight
                  + 2.0f * numCellsAccordingToBlueprint
                  - 4.0f * buildHolesArea
                  - 0.25f * buildWellsArea;
}

void ClickInputHandler::SortMoves() {
    mSortedMoves.Clear();
    
    auto& moves {mValidMoves.mMoves};
    auto numMoves {moves.Size()};
    
    for (auto i {0}; i < numMoves; ++i) {
        mSortedMoves.PushBack(&moves.At(i));
    }

    mSortedMoves.Sort(CompareMoves);
}

void ClickInputHandler::UpdateMoves(const FallingPiece& fallingPiece) {
    mUpdatedValidMoves.Clear();
    
    MovingPiece piece {
        fallingPiece.GetIntPosition(),
        fallingPiece.GetRotation(),
        fallingPiece.GetPieceType()
    };
    
    mValidMovesSearch.FindValidMoves(mUpdatedValidMoves, piece);
    
    auto& previousMoves {mValidMoves.mMoves};
    
    for (auto i {0}; i < previousMoves.Size(); ++i) {
        auto& move {previousMoves.At(i)};
        
        if (mUpdatedValidMoves.mMoves.Find(move) == nullptr) {
            move.mIsReachable = false;
        }
    }
    
    for (auto i {0}; i < mMoveAlternativeSet.Size();) {
        if (mUpdatedValidMoves.mMoves.Find(mMoveAlternativeSet.At(i)) == nullptr) {
            mMoveAlternativeSet.Erase(i);
        } else {
            ++i;
        }
    }
}

void ClickInputHandler::CreateNewMoveAlternativeSet() {
    mMoveAlternativeSet.Clear();
    ClearClickGrid();
    PopulateMoveAlternativeSet();
    
    if (mMoveAlternativeSet.IsEmpty()) {
        auto& moves {mValidMoves.mMoves};
        
        for (auto i {0}; i < moves.Size(); ++i) {
            moves.At(i).mHasBeenPresented = false;
        }
        
        PopulateMoveAlternativeSet();
    }
    
    mState = State::PresentingMoves;
}

void ClickInputHandler::ClearClickGrid() {
    for (auto row {0}; row < mNumClickGridRows; ++row) {
        for (auto column {0}; column < mNumClickGridColumns; ++column) {
            mClickGrid[row][column] = 0;
        }
    }
}

void ClickInputHandler::PopulateMoveAlternativeSet() {
    for (auto i {0}; i < mSortedMoves.Size(); ++i) {
        auto* move {mSortedMoves.At(i)};
        
        if (!move->mHasBeenPresented && move->mIsReachable && IsRoomForMove(*move) &&
            mMoveAlternativeSet.Size() < maxNumVisibleMoves) {
            
            move->mHasBeenPresented = true;
            
            SetupButton(*mMoveButtons[mMoveAlternativeSet.Size()], *move);
            mMoveAlternativeSet.PushBack(*move);
            InsertMoveInClickGrid(*move);
        }
    }
}

bool ClickInputHandler::IsRoomForMove(const Move& move) const {
    auto pieceNumRows {mPieceType->GetClickGridNumRows()};
    auto pieceNumColumns {mPieceType->GetClickGridNumColumns()};
    const auto& pieceClickGrid {mPieceType->GetClickGrid(move.mRotation)};
    auto position {move.mPosition * 2};
    
    for (auto pieceRow {0}; pieceRow < pieceNumRows; ++pieceRow) {
        for (auto pieceColumn {0}; pieceColumn < pieceNumColumns; ++pieceColumn) {
            auto cell {pieceClickGrid[pieceRow][pieceColumn]};
            
            if (cell == 0) {
                continue;
            }
        
            auto row {position.y + pieceRow + 1};
            auto column {position.x + pieceColumn + 1};
            
            if (mClickGrid[row][column] != 0) {
                return false;
            }
        }
    }
    
    return true;
}

void ClickInputHandler::InsertMoveInClickGrid(const Move& move) {
    auto pieceNumRows {mPieceType->GetClickGridNumRows()};
    auto pieceNumColumns {mPieceType->GetClickGridNumColumns()};
    const auto& pieceClickGrid {mPieceType->GetClickGrid(move.mRotation)};
    auto position {move.mPosition * 2};
    
    for (auto pieceRow {0}; pieceRow < pieceNumRows; ++pieceRow) {
        for (auto pieceColumn {0}; pieceColumn < pieceNumColumns; ++pieceColumn) {
            auto cell {pieceClickGrid[pieceRow][pieceColumn]};
            
            if (cell == 0) {
                continue;
            }
        
            auto row {position.y + pieceRow + 1};
            auto column {position.x + pieceColumn + 1};
            
            mClickGrid[row][column] = 1;
        }
    }
}

void ClickInputHandler::SetupButton(MoveButton& moveButton, Move& move) {
    move.mButton = &moveButton;
    
    auto cellSize {mGameScene.GetCellSize()};
    auto buttonSizeSceneCoords {mPieceType->GetButtonSize(move.mRotation) * cellSize / 2.0f};
    
    auto inputScaleFactor {
        mEngine.GetInput().GetScreenInputSize().y /
        mEngine.GetRenderer().GetOrthographicFrustumSize().y
    };
    
    moveButton.SetSize(buttonSizeSceneCoords * inputScaleFactor);
    
    auto buttonCenterLocalCoords {
        mPieceType->GetButtonCenterPosition(move.mRotation) * cellSize / 2.0f
    };
    
    Pht::Vec2 piecePosition {
        static_cast<float>(move.mPosition.x) * cellSize,
        static_cast<float>(move.mPosition.y) * cellSize,
    };
    
    const auto& fieldLowerLeft {mGameScene.GetFieldLoweLeft()};
    auto buttonPosition2d {fieldLowerLeft + piecePosition + buttonCenterLocalCoords};
    
    Pht::Vec3 buttonPosition3d {buttonPosition2d.x, buttonPosition2d.y, 0.0f};
    moveButton.SetPosition(buttonPosition3d);
}

const ClickInputHandler::MoveAlternativeSet* ClickInputHandler::GetMoveAlternativeSet() const {
    switch (mState) {
        case State::PresentingMoves:
            return &mMoveAlternativeSet;
        case State::Inactive:
            return nullptr;
    }
}

void ClickInputHandler::HandleTouch(const Pht::TouchEvent& touchEvent) {
    mEngine.GetRenderer().SetProjectionMode(Pht::ProjectionMode::Orthographic);
    
    for (auto i {0}; i < mMoveAlternativeSet.Size(); ++i) {
        auto& move {mMoveAlternativeSet.At(i)};
        
        switch (move.mButton->GetButton().OnTouch(touchEvent, identityMatrix)) {
            case Pht::Button::Result::Down:
            case Pht::Button::Result::MoveInside:
                if (mPieceType->IsBomb()) {
                    mGameLogic.StartBlastRadiusAnimation(move.mPosition);
                }
                return;
            case Pht::Button::Result::UpOutside:
            case Pht::Button::Result::MoveOutside:
                if (mPieceType->IsBomb()) {
                    mGameLogic.StopBlastRadiusAnimation();
                }
                return;
            case Pht::Button::Result::UpInside:
                mGameLogic.StopBlastRadiusAnimation();
                mGameLogic.StartFallingPieceAnimation(*move.mLastMovement);
                mState = State::Inactive;
                return;
            case Pht::Button::Result::None:
                break;
        }
    }
    
    switch (touchEvent.mState) {
        case Pht::TouchState::Begin:
            HandleTouchBegin();
            return;
        case Pht::TouchState::End:
            if (mTouchContainsSwipeUp) {
                mGameLogic.SwitchPiece();
            } else {
                CreateNewMoveAlternativeSet();
            }
            return;
        case Pht::TouchState::Other:
            return;
        case Pht::TouchState::Ongoing:
            break;
    }
    
    RecognizeSwipe(touchEvent);
}

void ClickInputHandler::HandleTouchBegin() {
    mSwipeUpRecognizer.TouchBegin();
    mTouchContainsSwipeUp = false;
}

void ClickInputHandler::RecognizeSwipe(const Pht::TouchEvent& touchEvent) {
    switch (mSwipeUpRecognizer.IsTouchContainingSwipe(touchEvent)) {
        case Pht::IsSwipe::Yes:
            mTouchContainsSwipeUp = true;
            break;
        case Pht::IsSwipe::Maybe:
        case Pht::IsSwipe::No:
            break;
    }
}

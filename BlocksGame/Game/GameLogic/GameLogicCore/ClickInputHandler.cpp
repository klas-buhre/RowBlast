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

using namespace RowBlast;

namespace {
    const float inputUnitsPerColumn {26.5f * 1.25f}; // {26.5f * 10.0f / Field::numColumns};
}

MoveButton::MoveButton(Pht::IEngine& engine) :
    mButton {mSceneObject, Pht::Vec2{0.0f, 0.0f}, engine} {}

void MoveButton::SetPosition(const Pht::Vec3& position) {
    mSceneObject.GetTransform().SetPosition(position);
    mSceneObject.Update(false);
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
    mAi {field},
    mSwipeUpRecognizer {Pht::SwipeDirection::Up, inputUnitsPerColumn} {

    for (auto i {0}; i < maxNumVisibleMoves; ++i) {
        mMoveButtons.push_back(std::make_unique<MoveButton>(engine));
    }
}

void ClickInputHandler::Init(const Level& level) {
    mState = State::Inactive;
    
    mAi.Init(level);
    
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
    mPieceType = &fallingPiece.GetPieceType();
    mAllValidMoves = &mAi.CalculateMoves(fallingPiece);
}

void ClickInputHandler::UpdateMoves(const FallingPiece& fallingPiece) {
    assert(mAllValidMoves);
    
    auto& updatedMoves {mAi.FindValidMoves(fallingPiece)};
    auto& previousMoves {*mAllValidMoves};
    
    for (auto* move: previousMoves) {
        if (updatedMoves.mMoves.Find(*move) == nullptr) {
            move->mIsReachable = false;
        }
    }
    
    for (auto i {0}; i < mVisibleMoves.Size();) {
        if (updatedMoves.mMoves.Find(mVisibleMoves.At(i)) == nullptr) {
            mVisibleMoves.Erase(i);
        } else {
            ++i;
        }
    }
}

void ClickInputHandler::CreateNewSetOfVisibleMoves() {
    assert(mAllValidMoves);

    mVisibleMoves.Clear();
    ClearClickGrid();
    PopulateSetOfVisibleMoves();
    
    if (mVisibleMoves.IsEmpty()) {
        for (auto* move: *mAllValidMoves) {
            move->mHasBeenPresented = false;
        }
        
        PopulateSetOfVisibleMoves();
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

void ClickInputHandler::PopulateSetOfVisibleMoves() {
    assert(mAllValidMoves);

    for (auto i {0}; i < mAllValidMoves->Size(); ++i) {
        auto* move {mAllValidMoves->At(i)};
        
        if (!move->mHasBeenPresented && move->mIsReachable && IsRoomForMove(*move) &&
            mVisibleMoves.Size() < maxNumVisibleMoves) {
            
            move->mHasBeenPresented = true;
            
            SetupButton(*mMoveButtons[mVisibleMoves.Size()], *move);
            mVisibleMoves.PushBack(*move);
            InsertMoveInClickGrid(*move);
        }
    }
}

bool ClickInputHandler::IsRoomForMove(const Move& move) const {
    assert(mPieceType);

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
    assert(mPieceType);

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
    assert(mPieceType);
    
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

const ClickInputHandler::VisibleMoves* ClickInputHandler::GetVisibleMoves() const {
    switch (mState) {
        case State::PresentingMoves:
            return &mVisibleMoves;
        case State::Inactive:
            return nullptr;
    }
}

void ClickInputHandler::HandleTouch(const Pht::TouchEvent& touchEvent) {
    assert(mPieceType);
    
    if (mState == State::Inactive) {
        return;
    }
    
    mEngine.GetRenderer().SetProjectionMode(Pht::ProjectionMode::Orthographic);
    
    for (auto i {0}; i < mVisibleMoves.Size(); ++i) {
        auto& move {mVisibleMoves.At(i)};
        
        switch (move.mButton->GetButton().OnTouch(touchEvent)) {
            case Pht::Button::Result::Down:
                if (mPieceType->IsBomb()) {
                    mGameLogic.StartBlastRadiusAnimation(move.mPosition);
                }
                return;
            case Pht::Button::Result::MoveInside:
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
                CreateNewSetOfVisibleMoves();
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

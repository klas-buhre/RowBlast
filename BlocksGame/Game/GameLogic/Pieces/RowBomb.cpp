#include "RowBomb.hpp"

using namespace BlocksGame;

RowBomb::RowBomb() {
    FillGrid fillGrid = {
        {Fill::Empty, Fill::Empty, Fill::Empty},
        {Fill::Empty, Fill::Full,  Fill::Empty},
        {Fill::Empty, Fill::Empty, Fill::Empty}
    };
    
    ClickGrid clickGrid = {
        {0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0}
    };

    InitGrids(fillGrid, clickGrid, BlockColor::None);
    SetPreviewCellSize(0.8f);
    SetNumRotations(1);
}

bool RowBomb::IsRowBomb() const {
    return true;
}

bool RowBomb::CanRotateAroundZ() const {
    return false;
}

int RowBomb::GetNumEmptyTopRows() const {
    return 1;
}
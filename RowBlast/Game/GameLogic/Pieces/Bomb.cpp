#include "Bomb.hpp"

using namespace RowBlast;

Bomb::Bomb() {
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
    SetPreviewCellSize(1.25f);
    SetNumRotations(1);
}

bool Bomb::IsBomb() const {
    return true;
}

bool Bomb::CanRotateAroundZ() const {
    return false;
}

int Bomb::GetNumEmptyTopRows() const {
    return 1;
}

#ifndef Tutorial_hpp
#define Tutorial_hpp

// Game includes.
#include "GuiViewManager.hpp"
#include "PlacePieceWindowController.hpp"
#include "FillRowsWindowController.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    class Level;
    class Piece;
    
    class Tutorial {
    public:
        Tutorial(Pht::IEngine& engine, GameScene& scene, const CommonResources& commonResources);
        
        void Init(const Level& level);
        void Update();
        void OnNextMove(int numMovesUsedIncludingCurrent);
        void OnSwitchPiece(const Piece& pieceType);
        bool IsSwitchPieceAllowed(int numMovesUsedIncludingCurrent) const;
        bool IsPlacePieceAllowed(const Piece& pieceType) const;
        bool IsPauseAllowed() const;

    private:
        enum class Controller {
            PlacePieceWindow,
            FillRowsWindow,
            None
        };

        void OnNextMoveFirstLevel(int numMovesUsedIncludingCurrent);
        void SetActiveController(Controller controller);
        
        GameScene& mScene;
        const Level* mLevel {nullptr};
        Controller mActiveController {Controller::None};
        GuiViewManager mViewManager;
        PlacePieceWindowController mPlacePieceWindowController;
        FillRowsWindowController mFillRowsWindowController;
    };
}

#endif

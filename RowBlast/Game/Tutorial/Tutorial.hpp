#ifndef Tutorial_hpp
#define Tutorial_hpp

// Game includes.
#include "GuiViewManager.hpp"
#include "PlacePieceWindowController.hpp"

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
        bool IsSwitchPieceAllowed() const;
        bool IsPlacePieceAllowed(const Piece& pieceType) const;

    private:
        enum class Controller {
            PlacePieceWindow,
            None
        };

        void OnNextMoveFirstLevel(int numMovesUsedIncludingCurrent);
        void SetActiveController(Controller controller);
        
        GameScene& mScene;
        const Level* mLevel {nullptr};
        Controller mActiveController {Controller::None};
        GuiViewManager mViewManager;
        PlacePieceWindowController mPlacePieceWindowController;
    };
}

#endif

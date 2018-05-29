#ifndef LevelStartDialogView_hpp
#define LevelStartDialogView_hpp

#include <memory>
#include <array>

// Engine includes.
#include "GuiView.hpp"
#include "SceneObject.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "SceneObjectPool.hpp"
#include "Level.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    class LevelInfo;
    class PieceResources;
    
    class LevelStartDialogView: public Pht::GuiView {
    public:
        LevelStartDialogView(Pht::IEngine& engine, const CommonResources& commonResources);
        void Init(const LevelInfo& levelInfo, const PieceResources& pieceResources);
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetPlayButton() const {
            return *mPlayButton;
        }
        
    private:
        struct LevelStartPreviewPiece {
            std::unique_ptr<SceneObjectPool> mBlockSceneObjects;
            Pht::SceneObject* mBombSceneObject {nullptr};
            Pht::SceneObject* mRowBombSceneObject {nullptr};
        };

        void CreatePreviewPiecesContainer(Pht::IEngine& engine);
        void CreatePreviewPiecesRectangle(Pht::SceneObject& parentObject, Pht::IEngine& engine);
        void UpdatePreviewPiece(LevelStartPreviewPiece& previewPiece,
                                const Piece& pieceType,
                                const Pht::Vec3& position,
                                const PieceResources& pieceResources);

        using LevelStartPreviewPieces = std::array<LevelStartPreviewPiece, Level::maxNumPieceTypes>;

        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mPlayButton;
        Pht::TextComponent* mCaption {nullptr};
        Pht::TextComponent* mClearObjective {nullptr};
        Pht::TextComponent* mBuildObjective {nullptr};
        LevelStartPreviewPieces mPreviewPieces;
    };
}

#endif

#ifndef LevelGoalDialogView_hpp
#define LevelGoalDialogView_hpp

#include <memory>
#include <array>

// Engine includes.
#include "GuiView.hpp"
#include "SceneObject.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "SceneObjectPool.hpp"
#include "Level.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
    class ParticleEffect;
}

namespace RowBlast {
    class CommonResources;
    class LevelInfo;
    class PieceResources;
    
    class LevelGoalDialogView: public Pht::GuiView {
    public:
        enum class Scene {
            Map,
            Game
        };

        LevelGoalDialogView(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            const PieceResources& pieceResources,
                            Scene scene);
        
        void Init(const LevelInfo& levelInfo);
        void StartEffects();
        void Update();
        
        MenuButton& GetCloseButton() {
            assert(mCloseButton);
            return *mCloseButton;
        }
        
        MenuButton& GetPlayButton() {
            assert(mPlayButton);
            return *mPlayButton;
        }

        MenuButton& GetBackButton() {
            assert(mBackButton);
            return *mBackButton;
        }

    private:
        struct LevelStartPreviewPiece {
            std::unique_ptr<SceneObjectPool> mBlockSceneObjects;
            Pht::SceneObject* mBombSceneObject {nullptr};
            Pht::SceneObject* mRowBombSceneObject {nullptr};
        };

        void CreatePreviewPiecesContainer(Pht::IEngine& engine);
        void CreateGlowEffects(Pht::SceneObject& parentObject, Pht::IEngine& engine);
        void InitPreviewPiece(LevelStartPreviewPiece& previewPiece,
                              const Piece& pieceType,
                              const Pht::Vec3& position);
        void UpdateAnimations(float dt);
        void AnimateEmissive(float dt);
        void AnimateBombRotation(float dt);
        void AnimateRowBombRotation(float dt);

        using LevelStartPreviewPieces = std::array<LevelStartPreviewPiece, Level::maxNumPieceTypes>;

        Pht::IEngine& mEngine;
        const PieceResources& mPieceResources;
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mPlayButton;
        std::unique_ptr<MenuButton> mBackButton;
        Pht::TextComponent* mCaption {nullptr};
        Pht::TextComponent* mClearObjective {nullptr};
        Pht::TextComponent* mBuildObjective {nullptr};
        LevelStartPreviewPieces mPreviewPieces;
        std::unique_ptr<Pht::SceneObject> mGlowEffect;
        std::unique_ptr<Pht::SceneObject> mRoundGlowEffect;
        float mAnimationTime {0.0f};
        float mEmissiveAnimationTime {0.0f};
        Pht::Vec3 mRowBombRotation;
    };
}

#endif

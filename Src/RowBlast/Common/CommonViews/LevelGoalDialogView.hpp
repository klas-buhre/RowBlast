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
    class IGuiLightProvider;
    
    class LevelGoalDialogView: public Pht::GuiView {
    public:
        enum class SceneId {
            Map,
            Game
        };

        LevelGoalDialogView(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            const PieceResources& pieceResources,
                            SceneId sceneId);
        
        void OnDeactivate() override;
        
        void SetUp(const LevelInfo& levelInfo);
        void StartEffects();
        void Update();
        
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
            mGuiLightProvider = &guiLightProvider;
        }
        
        MenuButton& GetCloseButton() {
            assert(mCloseButton);
            return *mCloseButton;
        }
        
        MenuButton& GetPlayButton() {
            assert(mPlayButton);
            return *mPlayButton;
        }

        MenuButton& GetOkButton() {
            assert(mOkButton);
            return *mOkButton;
        }

    private:
        struct LevelStartPreviewPiece {
            Pht::SceneObject* mSceneObject {nullptr};
            std::unique_ptr<SceneObjectPool> mBlockSceneObjects;
            Pht::SceneObject* mBombSceneObject {nullptr};
            Pht::SceneObject* mRowBombSceneObject {nullptr};
        };

        void CreatePreviewPiecesContainer();
        void CreateGlowEffectsBehindPieces(Pht::SceneObject& parentObject);
        void CreateGoalContainer(const CommonResources& commonResources,
                                 PotentiallyZoomedScreen zoom);
        void CreateGrayCube(const CommonResources& commonResources,
                            Pht::SceneObject& parent);
        void CreateAsteroid(Pht::SceneObject& parent);
        void CreateBlueprintSlot(Pht::SceneObject& parent);
        void CreateGlowEffectsBehindGoal(Pht::SceneObject& parentObject);
        void SetUpPreviewPiece(LevelStartPreviewPiece& previewPiece,
                               const Piece& pieceType,
                               const Pht::Vec3& position);
        void SetUpBlockWelds(const SubCell& subCell,
                             const Pht::Vec3& blockPos,
                             SceneObjectPool& pool);
        void SetUpBlockWeld(const Pht::Vec3& weldPosition,
                            float rotation,
                            const SubCell& subCell,
                            SceneObjectPool& pool);
        void UpdateAnimations(float dt);
        void AnimateEmissive(float dt);
        void AnimateBombRotation();
        void AnimateRowBombRotation(float dt);
        void AnimateGrayCubeRotation(float dt);
        void AnimateAsteroidRotation(float dt);

        using LevelStartPreviewPieces = std::array<LevelStartPreviewPiece, Level::maxNumPieceTypes>;

        Pht::IEngine& mEngine;
        const PieceResources& mPieceResources;
        IGuiLightProvider* mGuiLightProvider {nullptr};
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mPlayButton;
        std::unique_ptr<MenuButton> mOkButton;
        Pht::TextComponent* mCaption {nullptr};
        Pht::SceneObject* mClearObjectiveSceneObject {nullptr};
        Pht::SceneObject* mAsteroidObjectiveSceneObject {nullptr};
        Pht::SceneObject* mBuildObjectiveSceneObject {nullptr};
        LevelStartPreviewPieces mPreviewPieces;
        std::unique_ptr<Pht::SceneObject> mGlowEffect;
        std::unique_ptr<Pht::SceneObject> mRoundGlowEffect;
        std::unique_ptr<Pht::SceneObject> mGoalRoundGlowEffect;
        Pht::SceneObject* mGrayCubeSceneObject {nullptr};
        Pht::SceneObject* mAsteroidSceneObject {nullptr};
        float mAnimationTime {0.0f};
        float mEmissiveAnimationTime {0.0f};
        Pht::Vec3 mRowBombRotation;
        Pht::Vec3 mAsteroidRotation;
    };
}

#endif

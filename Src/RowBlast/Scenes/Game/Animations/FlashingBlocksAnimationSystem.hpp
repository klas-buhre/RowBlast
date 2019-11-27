#ifndef FlashingBlocksAnimationSystem_hpp
#define FlashingBlocksAnimationSystem_hpp

// Engine includes.
#include "Material.hpp"

// Game includes.
#include "Cell.hpp"
#include "PieceResources.hpp"

namespace RowBlast {
    class Field;
    
    class FlashingBlocksAnimationSystem {
    public:
        FlashingBlocksAnimationSystem(Field& field, PieceResources& pieceResources);
    
        void Init();
        void Start(BlockColor color);
        void Update(float dt);
        void ResetFlashingBlockAnimations();
        
        static const Pht::Color colorAdd;
        static const Pht::Color brightColorAdd;
        static const Pht::Color semiFlashingColorAdd;
        
    private:
        void UpdateInWaitingState();
        void ActivateWaitingBlock(SubCell& subCell, int row, int column);
        void UpdateInActiveState(float dt);
        void UpdateFlash(float dt);
        bool IsBlockAccordingToBlueprint(SubCell& subCell, int row, int column);
        void UpdateRenderables();
        void UpdateBlockRenderables(const Pht::Color& flashColorAdd,
                                    BlockBrightness flashingBlockBrightness);
        Pht::Color CalculateFlashColorAdd(const Pht::Color& flashMaxColorAdd);
        void UpdateBlockRenderable(const Pht::Color& flashColorAdd,
                                   BlockBrightness flashingBlockBrightness,
                                   BlockKind blockKind);
        void UpdateWeldRenderables(const Pht::Color& flashColorAdd,
                                   BlockBrightness flashingWeldBrightness);
        void UpdateWeldRenderable(const Pht::Color& flashColorAdd,
                                  BlockBrightness flashingWeldBrightness,
                                  WeldRenderableKind weldKind);
        
        enum class State {
            Waiting,
            Active,
            OnlyFlyingBlocksFlashing,
            Inactive
        };
        
        Field& mField;
        PieceResources& mPieceResources;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        BlockColor mColor;
    };
}

#endif

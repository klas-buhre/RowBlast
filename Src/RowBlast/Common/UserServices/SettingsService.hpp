#ifndef SettingsService_hpp
#define SettingsService_hpp

namespace RowBlast {
    enum class ControlType {
        Drag,
        Click,
        Swipe
    };
    
    enum class ClearRowsEffect {
        Shrink,
        Fly
    };

    class SettingsService {
    public:
        SettingsService();
        
        void SetControlType(ControlType controlType);
        void SetIsGhostPieceEnabled(bool isGhostPieceEnabled);
        void SetIsSoundEnabled(bool isSoundEnabled);
        void SetIsMusicEnabled(bool isMusicEnabled);
        void SetClearRowsEffect(ClearRowsEffect clearRowsEffect);
        
        ControlType GetControlType() const {
            return mControlType;
        }
        
        bool IsGhostPieceEnabled() const {
            return mIsGhostPieceEnabled;
        }

        bool IsSoundEnabled() const {
            return mIsSoundEnabled;
        }

        bool IsMusicEnabled() const {
            return mIsMusicEnabled;
        }
        
        ClearRowsEffect GetClearRowsEffect() const {
            return mClearRowsEffect;
        }

    private:
        void SaveState();
        bool LoadState();

        ControlType mControlType {ControlType::Drag};
        bool mIsGhostPieceEnabled {true};
        bool mIsSoundEnabled {true};
        bool mIsMusicEnabled {true};
        ClearRowsEffect mClearRowsEffect {ClearRowsEffect::Shrink};
    };
}

#endif

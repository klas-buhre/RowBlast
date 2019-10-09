#ifndef SettingsService_hpp
#define SettingsService_hpp

namespace RowBlast {
    enum class ControlType {
        Click,
        Gesture
    };

    class SettingsService {
    public:
        SettingsService();
        
        void SetControlType(ControlType controlType);
        void SetIsGhostPieceEnabled(bool isGhostPieceEnabled);
        void SetIsRotateAllPiecesEnabled(bool isRotateAllPiecesEnabled);
        void SetIsSoundEnabled(bool isSoundEnabled);
        void SetIsMusicEnabled(bool isMusicEnabled);
        
        ControlType GetControlType() const {
            return mControlType;
        }
        
        bool IsGhostPieceEnabled() const {
            return mIsGhostPieceEnabled;
        }

        bool IsRotateAllPiecesEnabled() const {
            return mIsRotateAllPiecesEnabled;
        }

        bool IsSoundEnabled() const {
            return mIsSoundEnabled;
        }

        bool IsMusicEnabled() const {
            return mIsMusicEnabled;
        }

    private:
        void SaveState();
        bool LoadState();

        ControlType mControlType {ControlType::Click};
        bool mIsGhostPieceEnabled {false};
        bool mIsRotateAllPiecesEnabled {false};
        bool mIsSoundEnabled {true};
        bool mIsMusicEnabled {true};
    };
}

#endif

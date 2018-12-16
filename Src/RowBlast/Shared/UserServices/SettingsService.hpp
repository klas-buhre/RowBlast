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
        void SetIsSoundEnabled(bool isSoundEnabled);
        void SetIsMusicEnabled(bool isMusicEnabled);
        
        ControlType GetControlType() const {
            return mControlType;
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
        bool mIsSoundEnabled {true};
        bool mIsMusicEnabled {true};
    };
}

#endif

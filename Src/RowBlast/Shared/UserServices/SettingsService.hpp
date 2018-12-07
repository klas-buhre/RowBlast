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
        void SetIsSoundEnabled(bool iSoundEnabled);
        
        ControlType GetControlType() const {
            return mControlType;
        }
        
        bool IsSoundEnabled() const {
            return mIsSoundEnabled;
        }
        
    private:
        void SaveState();
        bool LoadState();

        ControlType mControlType {ControlType::Click};
        bool mIsSoundEnabled {true};
    };
}

#endif

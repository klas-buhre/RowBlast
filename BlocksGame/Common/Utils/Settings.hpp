#ifndef Settings_hpp
#define Settings_hpp

namespace BlocksGame {
    enum class ControlType {
        Click,
        Gesture
    };
    
    struct Settings {
        ControlType mControlType {ControlType::Click};
    };
}

#endif

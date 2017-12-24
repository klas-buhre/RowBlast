#ifndef Settings_hpp
#define Settings_hpp

namespace BlocksGame {
    enum class Controls {
        Click,
        Gesture
    };
    
    struct Settings {
        Controls mControlType {Controls::Click};
    };
}

#endif

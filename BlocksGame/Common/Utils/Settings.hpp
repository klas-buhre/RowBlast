#ifndef Settings_hpp
#define Settings_hpp

namespace RowBlast {
    enum class ControlType {
        Click,
        Gesture
    };
    
    struct Settings {
        ControlType mControlType {ControlType::Click};
    };
}

#endif

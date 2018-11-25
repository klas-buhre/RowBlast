#ifndef InputUtil_hpp
#define InputUtil_hpp

#include <assert.h>

// Engine includes.
#include "IInput.hpp"
#include "InputEvent.hpp"

namespace RowBlast {
    namespace InputUtil {
        template <typename Result>
        Result HandleInput(Pht::IInput& input,
                           Result defaultResult,
                           std::function<Result(const Pht::TouchEvent& touchEvent)> onTouch) {
            Result result {defaultResult};
            
            while (input.HasEvents()) {
                auto& event {input.GetNextEvent()};
                switch (event.GetKind()) {
                    case Pht::InputKind::Touch: {
                        result = onTouch(event.GetTouchEvent());
                        break;
                    }
                    default:
                        assert(false);
                        break;
                }
                
                input.PopNextEvent();
                
                if (result != defaultResult) {
                    break;
                }
            }
            
            return result;
        }
    }
}

#endif

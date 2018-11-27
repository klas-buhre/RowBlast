#ifndef IGuiLightProvider_hpp
#define IGuiLightProvider_hpp

// Engine includes.
#include "Vector.hpp"

namespace RowBlast {
    class IGuiLightProvider {
    public:
        virtual ~IGuiLightProvider() {}
        
        virtual void SetGuiLightDirections(const Pht::Vec3& directionA,
                                           const Pht::Vec3& directionB) = 0;
        virtual void SetDefaultGuiLightDirections() = 0;
    };
}

#endif

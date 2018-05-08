#ifndef MenuWindow_hpp
#define MenuWindow_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class MenuWindow {
    public:
        enum class Size {
            Large,
            Small
        };
        
        MenuWindow(Pht::IEngine& engine, const CommonResources& commonResources, Size size);

        Pht::RenderableObject& GetRenderable() const {
            return *mRenderableObject;
        }
        
        const Pht::Vec2& GetSize() const {
            return mSize;
        }
        
    private:
        std::unique_ptr<Pht::RenderableObject> mRenderableObject;
        Pht::Vec2 mSize;
    };
}

#endif

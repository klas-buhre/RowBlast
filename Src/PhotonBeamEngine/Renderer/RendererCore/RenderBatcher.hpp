#ifndef RenderBatcher_hpp
#define RenderBatcher_hpp

#include <memory>

#include "Optional.hpp"

namespace Pht {
    class SceneObject;
    class RenderableObject;
    
    namespace RenderBatcher {
        std::unique_ptr<RenderableObject> CreateStaticBatch(const SceneObject& sceneObject,
                                                            const Optional<std::string>& vboName);
    };
}
#endif

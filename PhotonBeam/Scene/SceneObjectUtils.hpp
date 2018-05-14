#ifndef SceneObjectUtils_hpp
#define SceneObjectUtils_hpp

namespace Pht {
    class SceneObject;

    namespace SceneObjectUtils {
        void ScaleRecursively(Pht::SceneObject& sceneObject, float scale);
        void SetAlphaRecursively(Pht::SceneObject& sceneObject, float alpha);
    }
}

#endif

#ifndef FieldBorder_hpp
#define FieldBorder_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneResources.hpp"

namespace Pht {
    class IEngine;
    class IImage;
}

namespace RowBlast {
    class GameScene;
    class CommonResources;

    class FieldBorder {
    public:
        FieldBorder(Pht::IEngine& engine,
                    GameScene& scene,
                    const CommonResources& commonResources,
                    float height);
    
        void Init();
        void SetPositions(const Pht::Vec3& left, const Pht::Vec3& right, const Pht::Vec3& bottom);
        
    private:
        void CreateLeftBorder(Pht::IEngine& engine,
                              GameScene& scene,
                              const CommonResources& commonResources,
                              float height);
        void CreateRightBorder(Pht::IEngine& engine,
                               GameScene& scene,
                               const CommonResources& commonResources,
                               float height);
        std::unique_ptr<Pht::SceneObject> CreateSceneObject(const Pht::IImage& image,
                                                            const Pht::Vec2& size,
                                                            Pht::IEngine& engine);
        
        GameScene& mScene;
        std::unique_ptr<Pht::SceneObject> mLeftBorder;
        std::unique_ptr<Pht::SceneObject> mRightBorder;
        std::unique_ptr<Pht::SceneObject> mBottomBorder;
        Pht::SceneResources mSceneResources;
    };
}

#endif

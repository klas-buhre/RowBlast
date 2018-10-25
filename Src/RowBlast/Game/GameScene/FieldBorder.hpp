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
        FieldBorder(Pht::IEngine& engine, GameScene& scene, const CommonResources& commonResources);
    
        void Init(const Pht::Vec3& leftPosition, const Pht::Vec3& rightPosition, int numRows);
        
        static constexpr auto brightBorderThickness {0.055f};
        static constexpr auto darkerBorderThickness {0.045f};
        static constexpr auto borderThickness {brightBorderThickness + darkerBorderThickness};

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
        Pht::SceneResources mSceneResources;
    };
}

#endif

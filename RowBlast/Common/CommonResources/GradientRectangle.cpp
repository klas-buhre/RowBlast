#include "GradientRectangle.hpp"

// Engine includes.
#include "Scene.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"
#include "SceneObject.hpp"
#include "GuiView.hpp"

using namespace RowBlast;

namespace {
    Pht::SceneObject& CreateGradientQuad(const Pht::Vec2& size,
                                         float tilt,
                                         const Pht::Vec4& leftUpperColor,
                                         const Pht::Vec4& rightUpperColor,
                                         const Pht::Vec4& leftLowerColor,
                                         const Pht::Vec4& rightLowerColor,
                                         const Pht::Material& material,
                                         Pht::Scene& scene) {
        Pht::QuadMesh::Vertices vertices {
            {{-size.x / 2.0f, -size.y / 2.0f, 0.0f}, leftLowerColor},
            {{size.x / 2.0f, -size.y / 2.0f, 0.0f}, rightLowerColor},
            {{size.x / 2.0f + tilt, size.y / 2.0f, 0.0f}, rightUpperColor},
            {{-size.x / 2.0f + tilt, size.y / 2.0f, 0.0f}, leftUpperColor},
        };
        
        return scene.CreateSceneObject(Pht::QuadMesh {vertices}, material);
    }
    
    Pht::SceneObject& CreateGradientQuad(const Pht::Vec2& size,
                                         float tilt,
                                         const Pht::Vec4& leftUpperColor,
                                         const Pht::Vec4& rightUpperColor,
                                         const Pht::Vec4& leftLowerColor,
                                         const Pht::Vec4& rightLowerColor,
                                         const Pht::Material& material,
                                         Pht::GuiView& view,
                                         Pht::ISceneManager& sceneManager) {
        Pht::QuadMesh::Vertices vertices {
            {{-size.x / 2.0f, -size.y / 2.0f, 0.0f}, leftLowerColor},
            {{size.x / 2.0f, -size.y / 2.0f, 0.0f}, rightLowerColor},
            {{size.x / 2.0f + tilt, size.y / 2.0f, 0.0f}, rightUpperColor},
            {{-size.x / 2.0f + tilt, size.y / 2.0f, 0.0f}, leftUpperColor},
        };
        
        return view.CreateSceneObject(Pht::QuadMesh {vertices}, material, sceneManager);
    }
}

Pht::SceneObject& RowBlast::CreateGradientRectangle(Pht::Scene& scene,
                                                    Pht::SceneObject& parentObject,
                                                    const Pht::Vec3& position,
                                                    const Pht::Vec2& size,
                                                    float tilt,
                                                    float leftQuadWidth,
                                                    float rightQuadWidth,
                                                    const GradientRectangleColors& upperColors,
                                                    const GradientRectangleColors& lowerColors) {
    auto& sceneObject {scene.CreateSceneObject()};
    parentObject.AddChild(sceneObject);
    
    Pht::Material material;
    float midQuadWidth {size.x - leftQuadWidth - rightQuadWidth};

    auto& midQuad {
        CreateGradientQuad({midQuadWidth, size.y},
                           tilt,
                           upperColors.mMid,
                           upperColors.mMid,
                           lowerColors.mMid,
                           lowerColors.mMid,
                           material,
                           scene)
    };
    midQuad.GetTransform().SetPosition({position.x, position.y, position.z});
    sceneObject.AddChild(midQuad);
    
    auto& leftQuad {
        CreateGradientQuad({leftQuadWidth, size.y},
                           tilt,
                           upperColors.mLeft,
                           upperColors.mMid,
                           lowerColors.mLeft,
                           lowerColors.mMid,
                           material,
                           scene)
    };
    leftQuad.GetTransform().SetPosition({position.x - size.x / 2.0f + leftQuadWidth / 2.0f,
                                         position.y,
                                         position.z});
    sceneObject.AddChild(leftQuad);
    
    auto& rightQuad {
        CreateGradientQuad({rightQuadWidth, size.y},
                           tilt,
                           upperColors.mMid,
                           upperColors.mRight,
                           lowerColors.mMid,
                           lowerColors.mRight,
                           material,
                           scene)
    };
    rightQuad.GetTransform().SetPosition({position.x + size.x / 2.0f - rightQuadWidth / 2.0f,
                                          position.y,
                                          position.z});
    sceneObject.AddChild(rightQuad);
    
    return sceneObject;
}

Pht::SceneObject& RowBlast::CreateGradientRectangle(Pht::ISceneManager& sceneManager,
                                                    Pht::GuiView& view,
                                                    Pht::SceneObject& parentObject,
                                                    const Pht::Vec3& position,
                                                    const Pht::Vec2& size,
                                                    float tilt,
                                                    float leftQuadWidth,
                                                    float rightQuadWidth,
                                                    const GradientRectangleColors& upperColors,
                                                    const GradientRectangleColors& lowerColors) {
    auto& sceneObject {view.CreateSceneObject()};
    parentObject.AddChild(sceneObject);
    
    Pht::Material material;
    float midQuadWidth {size.x - leftQuadWidth - rightQuadWidth};

    auto& midQuad {
        CreateGradientQuad({midQuadWidth, size.y},
                           tilt,
                           upperColors.mMid,
                           upperColors.mMid,
                           lowerColors.mMid,
                           lowerColors.mMid,
                           material,
                           view,
                           sceneManager)
    };
    midQuad.GetTransform().SetPosition({position.x, position.y, position.z});
    sceneObject.AddChild(midQuad);
    
    auto& leftQuad {
        CreateGradientQuad({leftQuadWidth, size.y},
                           tilt,
                           upperColors.mLeft,
                           upperColors.mMid,
                           lowerColors.mLeft,
                           lowerColors.mMid,
                           material,
                           view,
                           sceneManager)
    };
    leftQuad.GetTransform().SetPosition({position.x - size.x / 2.0f + leftQuadWidth / 2.0f,
                                         position.y,
                                         position.z});
    sceneObject.AddChild(leftQuad);
    
    auto& rightQuad {
        CreateGradientQuad({rightQuadWidth, size.y},
                           tilt,
                           upperColors.mMid,
                           upperColors.mRight,
                           lowerColors.mMid,
                           lowerColors.mRight,
                           material,
                           view,
                           sceneManager)
    };
    rightQuad.GetTransform().SetPosition({position.x + size.x / 2.0f - rightQuadWidth / 2.0f,
                                          position.y,
                                          position.z});
    sceneObject.AddChild(rightQuad);
    
    return sceneObject;
}

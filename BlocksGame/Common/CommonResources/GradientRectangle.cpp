#include "GradientRectangle.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

using namespace BlocksGame;

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
}

Pht::SceneObject& BlocksGame::CreateGradientRectangle(Pht::Scene& scene,
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

GradientRectangle::GradientRectangle(Pht::IEngine& engine,
                                     const Pht::Vec3& position,
                                     const Pht::Vec2& size,
                                     float tilt,
                                     float leftQuadWidth,
                                     float rightQuadWidth,
                                     const GradientRectangleColors& upperColors,
                                     const GradientRectangleColors& lowerColors) {
    Pht::Material material;
    float midQuadWidth {size.x - leftQuadWidth - rightQuadWidth};
    
    mMidQuad = std::make_unique<Pht::SceneObject>(
        CreateGradientQuad({midQuadWidth, size.y},
                           tilt,
                           upperColors.mMid,
                           upperColors.mMid,
                           lowerColors.mMid,
                           lowerColors.mMid,
                           material,
                           engine));
    mMidQuad->SetPosition({position.x, position.y, position.z});
    
    mLeftQuad = std::make_unique<Pht::SceneObject>(
        CreateGradientQuad({leftQuadWidth, size.y},
                           tilt,
                           upperColors.mLeft,
                           upperColors.mMid,
                           lowerColors.mLeft,
                           lowerColors.mMid,
                           material,
                           engine));
    mLeftQuad->SetPosition({position.x - size.x / 2.0f + leftQuadWidth / 2.0f, position.y, position.z});

    mRightQuad = std::make_unique<Pht::SceneObject>(
        CreateGradientQuad({rightQuadWidth, size.y},
                           tilt,
                           upperColors.mMid,
                           upperColors.mRight,
                           lowerColors.mMid,
                           lowerColors.mRight,
                           material,
                           engine));
    mRightQuad->SetPosition({position.x + size.x / 2.0f - rightQuadWidth / 2.0f, position.y, position.z});
}

std::unique_ptr<Pht::RenderableObject>
GradientRectangle::CreateGradientQuad(const Pht::Vec2& size,
                                      float tilt,
                                      const Pht::Vec4& leftUpperColor,
                                      const Pht::Vec4& rightUpperColor,
                                      const Pht::Vec4& leftLowerColor,
                                      const Pht::Vec4& rightLowerColor,
                                      const Pht::Material& material,
                                      Pht::IEngine& engine) {
    Pht::QuadMesh::Vertices vertices {
        {{-size.x / 2.0f, -size.y / 2.0f, 0.0f}, leftLowerColor},
        {{size.x / 2.0f, -size.y / 2.0f, 0.0f}, rightLowerColor},
        {{size.x / 2.0f + tilt, size.y / 2.0f, 0.0f}, rightUpperColor},
        {{-size.x / 2.0f + tilt, size.y / 2.0f, 0.0f}, leftUpperColor},
    };
    
    auto& sceneManager {engine.GetSceneManager()};
    return sceneManager.CreateRenderableObject(Pht::QuadMesh {vertices}, material);
}

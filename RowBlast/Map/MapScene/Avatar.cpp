#include "Avatar.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "ObjMesh.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "MapScene.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto pictureSize {0.7f};
}

Avatar::Avatar(Pht::IEngine& engine, MapScene& mapScene, const CommonResources& commonResources) :
    mMapScene {mapScene},
    mSceneObject {std::make_unique<Pht::SceneObject>()} {

    auto& sceneManager {engine.GetSceneManager()};
    
    auto frame {
        sceneManager.CreateSceneObject(Pht::ObjMesh {"avatar_frame_266.obj", 0.4f},
                                       commonResources.GetMaterials().GetGoldMaterial(),
                                       mSceneResources)
    };
    mSceneObject->AddChild(*frame);
    mSceneResources.AddSceneObject(std::move(frame));
    
    Pht::Material pictureMaterial {"smiley_avatar.jpg"};
    auto picture {
        sceneManager.CreateSceneObject(Pht::QuadMesh {pictureSize, pictureSize},
                                       pictureMaterial,
                                       mSceneResources)
    };
    mSceneObject->AddChild(*picture);
    mSceneResources.AddSceneObject(std::move(picture));
}

void Avatar::Init() {
    mMapScene.GetPinsContainer().AddChild(*mSceneObject);
}

void Avatar::SetPosition(const Pht::Vec3& position) {
    mSceneObject->GetTransform().SetPosition(position);
}

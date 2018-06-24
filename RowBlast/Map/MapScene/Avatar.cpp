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
    constexpr auto pictureSize {0.65f};
    const Pht::Vec3 avatarOffset {0.0f, -0.9f, 0.75f};
}

Avatar::Avatar(Pht::IEngine& engine, MapScene& mapScene, const CommonResources& commonResources) :
    mMapScene {mapScene},
    mSceneObject {std::make_unique<Pht::SceneObject>()} {

    auto& sceneManager {engine.GetSceneManager()};
    
    auto frame {
        sceneManager.CreateSceneObject(Pht::ObjMesh {"avatar_frame_208.obj", 0.38f},
                                       commonResources.GetMaterials().GetGoldMaterial(),
                                       mSceneResources)
    };

    frame->GetTransform().SetPosition(avatarOffset);
    mSceneObject->AddChild(*frame);
    mSceneResources.AddSceneObject(std::move(frame));
    
    Pht::Material pictureMaterial {"avatar.jpg"};
    auto picture {
        sceneManager.CreateSceneObject(Pht::QuadMesh {pictureSize, pictureSize},
                                       pictureMaterial,
                                       mSceneResources)
    };

    picture->GetTransform().SetPosition(avatarOffset - Pht::Vec3{0.0f, 0.0f, 0.01f});
    mSceneObject->AddChild(*picture);
    mSceneResources.AddSceneObject(std::move(picture));
}

void Avatar::Init() {
    mMapScene.GetAvatarContainer().AddChild(*mSceneObject);
    mSceneObject->SetIsVisible(true);
}

void Avatar::SetPosition(const Pht::Vec3& position) {
    mSceneObject->GetTransform().SetPosition(position);
}

const Pht::Vec3& Avatar::GetPosition() const {
    return mSceneObject->GetTransform().GetPosition();
}

void Avatar::Hide() {
    mSceneObject->SetIsVisible(false);
}

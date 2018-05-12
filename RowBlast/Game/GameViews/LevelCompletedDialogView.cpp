#include "LevelCompletedDialogView.hpp"

#include <assert.h>
#include <array>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ObjMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "MenuQuad.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 shadowOffset {-0.3f, -0.3f, -0.3f};
    
    std::array<Pht::Vec3, 3> starOffsets {
        Pht::Vec3{-3.8f, 1.3f, UiLayer::block},
        Pht::Vec3{0.0f, 1.8f, UiLayer::block},
        Pht::Vec3{3.8f, 1.3f, UiLayer::block}
    };
}

LevelCompletedDialogView::LevelCompletedDialogView(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) :
    mEngine {engine} {
    
    LoadStar(commonResources);
     
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-3.4f, 5.0f, UiLayer::text},
               "LEVEL CLEARED",
               guiResources.GetCaptionTextProperties(zoom));
    
    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.5f,
        GetSize().y / 2.0f - 1.5f,
        UiLayer::textRectangle
    };
    
    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};
    
    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mMeshFilename = GuiResources::mCloseButtonMeshFilename;
    closeButtonStyle.mColor = GuiResources::mBlackButtonColor;
    closeButtonStyle.mSelectedColor = GuiResources::mBlackSelectedButtonColor;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mHasShadow = true;
    
    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);
    mCloseButton->CreateText({-0.34f, -0.35f, UiLayer::text},
                             "X",
                             guiResources.GetLargeWhiteButtonTextProperties(zoom));

    Pht::Vec2 nextButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style nextButtonStyle;
    nextButtonStyle.mMeshFilename = GuiResources::mBigButtonMeshFilename;
    nextButtonStyle.mColor = GuiResources::mBlueButtonColor;
    nextButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    nextButtonStyle.mPressedScale = 1.05f;
    nextButtonStyle.mHasShadow = true;

    mNextButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -2.9f, UiLayer::textRectangle},
                                               nextButtonInputSize,
                                               nextButtonStyle);
    mNextButton->CreateText({-1.1f, -0.31f, UiLayer::buttonText},
                            "Next",
                            guiResources.GetLargeWhiteButtonTextProperties(zoom));

    for (auto i {0}; i < 3; ++i) {
        auto star {std::make_unique<Pht::SceneObject>(mStarRenderable.get())};
        auto& transform {star->GetTransform()};
        transform.SetRotation({90.0f, 0.0f, 0.0f});
        transform.SetPosition(starOffsets[i]);
        mStars.push_back(star.get());
        AddSceneObject(std::move(star));

        auto shadow {std::make_unique<Pht::SceneObject>(mStarShadowRenderable.get())};
        auto& shadowTransform {shadow->GetTransform()};
        shadowTransform.SetRotation({90.0f, 0.0f, 0.0f});
        shadowTransform.SetPosition(starOffsets[i] + shadowOffset);
        mStarShadows.push_back(shadow.get());
        AddSceneObject(std::move(shadow));
    }
}

void LevelCompletedDialogView::LoadStar(const CommonResources& commonResources) {
    auto& sceneManager {mEngine.GetSceneManager()};
    mStarRenderable = sceneManager.CreateRenderableObject(
        Pht::ObjMesh {"star_1428.obj", 0.26f}, commonResources.GetMaterials().GetGoldMaterial());

    Pht::Material shaddowMaterial {Pht::Color{0.4f, 0.4f, 0.4f}};
    shaddowMaterial.SetOpacity(0.075f);
    mStarShadowRenderable = sceneManager.CreateRenderableObject(
        Pht::ObjMesh {"star_1428.obj", 0.26f}, shaddowMaterial);
}

void LevelCompletedDialogView::Init() {
    mStarAngle = 0.0f;
}

void LevelCompletedDialogView::Update() {
    auto speed {12.0f};
    
    for (auto* star: mStars) {
        auto& transform {star->GetTransform()};
        transform.SetRotation({90.0f, 0.0f, mStarAngle});
    }

    for (auto* shadow: mStarShadows) {
        auto& transform {shadow->GetTransform()};
        transform.SetRotation({90.0f, 0.0f, mStarAngle});
    }

    mStarAngle -= mEngine.GetLastFrameSeconds() * speed;
}

void LevelCompletedDialogView::SetNumStars(int numStars) {
    assert(numStars <= 3);
    
    for (auto* star: mStars) {
        star->SetIsVisible(false);
    }

    for (auto* shadow: mStarShadows) {
        shadow->SetIsVisible(false);
    }

    for (auto i {0}; i < numStars; ++i) {
        mStars[i]->SetIsVisible(true);
        mStarShadows[i]->SetIsVisible(true);
    }
}

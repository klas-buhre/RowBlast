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

using namespace BlocksGame;

namespace {
    std::array<Pht::Vec3, 3> starOffsets {
        Pht::Vec3{-3.8f, 1.8f, UiLayer::textRectangle},
        Pht::Vec3{0.0f, 2.3f, UiLayer::textRectangle},
        Pht::Vec3{3.8f, 1.8f, UiLayer::textRectangle}
    };
}

LevelCompletedDialogView::LevelCompletedDialogView(Pht::IEngine& engine,
                                                   const CommonResources& commonResources) :
    mEngine {engine} {
    
    Pht::TextProperties textProperties {commonResources.GetHussarFontSize27()};
    LoadStar(commonResources);

    Pht::Vec2 size {engine.GetRenderer().GetHudFrustumSize().x, 11.0f};
    SetSize(size);
    SetPosition({0.0f, 0.0f});
    
    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.0f,
        GetSize().y / 2.0f - 1.0f,
        UiLayer::textRectangle
    };
    mCloseButton = std::make_unique<CloseButton>(engine, *this, closeButtonPosition, textProperties);

    Pht::Vec2 buttonSize {6.5f, 2.1f};
    Pht::Vec2 buttonInputSize {180.0f, 47.0f};
    MenuButton::Style buttonStyle;
    buttonStyle.mColor = Pht::Color {0.2f, 0.82f, 0.2f};
    buttonStyle.mSelectedColor = Pht::Color {0.23f, 1.0f, 0.23f};
    
    mNextButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -3.5f, UiLayer::textRectangle},
                                               buttonSize,
                                               buttonInputSize,
                                               buttonStyle);
    mNextButton->SetText(
        std::make_unique<Pht::Text>(Pht::Vec2 {-0.9f, -0.23f}, "NEXT", textProperties));
    
    auto levelClearedText {
        std::make_unique<Pht::Text>(Pht::Vec2 {-2.3f, -1.5f}, "Level cleared!", textProperties)
    };
    
    AddText(std::move(levelClearedText));
    
    for (auto i {0}; i < 3; ++i) {
        auto star {std::make_unique<Pht::SceneObject>(mStarRenderable.get())};
        star->SetRotationX(-90.0f);
        star->SetPosition(starOffsets[i]);
        mStars.push_back(star.get());
        AddSceneObject(std::move(star));
    }
}

void LevelCompletedDialogView::LoadStar(const CommonResources& commonResources) {
    auto& sceneManager {mEngine.GetSceneManager()};
    mStarRenderable = sceneManager.CreateRenderableObject(
        Pht::ObjMesh {"star_1428.obj", 0.26f}, commonResources.GetMaterials().GetGoldMaterial());
}

void LevelCompletedDialogView::Init() {
    mStarAngle = 0.0f;
}

void LevelCompletedDialogView::Update() {
    auto starIndex {0};
    auto speed {12.0f};
    
    for (auto* star: mStars) {
        star->SetRotationX(90.0f);
        star->SetRotationZ(mStarAngle);
        star->SetPosition(starOffsets[starIndex]);
        
        ++starIndex;
    }
    
    mStarAngle -= mEngine.GetLastFrameSeconds() * speed;
}

void LevelCompletedDialogView::SetNumStars(int numStars) {
    assert(numStars <= 3);
    
    for (auto* star: mStars) {
        star->SetIsVisible(false);
    }
    
    for (auto i {0}; i < numStars; ++i) {
        mStars[i]->SetIsVisible(true);
    }
}

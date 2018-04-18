#include "RestartConfirmationDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "MenuQuad.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

RestartConfirmationDialogView::RestartConfirmationDialogView(Pht::IEngine& engine,
                                                             const CommonResources& commonResources) {
    Pht::TextProperties textProperties {commonResources.GetHussarFontSize27()};
    Pht::TextProperties restartTextProperties {commonResources.GetHussarFontSize30()};
    
    auto frustumWidth {engine.GetRenderer().GetHudFrustumSize().x};
    Pht::Vec2 size {frustumWidth, 11.5f};
    SetSize(size);
    SetPosition({0.0f, 0.0f});
    
    auto quad {MenuQuad::CreateGray(engine, GetSceneResources(), size)};
    quad->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(quad));
    
    CreateText({-1.85f, 4.0f, UiLayer::text}, "RESTART", restartTextProperties);
    
    Pht::Material lineMaterial {Pht::Color{1.0f, 1.0f, 1.0f}};
    lineMaterial.SetOpacity(0.4f);
    auto& sceneManager {engine.GetSceneManager()};
    auto lineSceneObject {
        sceneManager.CreateSceneObject(Pht::QuadMesh {frustumWidth - 1.0f, 0.08f},
                                       lineMaterial,
                                       GetSceneResources())
    };
    lineSceneObject->GetTransform().SetPosition({0.0f, 3.0f, UiLayer::textRectangle});
    AddSceneObject(std::move(lineSceneObject));
    
    CreateText({-5.4f, 1.5f, UiLayer::text}, "Are you sure you want to restart", textProperties);
    CreateText({-1.65f, 0.5f, UiLayer::text}, "the level?", textProperties);
    
    Pht::Vec2 buttonSize {10.0f, 1.72f};
    Pht::Vec2 buttonInputSize {215.0f, 43.0f};
    MenuButton::Style buttonStyle;
    buttonStyle.mColor = Pht::Color {0.4f, 0.74f, 1.0f};
    buttonStyle.mSelectedColor = Pht::Color {0.6f, 0.94f, 1.0f};
    buttonStyle.mPressedScale = 0.925f;
    buttonStyle.mOpacity = 0.76f;
    buttonStyle.mIsRounded = false;
    
    mYesButton = std::make_unique<MenuButton>(engine,
                                              *this,
                                              Pht::Vec3 {0.0f, -1.4f, UiLayer::textRectangle},
                                              buttonSize,
                                              buttonInputSize,
                                              buttonStyle);
    mYesButton->CreateText({-0.6f, -0.23f, UiLayer::buttonText}, "YES", textProperties);
    
    mNoButton = std::make_unique<MenuButton>(engine,
                                             *this,
                                             Pht::Vec3 {0.0f, -3.7f, UiLayer::textRectangle},
                                             buttonSize,
                                             buttonInputSize,
                                             buttonStyle);
    mNoButton->CreateText({-0.5f, -0.23f, UiLayer::buttonText}, "NO", textProperties);
}

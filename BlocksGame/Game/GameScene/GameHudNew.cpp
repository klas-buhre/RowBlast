#include "GameHudNew.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "GradientRectangle.hpp"
#include "GameLogic.hpp"
#include "LevelResources.hpp"
#include "PieceResources.hpp"

using namespace BlocksGame;

namespace {
    const Pht::Vec3 lightDirectionA {0.6f, 1.0f, 1.0f};
    const Pht::Vec3 lightDirectionB {0.4f, 1.0f, 1.0f};
    const auto lightAnimationDuration {5.0f};
}

GameHudNew::GameHudNew(Pht::IEngine& engine,
                const GameLogic& gameLogic,
                const LevelResources& levelResources,
                const PieceResources& pieceResources,
                GameHudController& gameHudController,
                const Pht::Font& font,
                Pht::Scene& scene,
                Pht::SceneObject& parentObject,
                int hudLayer,
                const Level& level) :
    mEngine {engine},
    mGameLogic {gameLogic},
    mLevelObjective {level.GetObjective()} {

    CreateLightAndCamera(scene, parentObject, hudLayer);
    
    Pht::TextProperties textProperties {font, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}};
    CreateProgressObject(scene, parentObject, textProperties, levelResources);
    CreateMovesObject(scene, parentObject, textProperties, pieceResources);
}

void GameHudNew::CreateLightAndCamera(Pht::Scene& scene,
                                      Pht::SceneObject& parentObject,
                                      int hudLayer) {
    auto& lightSceneObject {scene.CreateSceneObject()};
    lightSceneObject.SetIsVisible(false);
    auto lightComponent {std::make_unique<Pht::LightComponent>(lightSceneObject)};
    mLight = lightComponent.get();
    lightSceneObject.SetComponent<Pht::LightComponent>(std::move(lightComponent));
    parentObject.AddChild(lightSceneObject);
    
    auto& cameraSceneObject {scene.CreateSceneObject()};
    cameraSceneObject.SetIsVisible(false);
    cameraSceneObject.GetTransform().SetPosition({0.0f, 0.0f, 15.5f});
    auto cameraComponent {std::make_unique<Pht::CameraComponent>(cameraSceneObject)};
    auto* camera {cameraComponent.get()};
    cameraSceneObject.SetComponent<Pht::CameraComponent>(std::move(cameraComponent));
    parentObject.AddChild(cameraSceneObject);
    
    auto* hudRenderPass {scene.GetRenderPass(hudLayer)};
    assert(hudRenderPass);
    hudRenderPass->SetLight(mLight);
    hudRenderPass->SetCamera(camera);
}

void GameHudNew::CreateProgressObject(Pht::Scene& scene,
                                      Pht::SceneObject& parentObject,
                                      const Pht::TextProperties& textProperties,
                                      const LevelResources& levelResources) {
    auto& progressContainer {scene.CreateSceneObject()};
    progressContainer.GetTransform().SetPosition({-4.1f, 12.6f, 0.0f});
    parentObject.AddChild(progressContainer);
    
    CreateTextRectangle({0.9f, 0.2f, -3.0f}, 4.4f, false, scene, progressContainer);
    
    std::string text {"    "};  // Warning! Must be four spaces to fit digits.
    mProgressText = &scene.CreateText(text, textProperties);
    auto& progressTextSceneobject {mProgressText->GetSceneObject()};
    progressTextSceneobject.GetTransform().SetPosition({1.1f, 0.0f, 0.0f});
    progressContainer.AddChild(progressTextSceneobject);
    
    switch (mLevelObjective) {
        case Level::Objective::Clear:
            CreateSmallPiecesRectangle({0.15f, 0.1f, -2.0f}, scene, progressContainer);
            CreateGrayBlock(scene, progressContainer, levelResources);
            break;
        case Level::Objective::Build:
            CreateBlueprintSlot(scene, progressContainer, levelResources);
            break;
    }
}

void GameHudNew::CreateGrayBlock(Pht::Scene& scene,
                                 Pht::SceneObject& progressContainer,
                                 const LevelResources& levelResources) {
    auto& grayBlock {scene.CreateSceneObject()};
    grayBlock.SetRenderable(&levelResources.GetLevelBlockRenderable(BlockRenderableKind::Full));
    
    auto& transform {grayBlock.GetTransform()};
    transform.SetPosition({0.19f, 0.2f, -1.0f});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    auto scale {0.505f};
    transform.SetScale({scale, scale, scale});
    
    progressContainer.AddChild(grayBlock);
}

void GameHudNew::CreateBlueprintSlot(Pht::Scene& scene,
                                     Pht::SceneObject& progressContainer,
                                     const LevelResources& levelResources) {
    auto& blueprintSlot {scene.CreateSceneObject()};
    blueprintSlot.SetRenderable(&levelResources.GetBlueprintSlotRenderable());
    
    auto& transform {blueprintSlot.GetTransform()};
    transform.SetPosition({0.55f, 0.2f, -1.0f});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    auto scale {0.56f};
    transform.SetScale({scale, scale, scale});
    
    progressContainer.AddChild(blueprintSlot);
}

void GameHudNew::CreateMovesObject(Pht::Scene& scene,
                                   Pht::SceneObject& parentObject,
                                   const Pht::TextProperties& textProperties,
                                   const PieceResources& pieceResources) {
    auto& movesContainer {scene.CreateSceneObject()};
    movesContainer.GetTransform().SetPosition({3.1f, 12.6f, 0.0f});
    parentObject.AddChild(movesContainer);
    
    CreateTextRectangle({0.9f, 0.2f, -3.0f}, 4.4f, false, scene, movesContainer);
    
    std::string text {"   "};   // Warning! Must be three spaces to fit digits.
    mMovesText = &scene.CreateText(text, textProperties);
    auto& movesTextSceneobject {mMovesText->GetSceneObject()};
    movesTextSceneobject.GetTransform().SetPosition({1.1f, 0.0f, 0.0f});
    movesContainer.AddChild(movesTextSceneobject);
    
    CreateSmallPiecesRectangle({0.15f, 0.1f, -2.0f}, scene, movesContainer);
    CreateLPiece(scene, movesContainer, pieceResources);
}

void GameHudNew::CreateLPiece(Pht::Scene& scene,
                              Pht::SceneObject& movesContainer,
                              const PieceResources& pieceResources) {
    auto& lPiece {scene.CreateSceneObject()};
    movesContainer.AddChild(lPiece);
    
    auto& baseTransform {lPiece.GetTransform()};
    baseTransform.SetPosition({0.12f, 0.2f, 0.0f});
    baseTransform.SetRotation({-30.0f, -30.0f, 0.0f});
    auto scale {0.32f};
    baseTransform.SetScale({scale, scale, scale});
    
    auto& greenBlockRenderable {
        pieceResources.GetBlockRenderableObject(BlockRenderableKind::Full,
                                                BlockColor::Green,
                                                BlockBrightness::Normal)
    };
    
    auto halfCellSize {0.625f};
    CreateGreenBlock({-halfCellSize, -halfCellSize, -scale}, greenBlockRenderable, scene, lPiece);
    CreateGreenBlock({halfCellSize, -halfCellSize, -scale}, greenBlockRenderable, scene, lPiece);
    CreateGreenBlock({halfCellSize, halfCellSize, -scale}, greenBlockRenderable, scene, lPiece);
}

void GameHudNew::CreateGreenBlock(const Pht::Vec3& position,
                                  Pht::RenderableObject& blockRenderable,
                                  Pht::Scene& scene,
                                  Pht::SceneObject& lPiece) {
    auto& block {scene.CreateSceneObject()};
    block.GetTransform().SetPosition(position);
    block.SetRenderable(&blockRenderable);
    lPiece.AddChild(block);
}

void GameHudNew::CreateSmallPiecesRectangle(const Pht::Vec3& position,
                                            Pht::Scene& scene,
                                            Pht::SceneObject& parentObject) {
    Pht::Vec2 size {1.8f, 1.3f};
    auto tilt {0.23f};
    float leftQuadWidth {0.3f};
    float rightQuadWidth {0.3f};
 
    GradientRectangleColors upperColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {0.85f, 0.75f, 0.95f, 0.9f},
        .mRight = {0.9f, 0.9f, 1.0f, 0.0f}
    };

    GradientRectangleColors lowerColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {0.9f, 0.9f, 1.0f, 0.0f},
        .mRight = {0.9f, 0.9f, 1.0f, 0.0f}
    };
    
    CreateGradientRectangle(scene,
                            parentObject,
                            position,
                            size,
                            tilt,
                            leftQuadWidth,
                            rightQuadWidth,
                            upperColors,
                            lowerColors);
}

void GameHudNew::CreateTextRectangle(const Pht::Vec3& position,
                                     float length,
                                     bool isBright,
                                     Pht::Scene& scene,
                                     Pht::SceneObject& parentObject) {
    Pht::Vec2 size {length, 0.7f};
    float leftQuadWidth {1.0f};
    float rightQuadWidth {1.0f};
    
    GradientRectangleColors colors {
        .mLeft = {0.6f, 0.3f, 0.75f, 0.0f},
        .mMid = {isBright ? 0.7f : 0.6f, isBright ? 0.5f : 0.3f, isBright ? 0.85f : 0.75f, 0.8f},
        .mRight = {0.6f, 0.3f, 0.75f, 0.0f}
    };

    CreateGradientRectangle(scene,
                            parentObject,
                            position,
                            size,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            colors,
                            colors);
}

void GameHudNew::OnSwitchButtonDown() {

}

void GameHudNew::OnSwitchButtonUp() {

}

void GameHudNew::Update() {
    UpdateLightAnimation();
    UpdateProgress();
    UpdateMovesLeft();
}

void GameHudNew::UpdateLightAnimation() {
    mLightAnimationTime += mEngine.GetLastFrameSeconds();
    
    if (mLightAnimationTime > lightAnimationDuration) {
        mLightAnimationTime = 0.0f;
    }
    
    auto t {(sin(mLightAnimationTime * 2.0f * 3.1415f / lightAnimationDuration) + 1.0f) / 2.0f};
    mLight->SetDirection(lightDirectionA.Lerp(t, lightDirectionB));
}

void GameHudNew::UpdateProgress() {
    auto progress {
        mLevelObjective == Level::Objective::Clear ? mGameLogic.GetNumLevelBlocksLeft() :
                                                     mGameLogic.GetNumEmptyBlueprintSlotsLeft()
    };
    
    if (progress != mProgress) {
        const auto bufSize {64};
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%4d", progress);
        auto numDigits {std::strlen(buffer)};
        assert(numDigits <= 4);
        
        auto& text {mProgressText->GetText()};
        auto textLength {text.size()};
        assert(textLength == 4);
        text[0] = buffer[0];
        text[1] = buffer[1];
        text[2] = buffer[2];
        text[3] = buffer[3];
        
        mProgress = progress;
    }
}

void GameHudNew::UpdateMovesLeft() {
    auto movesLeft {mGameLogic.GetMovesLeft()};
    
    if (movesLeft != mMovesLeft) {
        const auto bufSize {64};
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%3d", movesLeft);
        auto numDigits {std::strlen(buffer)};
        assert(numDigits <= 3);
        
        auto& text {mMovesText->GetText()};
        auto textLength {text.size()};
        assert(textLength == 3);
        text[0] = buffer[0];
        text[1] = buffer[1];
        text[2] = buffer[2];
        
        mMovesLeft = movesLeft;
    }
}

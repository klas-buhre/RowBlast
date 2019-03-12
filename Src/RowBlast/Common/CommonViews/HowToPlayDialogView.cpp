#include "HowToPlayDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"
#include "IAnimationSystem.hpp"
#include "Animation.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "PieceResources.hpp"
#include "LevelResources.hpp"
#include "IGuiLightProvider.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 lightDirectionA {0.785f, 1.0f, 0.67f};
    const Pht::Vec3 lightDirectionB {1.0f, 1.0f, 0.74f};
}

HowToPlayDialogView::HowToPlayDialogView(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         const PieceResources& pieceResources,
                                         const LevelResources& levelResources,
                                         SceneId sceneId) :
    mEngine {engine} {

    auto zoom {sceneId == SceneId::Game ? PotentiallyZoomedScreen::Yes : PotentiallyZoomedScreen::No};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-4.5f, 8.25f, UiLayer::text}, "HOW TO PLAY", largeTextProperties);

    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.3f,
        GetSize().y / 2.0f - 1.3f,
        UiLayer::textRectangle
    };

    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};

    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mRenderableObject = &guiResources.GetCloseButton(zoom);
    
    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    CreateBlockAnimation(pieceResources, levelResources);

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.4f, -6.4f, UiLayer::text}, "Bring the asteroid down to", textProperties);
    CreateText({-3.8f, -7.475f, UiLayer::text}, "the bottom of the field", textProperties);
}

void HowToPlayDialogView::CreateBlockAnimation(const PieceResources& pieceResources,
                                               const LevelResources& levelResources) {
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition({0.0f, 1.0f, 0.0f});
    container.GetTransform().SetScale(1.25f);
    
    GetRoot().AddChild(container);
    
    auto& animationSystem {mEngine.GetAnimationSystem()};
    auto& rootAnimation {animationSystem.CreateAnimation(container)};
    mAnimation = &rootAnimation;
    rootAnimation.AddKeyframe(Pht::Keyframe {0.0f});
    rootAnimation.AddKeyframe(Pht::Keyframe {4.0f});
    
    Pht::Vec3 lPieceInitialPosition {-0.5f, 3.5f, UiLayer::block};
    auto& lPiece {CreateLPiece(lPieceInitialPosition, container, pieceResources)};
    Pht::Vec3 remainingLPieceInitialPosition {-0.5f, -2.0f, UiLayer::block};
    auto& remainingLPiece {CreateTwoBlocks(remainingLPieceInitialPosition, BlockColor::Yellow, container, pieceResources)};
    remainingLPiece.SetIsVisible(false);
    Pht::Vec3 redBlocksInitialPosition {2.5f, -2.0f, UiLayer::block};
    auto& redBlocks {CreateTwoBlocks(redBlocksInitialPosition, BlockColor::Red, container, pieceResources)};
    auto& leftGrayBlocks {CreateThreeGrayBlocks({-2.0f, -3.0f, UiLayer::block}, container, levelResources)};
    auto& rightGrayBlocks {CreateThreeGrayBlocks({2.0f, -3.0f, UiLayer::block}, container, levelResources)};
    CreateThreeGrayBlocksWithGap({-1.5f, -4.0f, UiLayer::block}, container, levelResources);
    CreateThreeGrayBlocks({2.0f, -4.0f, UiLayer::block}, container, levelResources);
    
    auto rowClearTime {1.6f};
    auto fallWaitDuration {0.35f};
    auto fallDuration {0.2f};
    auto animationDuration {4.0f};
    
    auto& lPieceAnimation {animationSystem.CreateAnimation(lPiece)};
    
    Pht::Keyframe lPieceAnimationKeyframe1 {0.0f};
    lPieceAnimationKeyframe1.SetPosition(lPieceInitialPosition);
    lPieceAnimationKeyframe1.SetIsVisible(true);
    lPieceAnimation.AddKeyframe(lPieceAnimationKeyframe1);

    Pht::Keyframe lPieceAnimationKeyframe2 {1.0f};
    lPieceAnimationKeyframe2.SetPosition(lPieceInitialPosition);
    lPieceAnimation.AddKeyframe(lPieceAnimationKeyframe2);

    Pht::Keyframe lPieceAnimationKeyframe3 {1.3f};
    lPieceAnimationKeyframe3.SetPosition({-0.5f, -2.5f, UiLayer::block});
    lPieceAnimation.AddKeyframe(lPieceAnimationKeyframe3);

    Pht::Keyframe lPieceAnimationKeyframe4 {rowClearTime};
    lPieceAnimationKeyframe4.SetIsVisible(false);
    lPieceAnimation.AddKeyframe(lPieceAnimationKeyframe4);

    lPieceAnimation.AddKeyframe(Pht::Keyframe {animationDuration});
    
    auto& remainingLPieceAnimation {animationSystem.CreateAnimation(remainingLPiece)};
    
    Pht::Keyframe remainingLPieceKeyframe1 {0.0f};
    remainingLPieceKeyframe1.SetIsVisible(false);
    remainingLPieceKeyframe1.SetPosition(remainingLPieceInitialPosition);
    remainingLPieceAnimation.AddKeyframe(remainingLPieceKeyframe1);

    Pht::Keyframe remainingLPieceKeyframe2 {rowClearTime};
    remainingLPieceKeyframe2.SetIsVisible(true);
    remainingLPieceAnimation.AddKeyframe(remainingLPieceKeyframe2);

    Pht::Keyframe remainingLPieceKeyframe3 {rowClearTime + fallWaitDuration};
    remainingLPieceKeyframe3.SetPosition(remainingLPieceInitialPosition);
    remainingLPieceAnimation.AddKeyframe(remainingLPieceKeyframe3);

    Pht::Keyframe remainingLPieceKeyframe4 {rowClearTime + fallWaitDuration + fallDuration};
    remainingLPieceKeyframe4.SetPosition({-0.5f, -3.0f, UiLayer::block});
    remainingLPieceAnimation.AddKeyframe(remainingLPieceKeyframe4);

    remainingLPieceAnimation.AddKeyframe(Pht::Keyframe {animationDuration});
    
    auto& redBlocksAnimation {animationSystem.CreateAnimation(redBlocks)};
    
    Pht::Keyframe redBlocksKeyframe1 {0.0f};
    redBlocksKeyframe1.SetPosition(redBlocksInitialPosition);
    redBlocksAnimation.AddKeyframe(redBlocksKeyframe1);

    Pht::Keyframe redBlocksKeyframe2 {rowClearTime + fallWaitDuration};
    redBlocksKeyframe2.SetPosition(redBlocksInitialPosition);
    redBlocksAnimation.AddKeyframe(redBlocksKeyframe2);

    Pht::Keyframe redBlocksKeyframe3 {rowClearTime + fallWaitDuration + fallDuration};
    redBlocksKeyframe3.SetPosition({2.5f, -3.0f, UiLayer::block});
    redBlocksAnimation.AddKeyframe(redBlocksKeyframe3);

    redBlocksAnimation.AddKeyframe(Pht::Keyframe {animationDuration});

    auto& leftGrayBlocksAnimation {animationSystem.CreateAnimation(leftGrayBlocks)};
    
    Pht::Keyframe leftGrayBlocksKeyframe1 {0.0f};
    leftGrayBlocksKeyframe1.SetIsVisible(true);
    leftGrayBlocksAnimation.AddKeyframe(leftGrayBlocksKeyframe1);
    
    Pht::Keyframe leftGrayBlocksKeyframe2 {rowClearTime};
    leftGrayBlocksKeyframe2.SetIsVisible(false);
    leftGrayBlocksAnimation.AddKeyframe(leftGrayBlocksKeyframe2);
    
    leftGrayBlocksAnimation.AddKeyframe(Pht::Keyframe {animationDuration});

    auto& rightGrayBlocksAnimation {animationSystem.CreateAnimation(rightGrayBlocks)};
    
    Pht::Keyframe rightGrayBlocksKeyframe1 {0.0f};
    rightGrayBlocksKeyframe1.SetIsVisible(true);
    rightGrayBlocksAnimation.AddKeyframe(rightGrayBlocksKeyframe1);
    
    Pht::Keyframe rightGrayBlocksKeyframe2 {rowClearTime};
    rightGrayBlocksKeyframe2.SetIsVisible(false);
    rightGrayBlocksAnimation.AddKeyframe(rightGrayBlocksKeyframe2);
    
    rightGrayBlocksAnimation.AddKeyframe(Pht::Keyframe {animationDuration});

    rootAnimation.Play();
}

Pht::SceneObject& HowToPlayDialogView::CreateLPiece(const Pht::Vec3& position,
                                                    Pht::SceneObject& parent,
                                                    const PieceResources& pieceResources) {
    auto& lPiece {CreateSceneObject()};
    parent.AddChild(lPiece);
    
    lPiece.GetTransform().SetPosition(position);
    
    auto& blockRenderable {
        pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                BlockColor::Yellow,
                                                BlockBrightness::Normal)
    };

    auto halfCellSize {0.5f};
    CreateBlock({-halfCellSize, halfCellSize, 0.0f}, blockRenderable, lPiece);
    CreateBlock({halfCellSize, -halfCellSize, 0.0f}, blockRenderable, lPiece);
    CreateBlock({halfCellSize, halfCellSize, 0.0f}, blockRenderable, lPiece);
    
    auto& weldRenderable {
        pieceResources.GetWeldRenderableObject(WeldRenderableKind::Normal,
                                               BlockColor::Yellow,
                                               BlockBrightness::Normal)
    };

    CreateWeld({0.0f, halfCellSize, halfCellSize}, weldRenderable, 0.0f, lPiece);
    CreateWeld({halfCellSize, 0.0f, halfCellSize}, weldRenderable, 90.0f, lPiece);
    
    return lPiece;
}

Pht::SceneObject& HowToPlayDialogView::CreateTwoBlocks(const Pht::Vec3& position,
                                                       BlockColor color,
                                                       Pht::SceneObject& parent,
                                                       const PieceResources& pieceResources) {
    auto& blocks {CreateSceneObject()};
    parent.AddChild(blocks);
    
    blocks.GetTransform().SetPosition(position);
    
    auto& blockRenderable {
        pieceResources.GetBlockRenderableObject(BlockKind::Full, color, BlockBrightness::Normal)
    };

    auto halfCellSize {0.5f};
    CreateBlock({-halfCellSize, 0.0f, 0.0f}, blockRenderable, blocks);
    CreateBlock({halfCellSize, 0.0f, 0.0f}, blockRenderable, blocks);
    
    auto& weldRenderable {
        pieceResources.GetWeldRenderableObject(WeldRenderableKind::Normal,
                                               color,
                                               BlockBrightness::Normal)
    };

    CreateWeld({0.0f, 0.0f, halfCellSize}, weldRenderable, 0.0f, blocks);
    
    return blocks;
}

Pht::SceneObject& HowToPlayDialogView::CreateThreeGrayBlocks(const Pht::Vec3& position,
                                                             Pht::SceneObject& parent,
                                                             const LevelResources& levelResources) {
    auto& blocks {CreateSceneObject()};
    parent.AddChild(blocks);
    
    blocks.GetTransform().SetPosition(position);
    
    auto& blockRenderable {levelResources.GetLevelBlockRenderable(BlockKind::Full)};

    auto halfCellSize {0.5f};
    CreateBlock({-halfCellSize * 2.0f, 0.0f, 0.0f}, blockRenderable, blocks);
    CreateBlock({0.0f, 0.0f, 0.0f}, blockRenderable, blocks);
    CreateBlock({halfCellSize * 2.0f, 0.0f, 0.0f}, blockRenderable, blocks);
    
    return blocks;
}

Pht::SceneObject&
HowToPlayDialogView::CreateThreeGrayBlocksWithGap(const Pht::Vec3& position,
                                                  Pht::SceneObject& parent,
                                                  const LevelResources& levelResources) {
    auto& blocks {CreateSceneObject()};
    parent.AddChild(blocks);
    
    blocks.GetTransform().SetPosition(position);
    
    auto& blockRenderable {levelResources.GetLevelBlockRenderable(BlockKind::Full)};

    auto halfCellSize {0.5f};
    CreateBlock({-halfCellSize * 3.0f, 0.0f, 0.0f}, blockRenderable, blocks);
    CreateBlock({halfCellSize, 0.0f, 0.0f}, blockRenderable, blocks);
    CreateBlock({halfCellSize * 3.0f, 0.0f, 0.0f}, blockRenderable, blocks);
    
    return blocks;
}

void HowToPlayDialogView::CreateBlock(const Pht::Vec3& position,
                                      Pht::RenderableObject& blockRenderable,
                                      Pht::SceneObject& parent) {
    auto& block {CreateSceneObject()};
    auto& transform {block.GetTransform()};
    transform.SetPosition(position);
    transform.SetScale(0.8f);
    block.SetRenderable(&blockRenderable);
    parent.AddChild(block);
}

void HowToPlayDialogView::CreateWeld(const Pht::Vec3& position,
                                     Pht::RenderableObject& weldRenderable,
                                     float rotation,
                                     Pht::SceneObject& parent) {
    auto& weld {CreateSceneObject()};
    auto& transform {weld.GetTransform()};
    transform.SetPosition(position);
    transform.SetScale(0.8f);
    transform.SetRotation({0.0f, 0.0f, rotation});
    weld.SetRenderable(&weldRenderable);
    parent.AddChild(weld);
}

void HowToPlayDialogView::SetUp() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetGuiLightDirections(lightDirectionA, lightDirectionB);
    }
}

void HowToPlayDialogView::Update() {
    if (mAnimation) {
        auto dt {mEngine.GetLastFrameSeconds()};
        mAnimation->Update(dt);
    }
}

void HowToPlayDialogView::OnDeactivate() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}

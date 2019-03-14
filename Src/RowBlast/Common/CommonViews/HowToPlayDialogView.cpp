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
    
    MenuButton::Style buttonStyle;
    buttonStyle.mPressedScale = 1.05f;
    buttonStyle.mRenderableObject = &guiResources.GetEvenSmallerBlueGlossyButton(zoom);
    buttonStyle.mSelectedRenderableObject = &guiResources.GetEvenSmallerDarkBlueGlossyButton(zoom);

    Pht::Vec3 previousButtonPosition {
        -GetSize().x / 2.0f + 1.6f,
        -GetSize().y / 2.0f + 1.2f,
        UiLayer::textRectangle
    };
    
    Pht::Vec2 buttonInputSize {60.0f, 60.0f};
    
    mPreviousButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   previousButtonPosition,
                                                   buttonInputSize,
                                                   buttonStyle);
    mPreviousButton->CreateIcon("previous.png",
                                {0.0f, 0.0f, UiLayer::buttonText},
                                {0.9f, 0.9f},
                                {1.0f, 1.0f, 1.0f, 1.0f},
                                Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                                Pht::Vec3 {-0.05f, -0.05f, -0.1f});

    Pht::Vec3 nextButtonPosition {
        GetSize().x / 2.0f - 1.6f,
        -GetSize().y / 2.0f + 1.2f,
        UiLayer::textRectangle
    };

    mNextButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               nextButtonPosition,
                                               buttonInputSize,
                                               buttonStyle);
    mNextButton->CreateIcon("play.png",
                            {0.0f, 0.0f, UiLayer::buttonText},
                            {0.9f, 0.9f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                            Pht::Vec3 {-0.05f, -0.05f, -0.1f});
        
    for (auto i {0}; i < mNumPages; ++i) {
        GetRoot().AddChild(CreateFilledCircleIcon(i, false));
    }

    CreateGoalPage(guiResources, pieceResources, levelResources, zoom);
    CreateControlsPage(guiResources, zoom);
}

void HowToPlayDialogView::CreateGoalPage(const GuiResources& guiResources,
                                         const PieceResources& pieceResources,
                                         const LevelResources& levelResources,
                                         PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    GetRoot().AddChild(container);
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-1.5f, 8.25f, UiLayer::text}, "GOAL", largeTextProperties, container);

    auto& animation {CreateClearBlocksAnimation(container, pieceResources, levelResources)};

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-5.65f, -5.4f, UiLayer::text}, "Usually the goal is to clear blocks", textProperties, container);
    CreateText({-4.35f, -6.475f, UiLayer::text}, "by filling horizontal rows.", textProperties, container);
    
    container.AddChild(CreateFilledCircleIcon(static_cast<int>(mPages.size()), true));
    
    mPages.push_back(Page {container, &animation});
}

void HowToPlayDialogView::CreateControlsPage(const GuiResources& guiResources,
                                             PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    GetRoot().AddChild(container);
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-2.45f, 8.25f, UiLayer::text}, "CONTROLS", largeTextProperties, container);
    
    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-5.3f, 6.0f, UiLayer::text}, "The game can be played using", textProperties, container);
    CreateText({-3.3f, 4.3f, UiLayer::text}, "SingleTap controls, or", textProperties, container);
    CreateText({-3.3f, 2.6f, UiLayer::text}, "Swipe controls", textProperties, container);
    
    CreateSingleTapIcon({-4.3f, 4.5f, UiLayer::text}, container);
    CreateSwipeIcon({-4.2f, 2.8f, UiLayer::text}, container);
    
    CreateText({-5.3f, 0.9f, UiLayer::text}, "The control type can be changed", textProperties, container);
    CreateText({-5.3f, -0.175f, UiLayer::text}, "in the options menu.", textProperties, container);
    
    container.AddChild(CreateFilledCircleIcon(static_cast<int>(mPages.size()), true));
    
    mPages.push_back(Page {container, nullptr});
}

Pht::SceneObject& HowToPlayDialogView::CreateFilledCircleIcon(int index, bool isBright) {
    auto indexMax {mNumPages - 1};
    auto diff {0.75f};
    
    Pht::Vec3 position {
        -indexMax * diff / 2.0f + index * diff,
        -GetSize().y / 2.0f + 1.2f,
        isBright ? UiLayer::root : UiLayer::textRectangle
    };
    
    auto brightness {isBright ? 0.95f : 0.4f};
    Pht::Vec4 color {brightness, brightness, brightness, 1.0f};
    Pht::Material iconMaterial {"filled_circle.png", 0.0f, 0.0f, 0.0f, 0.0f};
    iconMaterial.SetBlend(Pht::Blend::Yes);
    iconMaterial.SetOpacity(color.w);
    iconMaterial.SetAmbient(Pht::Color{color.x, color.y, color.z});
    
    auto& iconSceneObject {
        CreateSceneObject(Pht::QuadMesh {0.35f, 0.35f}, iconMaterial, mEngine.GetSceneManager())
    };
    
    iconSceneObject.GetTransform().SetPosition(position);
    
    return iconSceneObject;
}

Pht::Animation& HowToPlayDialogView::CreateClearBlocksAnimation(Pht::SceneObject& parent,
                                                                const PieceResources& pieceResources,
                                                                const LevelResources& levelResources) {
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition({0.0f, 1.5f, 0.0f});
    container.GetTransform().SetScale(1.15f);
    parent.AddChild(container);
    
    CreateFieldQuad(container);
    
    auto& animationSystem {mEngine.GetAnimationSystem()};
    auto& rootAnimation {animationSystem.CreateAnimation(container)};
    rootAnimation.AddKeyframe(Pht::Keyframe {0.0f});
    rootAnimation.AddKeyframe(Pht::Keyframe {4.0f});
    
    Pht::Vec3 lPieceInitialPosition {-0.5f, 3.3f, UiLayer::block};
    auto& lPiece {CreateLPiece(lPieceInitialPosition, container, pieceResources)};
    Pht::Vec3 remainingLPieceInitialPosition {-0.5f, -2.0f, UiLayer::block};
    auto& remainingLPiece {CreateTwoBlocks(remainingLPieceInitialPosition, BlockColor::Yellow, container, pieceResources)};
    remainingLPiece.SetIsVisible(false);
    Pht::Vec3 greenBlocksInitialPosition {2.5f, -2.0f, UiLayer::block};
    auto& greenBlocks {CreateTwoBlocks(greenBlocksInitialPosition, BlockColor::Green, container, pieceResources)};
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
    
    auto& greenBlocksAnimation {animationSystem.CreateAnimation(greenBlocks)};
    
    Pht::Keyframe greenBlocksKeyframe1 {0.0f};
    greenBlocksKeyframe1.SetPosition(greenBlocksInitialPosition);
    greenBlocksAnimation.AddKeyframe(greenBlocksKeyframe1);

    Pht::Keyframe greenBlocksKeyframe2 {rowClearTime + fallWaitDuration};
    greenBlocksKeyframe2.SetPosition(greenBlocksInitialPosition);
    greenBlocksAnimation.AddKeyframe(greenBlocksKeyframe2);

    Pht::Keyframe greenBlocksKeyframe3 {rowClearTime + fallWaitDuration + fallDuration};
    greenBlocksKeyframe3.SetPosition({2.5f, -3.0f, UiLayer::block});
    greenBlocksAnimation.AddKeyframe(greenBlocksKeyframe3);

    greenBlocksAnimation.AddKeyframe(Pht::Keyframe {animationDuration});

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

    return rootAnimation;
}

void HowToPlayDialogView::CreateFieldQuad(Pht::SceneObject& parent) {
    Pht::Material fieldMaterial;
    fieldMaterial.SetOpacity(0.93f);
    
    auto width {7.0f};
    auto height {9.0f};
    auto f {0.9f};
    
    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.3f * f, 0.3f * f, 0.752f * f, 1.0f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.8f * f, 0.225f * f, 0.425f * f, 1.0f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.3f * f, 0.3f * f, 0.752f * f, 1.0f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.81f * f, 0.225f * f, 0.425f * f, 1.0f}},
    };

    auto& fieldQuad {
        CreateSceneObject(Pht::QuadMesh {vertices}, fieldMaterial, mEngine.GetSceneManager())
    };
    
    fieldQuad.GetTransform().SetPosition({0.0f, 0.0f, UiLayer::panel});
    parent.AddChild(fieldQuad);
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

void HowToPlayDialogView::CreateIcon(const std::string& filename,
                                     const Pht::Vec3& position,
                                     const Pht::Vec2& size,
                                     Pht::SceneObject& parent) {
    Pht::Vec4 color {0.95f, 0.95f, 0.95f, 1.0f};
    Pht::Material iconMaterial {filename, 0.0f, 0.0f, 0.0f, 0.0f};
    iconMaterial.SetBlend(Pht::Blend::Yes);
    iconMaterial.SetOpacity(color.w);
    iconMaterial.SetAmbient(Pht::Color{color.x, color.y, color.z});
    
    auto& iconSceneObject {
        CreateSceneObject(Pht::QuadMesh {size.x, size.y}, iconMaterial, mEngine.GetSceneManager())
    };
    
    iconSceneObject.GetTransform().SetPosition(position);
    parent.AddChild(iconSceneObject);
}

void HowToPlayDialogView::CreateSingleTapIcon(const Pht::Vec3& position, Pht::SceneObject& parent) {
    auto& icon {CreateSceneObject()};
    icon.GetTransform().SetPosition(position);
    parent.AddChild(icon);

    CreateIcon("hand.png", {0.0f, 0.03f, 0.0f}, {0.9f, 0.9f}, icon);
    CreateIcon("circle.png", {-0.03f, 0.42f, 0.0f}, {0.36f, 0.36f}, icon);
    CreateIcon("circle.png", {-0.03f, 0.42f, 0.0f}, {0.42f, 0.42f}, icon);
}

void HowToPlayDialogView::CreateSwipeIcon(const Pht::Vec3& position, Pht::SceneObject& parent) {
    auto& icon {CreateSceneObject()};
    icon.GetTransform().SetPosition(position);
    parent.AddChild(icon);

    CreateIcon("hand.png", {0.0, 0.03f, 0.0f}, {0.9f, 0.9f}, icon);
    CreateIcon("back.png", {-0.4f, 0.4f, 0.0f}, {0.42f, 0.42f}, icon);
    CreateIcon("right_arrow.png", {0.35f, 0.4f, 0.0f}, {0.42f, 0.42f}, icon);
}

void HowToPlayDialogView::SetUp() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetGuiLightDirections(lightDirectionA, lightDirectionB);
    }
    
    SetPage(0);
}

void HowToPlayDialogView::GoToNextPage() {
    if (mPageIndex < static_cast<int>(mPages.size()) - 1) {
        mPageIndex++;
        SetPage(mPageIndex);
    }
}

void HowToPlayDialogView::GoToPreviousPage() {
    if (mPageIndex > 0) {
        mPageIndex--;
        SetPage(mPageIndex);
    }
}

void HowToPlayDialogView::SetPage(int pageIndex) {
    for (auto& page: mPages) {
        page.mSceneObject.SetIsVisible(false);
        if (page.mAnimation) {
            page.mAnimation->Stop();
        }
    }
    
    assert(pageIndex < mPages.size());
    mPageIndex = pageIndex;
    
    auto& page {mPages[mPageIndex]};
    page.mSceneObject.SetIsVisible(true);
    if (page.mAnimation) {
        page.mAnimation->Play();
    }
}

void HowToPlayDialogView::Update() {
    auto& page {mPages[mPageIndex]};
    if (page.mAnimation) {
        auto dt {mEngine.GetLastFrameSeconds()};
        page.mAnimation->Update(dt);
    }
}

void HowToPlayDialogView::OnDeactivate() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}

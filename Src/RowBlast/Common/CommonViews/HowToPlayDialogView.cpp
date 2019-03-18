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
                                {-0.02f, 0.02f, UiLayer::buttonText},
                                {0.85f, 0.85f},
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
                            {0.02f, 0.02f, UiLayer::buttonText},
                            {0.85f, 0.85f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                            Pht::Vec3 {-0.05f, -0.05f, -0.1f});
        
    for (auto i {0}; i < mNumPages; ++i) {
        GetRoot().AddChild(CreateFilledCircleIcon(i, false));
    }

    CreateGoalPage(guiResources, pieceResources, levelResources, zoom);
    CreateControlsPage(guiResources, zoom);
    CreatePlacePiecePage(guiResources, pieceResources, levelResources, zoom);
    CreateOtherMovesPage(guiResources, pieceResources, levelResources, zoom);
    CreateSwitchPiecePage(commonResources, pieceResources, levelResources, zoom);
    CreateMovePiecePage(guiResources, pieceResources, levelResources, zoom);
    CreateRotatePiecePage(guiResources, pieceResources, levelResources, zoom);
    CreateDropPiecePage(guiResources, pieceResources, levelResources, zoom);
    CreateDragPieceDownPage(guiResources, pieceResources, levelResources, zoom);
}

void HowToPlayDialogView::CreateGoalPage(const GuiResources& guiResources,
                                         const PieceResources& pieceResources,
                                         const LevelResources& levelResources,
                                         PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    GetRoot().AddChild(container);
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-1.5f, 8.25f, UiLayer::text}, "GOAL", largeTextProperties, container);

    auto& animation {
        CreateClearBlocksAnimation(container,
                                   ClearBlocksChildAnimations {},
                                   pieceResources,
                                   levelResources,
                                   nullptr)
    };

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
    
    CreateSingleTapIcon({-4.3f, 4.55f, UiLayer::text}, container);
    CreateSwipeIcon({-4.2f, 2.75f, UiLayer::text}, container);
    
    CreateText({-5.3f, 0.9f, UiLayer::text}, "The control type can be changed", textProperties, container);
    CreateText({-5.3f, -0.175f, UiLayer::text}, "in the options menu.", textProperties, container);
    
    container.AddChild(CreateFilledCircleIcon(static_cast<int>(mPages.size()), true));
    
    mPages.push_back(Page {container});
}

void HowToPlayDialogView::CreatePlacePiecePage(const GuiResources& guiResources,
                                               const PieceResources& pieceResources,
                                               const LevelResources& levelResources,
                                               PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    GetRoot().AddChild(container);
    
    CreateSingleTapIcon({-3.8f, 8.5f, UiLayer::text}, container);
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-3.1f, 8.25f, UiLayer::text}, "SINGLE TAP (1)", largeTextProperties, container);

    auto handAnimation {std::make_unique<HandAnimation>(mEngine, 1.0f, true)};
    
    auto& animation {
        CreateClearBlocksAnimation(container,
                                   ClearBlocksChildAnimations {.mPlacePiece = true},
                                   pieceResources,
                                   levelResources,
                                   handAnimation.get())
    };

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-5.2f, -5.4f, UiLayer::text}, "When using SingleTap controls,", textProperties, container);
    CreateText({-5.6f, -6.475f, UiLayer::text}, "just tap a move to place the piece.", textProperties, container);
    
    container.AddChild(CreateFilledCircleIcon(static_cast<int>(mPages.size()), true));
    
    mPages.push_back(Page {container, &animation, std::move(handAnimation)});
}

void HowToPlayDialogView::CreateOtherMovesPage(const GuiResources& guiResources,
                                               const PieceResources& pieceResources,
                                               const LevelResources& levelResources,
                                               PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    GetRoot().AddChild(container);
    
    CreateSingleTapIcon({-4.0f, 8.5f, UiLayer::text}, container);
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-3.3f, 8.25f, UiLayer::text}, "SINGLE TAP (2)", largeTextProperties, container);

    auto handAnimation {std::make_unique<HandAnimation>(mEngine, 1.0f, true)};
    
    auto& animation {
        CreateBlocksAnimation(container,
                              BlocksChildAnimations {.mOtherMoves = true},
                              pieceResources,
                              levelResources,
                              *handAnimation,
                              4.0f)
    };

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-5.45f, -5.4f, UiLayer::text}, "With SingleTap controls, just tap", textProperties, container);
    CreateText({-5.05f, -6.475f, UiLayer::text}, "the screen to find more moves.", textProperties, container);
    
    container.AddChild(CreateFilledCircleIcon(static_cast<int>(mPages.size()), true));
    
    mPages.push_back(Page {container, &animation, std::move(handAnimation)});
}

void HowToPlayDialogView::CreateSwitchPiecePage(const CommonResources& commonResources,
                                                const PieceResources& pieceResources,
                                                const LevelResources& levelResources,
                                                PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    GetRoot().AddChild(container);
    
    auto& guiResources {commonResources.GetGuiResources()};
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-3.2f, 8.25f, UiLayer::text}, "SWITCH PIECE", largeTextProperties, container);

    auto handAnimation {std::make_unique<HandAnimation>(mEngine, 1.0f, true)};
    
    auto& animation {
        CreateSwitchPieceAnimation(container,
                                   commonResources,
                                   pieceResources,
                                   levelResources,
                                   *handAnimation)
    };

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.05f, -5.4f, UiLayer::text}, "Tap the switch button or", textProperties, container);
    CreateText({-4.05f, -6.475f, UiLayer::text}, "swipe up to switch piece.", textProperties, container);
    
    container.AddChild(CreateFilledCircleIcon(static_cast<int>(mPages.size()), true));
    
    mPages.push_back(Page {container, &animation, std::move(handAnimation)});
}

void HowToPlayDialogView::CreateMovePiecePage(const GuiResources& guiResources,
                                              const PieceResources& pieceResources,
                                              const LevelResources& levelResources,
                                              PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    GetRoot().AddChild(container);
    
    CreateSwipeIcon({-2.65f, 8.5f, UiLayer::text}, container);
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-1.75f, 8.25f, UiLayer::text}, "SWIPE (1)", largeTextProperties, container);

    auto handAnimation {std::make_unique<HandAnimation>(mEngine, 1.0f, true)};
    
    auto& animation {
        CreateBlocksAnimation(container,
                              BlocksChildAnimations {.mMovePieceSideways = true},
                              pieceResources,
                              levelResources,
                              *handAnimation,
                              6.0f)
    };

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-5.1f, -5.4f, UiLayer::text}, "With swipe controls, slide your", textProperties, container);
    CreateText({-5.6f, -6.475f, UiLayer::text}, "finger to move the piece sideways.", textProperties, container);
    
    container.AddChild(CreateFilledCircleIcon(static_cast<int>(mPages.size()), true));
    
    mPages.push_back(Page {container, &animation, std::move(handAnimation)});
}

void HowToPlayDialogView::CreateRotatePiecePage(const GuiResources& guiResources,
                                                const PieceResources& pieceResources,
                                                const LevelResources& levelResources,
                                                PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    GetRoot().AddChild(container);
    
    CreateSwipeIcon({-2.8f, 8.5f, UiLayer::text}, container);
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-1.9f, 8.25f, UiLayer::text}, "SWIPE (2)", largeTextProperties, container);

    auto handAnimation {std::make_unique<HandAnimation>(mEngine, 1.0f, true)};
    
    auto& animation {
        CreateBlocksAnimation(container,
                              BlocksChildAnimations {.mRotatePiece = true},
                              pieceResources,
                              levelResources,
                              *handAnimation,
                              6.0f)
    };

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.65f, -5.4f, UiLayer::text}, "With swipe controls, tap the", textProperties, container);
    CreateText({-4.2f, -6.475f, UiLayer::text}, "screen to rotate the piece.", textProperties, container);
    
    container.AddChild(CreateFilledCircleIcon(static_cast<int>(mPages.size()), true));
    
    mPages.push_back(Page {container, &animation, std::move(handAnimation)});
}

void HowToPlayDialogView::CreateDropPiecePage(const GuiResources& guiResources,
                                              const PieceResources& pieceResources,
                                              const LevelResources& levelResources,
                                              PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    GetRoot().AddChild(container);
    
    CreateSwipeIcon({-2.8f, 8.5f, UiLayer::text}, container);
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-1.9f, 8.25f, UiLayer::text}, "SWIPE (3)", largeTextProperties, container);

    auto handAnimation {std::make_unique<HandAnimation>(mEngine, 1.0f, true)};
    
    auto& animation {
        CreateClearBlocksAnimation(container,
                                   ClearBlocksChildAnimations {.mDropPiece = true},
                                   pieceResources,
                                   levelResources,
                                   handAnimation.get())
    };

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.4f, -5.4f, UiLayer::text}, "With swipe controls, swipe", textProperties, container);
    CreateText({-3.9f, -6.475f, UiLayer::text}, "down to drop the piece.", textProperties, container);
    
    container.AddChild(CreateFilledCircleIcon(static_cast<int>(mPages.size()), true));
    
    mPages.push_back(Page {container, &animation, std::move(handAnimation)});
}

void HowToPlayDialogView::CreateDragPieceDownPage(const GuiResources& guiResources,
                                                  const PieceResources& pieceResources,
                                                  const LevelResources& levelResources,
                                                  PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    GetRoot().AddChild(container);
    
    CreateSwipeIcon({-2.8f, 8.5f, UiLayer::text}, container);
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-1.9f, 8.25f, UiLayer::text}, "SWIPE (4)", largeTextProperties, container);

    auto handAnimation {std::make_unique<HandAnimation>(mEngine, 1.0f, true)};
    
    auto& animation {
        CreateMovePieceDownAnimation(container, pieceResources, levelResources, *handAnimation)
    };

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.85f, -4.7f, UiLayer::text}, "To move the piece down a bit,", textProperties, container);
    CreateText({-5.35f, -5.775f, UiLayer::text}, "slide the finger down and swipe", textProperties, container);
    CreateText({-5.35f, -6.85f, UiLayer::text}, "up without releasing the screen.", textProperties, container);
    
    container.AddChild(CreateFilledCircleIcon(static_cast<int>(mPages.size()), true));
    
    mPages.push_back(Page {container, &animation, std::move(handAnimation)});
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
                                                                const ClearBlocksChildAnimations& childAnimations,
                                                                const PieceResources& pieceResources,
                                                                const LevelResources& levelResources,
                                                                HandAnimation* handAnimation) {
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition({0.0f, 1.5f, 0.0f});
    container.GetTransform().SetScale(1.15f);
    parent.AddChild(container);
    
    CreateFieldQuad(container);
    
    auto rowClearTime {1.6f};
    auto fallWaitDuration {0.35f};
    auto fallDuration {0.2f};
    auto animationDuration {4.0f};

    auto& animationSystem {mEngine.GetAnimationSystem()};
    auto& rootAnimation {
        animationSystem.CreateAnimation(container, {{.mTime = 0.0f}, {.mTime = animationDuration}})
    };
    
    if (handAnimation) {
        handAnimation->Init(container);
    }
    
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
    
    std::vector<Pht::Keyframe> lPieceAnimationKeyframes {
        {.mTime = 0.0f, .mPosition = lPieceInitialPosition, .mIsVisible = true},
        {.mTime = 1.0f, .mPosition = lPieceInitialPosition},
        {.mTime = 1.3f, .mPosition = Pht::Vec3{-0.5f, -2.5f, UiLayer::block}},
        {.mTime = rowClearTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(lPiece, lPieceAnimationKeyframes);

    std::vector<Pht::Keyframe> remainingLPieceKeyframes {
        {.mTime = 0.0f, .mPosition = remainingLPieceInitialPosition, .mIsVisible = false},
        {.mTime = rowClearTime, .mIsVisible = true},
        {.mTime = rowClearTime + fallWaitDuration, .mPosition = remainingLPieceInitialPosition},
        {.mTime = rowClearTime + fallWaitDuration + fallDuration, .mPosition = Pht::Vec3{-0.5f, -3.0f, UiLayer::block}},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(remainingLPiece, remainingLPieceKeyframes);

    std::vector<Pht::Keyframe> greenBlocksKeyframes {
        {.mTime = 0.0f, .mPosition = greenBlocksInitialPosition},
        {.mTime = rowClearTime + fallWaitDuration, .mPosition = greenBlocksInitialPosition},
        {.mTime = rowClearTime + fallWaitDuration + fallDuration, .mPosition = Pht::Vec3{2.5f, -3.0f, UiLayer::block}},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(greenBlocks, greenBlocksKeyframes);

    std::vector<Pht::Keyframe> leftGrayBlocksKeyframes {
        {.mTime = 0.0f, .mIsVisible = true},
        {.mTime = rowClearTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(leftGrayBlocks, leftGrayBlocksKeyframes);

    std::vector<Pht::Keyframe> rightGrayBlocksKeyframes {
        {.mTime = 0.0f, .mIsVisible = true},
        {.mTime = rowClearTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(rightGrayBlocks, rightGrayBlocksKeyframes);

    if (childAnimations.mPlacePiece && handAnimation) {
        auto& ghostPieces {CreateSceneObject()};
        container.AddChild(ghostPieces);
        
        CreateLPieceGhostPiece({-0.5f, -2.5f, UiLayer::block}, 90.0f, ghostPieces, levelResources);
        CreateLPieceGhostPiece({2.5f, -0.5f, UiLayer::block}, 0.0f, ghostPieces, levelResources);
        CreateLPieceGhostPiece({-2.5f, -1.5f, UiLayer::block}, 0.0f, ghostPieces, levelResources);

        std::vector<Pht::Keyframe> ghostPiecesKeyframes {
            {.mTime = 0.0f, .mIsVisible = true},
            {.mTime = 1.0f, .mIsVisible = false},
            {.mTime = animationDuration}
        };
        animationSystem.CreateAnimation(ghostPieces, ghostPiecesKeyframes);
        
        std::vector<Pht::Keyframe> handAnimationKeyframes {
            {
                .mTime = 0.0f,
                .mCallback = [handAnimation] () {
                    handAnimation->StartInNotTouchingScreenState({0.5f, -2.5f, UiLayer::root},
                                                                 45.0f,
                                                                 10.0f);
                }
            },
            {
                .mTime = 0.75f,
                .mCallback = [handAnimation] () {
                    handAnimation->BeginTouch(0.0f);
                }
            },
            {
                .mTime = animationDuration
            }
        };
        
        animationSystem.CreateAnimation(handAnimation->GetSceneObject(), handAnimationKeyframes);
    }
    
    if (childAnimations.mDropPiece && handAnimation) {
        Pht::Vec3 handInitialPosition {2.25f, 1.0f, UiLayer::root};
        Pht::Vec3 handAfterSwipePosition {2.25f, -1.3f, UiLayer::root};
        
        std::vector<Pht::Keyframe> handAnimationKeyframes {
            {
                .mTime = 0.0f,
                .mPosition = handInitialPosition,
                .mCallback = [handAnimation, &handInitialPosition] () {
                    handAnimation->StartInNotTouchingScreenState(handInitialPosition, 45.0f, 10.0f);
                }
            },
            {
                .mTime = 0.4f,
                .mPosition = handInitialPosition,
                .mCallback = [handAnimation] () {
                    handAnimation->BeginTouch(0.45f);
                }
            },
            {
                .mTime = 0.6f,
                .mPosition = handInitialPosition
            },
            {
                .mTime = 1.0f,
                .mPosition = handAfterSwipePosition
            },
            {
                .mTime = 1.5f,
                .mPosition = handAfterSwipePosition
            },
            {
                .mTime = 3.0f,
                .mPosition = handInitialPosition
            },
            {
                .mTime = animationDuration
            }
        };
        
        auto& handPhtAnimation {
            animationSystem.CreateAnimation(handAnimation->GetSceneObject(), handAnimationKeyframes)
        };
        handPhtAnimation.SetInterpolation(Pht::Interpolation::Cosine);
    }

    return rootAnimation;
}

Pht::Animation& HowToPlayDialogView::CreateBlocksAnimation(Pht::SceneObject& parent,
                                                           const BlocksChildAnimations& childAnimations,
                                                           const PieceResources& pieceResources,
                                                           const LevelResources& levelResources,
                                                           HandAnimation& handAnimation,
                                                           float animationDuration) {
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition({0.0f, 1.5f, 0.0f});
    container.GetTransform().SetScale(1.15f);
    parent.AddChild(container);
    
    CreateFieldQuad(container);

    auto& animationSystem {mEngine.GetAnimationSystem()};
    auto& rootAnimation {
        animationSystem.CreateAnimation(container, {{.mTime = 0.0f}, {.mTime = animationDuration}})
    };
    
    handAnimation.Init(container);
    
    Pht::Vec3 lPieceInitialPosition {-0.5f, 3.3f, UiLayer::block};
    
    auto& lPiece {CreateLPiece(lPieceInitialPosition, container, pieceResources)};
    CreateTwoBlocks({2.5f, -2.0f, UiLayer::block}, BlockColor::Green, container, pieceResources);
    CreateThreeGrayBlocks({-2.0f, -3.0f, UiLayer::block}, container, levelResources);
    CreateThreeGrayBlocks({2.0f, -3.0f, UiLayer::block}, container, levelResources);
    CreateThreeGrayBlocksWithGap({-1.5f, -4.0f, UiLayer::block}, container, levelResources);
    CreateThreeGrayBlocks({2.0f, -4.0f, UiLayer::block}, container, levelResources);

    if (childAnimations.mOtherMoves) {
        auto& firstMovesSet {CreateSceneObject()};
        container.AddChild(firstMovesSet);
        
        CreateLPieceGhostPiece({-2.5f, -1.5f, UiLayer::block}, 0.0f, firstMovesSet, levelResources);
        CreateLPieceGhostPiece({-0.5f, -2.5f, UiLayer::block}, 90.0f, firstMovesSet, levelResources);
        CreateLPieceGhostPiece({2.5f, -0.5f, UiLayer::block}, 0.0f, firstMovesSet, levelResources);

        std::vector<Pht::Keyframe> firstMovesSetKeyframes {
            {.mTime = 0.0f, .mIsVisible = true},
            {.mTime = 1.0f, .mIsVisible = false},
            {.mTime = animationDuration - 0.2f, .mIsVisible = true},
            {.mTime = animationDuration}
        };
        animationSystem.CreateAnimation(firstMovesSet, firstMovesSetKeyframes);

        auto& secondMovesSet {CreateSceneObject()};
        container.AddChild(secondMovesSet);
        
        CreateLPieceGhostPiece({-2.5f, -1.5f, UiLayer::block}, 270.0f, secondMovesSet, levelResources);
        CreateLPieceGhostPiece({0.5f, -2.5f, UiLayer::block}, 180.0f, secondMovesSet, levelResources);
        CreateLPieceGhostPiece({2.5f, -0.5f, UiLayer::block}, 270.0f, secondMovesSet, levelResources);

        std::vector<Pht::Keyframe> secondMovesSetKeyframes {
            {.mTime = 0.0f, .mIsVisible = false},
            {.mTime = 1.0f, .mIsVisible = true},
            {.mTime = 2.5f, .mIsVisible = false},
            {.mTime = animationDuration}
        };
        animationSystem.CreateAnimation(secondMovesSet, secondMovesSetKeyframes);
        
        auto& thirdMovesSet {CreateSceneObject()};
        container.AddChild(thirdMovesSet);
        
        CreateLPieceGhostPiece({-1.5f, -1.5f, UiLayer::block}, 270.0f, thirdMovesSet, levelResources);
        CreateLPieceGhostPiece({1.5f, -1.5f, UiLayer::block}, 180.0f, thirdMovesSet, levelResources);

        std::vector<Pht::Keyframe> thirdMovesSetKeyframes {
            {.mTime = 0.0f, .mIsVisible = false},
            {.mTime = 2.5f, .mIsVisible = true},
            {.mTime = animationDuration - 0.2f, .mIsVisible = false},
            {.mTime = animationDuration}
        };
        animationSystem.CreateAnimation(thirdMovesSet, thirdMovesSetKeyframes);

        std::vector<Pht::Keyframe> handAnimationKeyframes {
            {
                .mTime = 0.0f,
                .mCallback = [&handAnimation] () {
                    handAnimation.StartInNotTouchingScreenState({2.0f, 2.0f, UiLayer::root},
                                                                90.0f,
                                                                10.0f);
                }
            },
            {
                .mTime = 0.75f,
                .mCallback = [&handAnimation] () {
                    handAnimation.BeginTouch(0.0f);
                }
            },
            {
                .mTime = 2.25f,
                .mCallback = [&handAnimation] () {
                    handAnimation.BeginTouch(0.0f);
                }
            },
            {
                .mTime = animationDuration - 0.4f,
                .mCallback = [&handAnimation] () {
                    handAnimation.BeginTouch(0.0f);
                }
            },
            {
                .mTime = animationDuration
            }
        };
        
        animationSystem.CreateAnimation(handAnimation.GetSceneObject(), handAnimationKeyframes);
    }
    
    if (childAnimations.mMovePieceSideways) {
        std::vector<Pht::Keyframe> lPieceAnimationKeyframes {
            {.mTime = 0.0f, .mPosition = lPieceInitialPosition},
            {.mTime = 1.0f, .mPosition = lPieceInitialPosition},
            {.mTime = 1.75f, .mPosition = Pht::Vec3{-1.5f, 3.3f, UiLayer::block}},
            {.mTime = 2.5f, .mPosition = Pht::Vec3{-2.5f, 3.3f, UiLayer::block}},
            {.mTime = 2.9f, .mPosition = Pht::Vec3{-1.5f, 3.3f, UiLayer::block}},
            {.mTime = 3.3f, .mPosition = Pht::Vec3{-0.5f, 3.3f, UiLayer::block}},
            {.mTime = 3.8f, .mPosition = Pht::Vec3{0.5f, 3.3f, UiLayer::block}},
            {.mTime = 4.5f, .mPosition = Pht::Vec3{1.5f, 3.3f, UiLayer::block}},
            {.mTime = 5.0f, .mPosition = Pht::Vec3{0.5f, 3.3f, UiLayer::block}},
            {.mTime = 5.5f, .mPosition = lPieceInitialPosition},
            {.mTime = animationDuration}
        };
        auto& lPieceAnimation {animationSystem.CreateAnimation(lPiece, lPieceAnimationKeyframes)};
        lPieceAnimation.SetInterpolation(Pht::Interpolation::None);

        Pht::Vec3 handInitialPosition {1.5f, -1.5f, UiLayer::root};
        Pht::Vec3 handLeftPosition {-1.0f, -1.5f, UiLayer::root};
        Pht::Vec3 handRightPosition {3.5f, -1.5f, UiLayer::root};
        
        std::vector<Pht::Keyframe> handAnimationKeyframes {
            {
                .mTime = 0.0f,
                .mPosition = handInitialPosition,
                .mCallback = [&handAnimation, &handInitialPosition] () {
                    handAnimation.StartInNotTouchingScreenState(handInitialPosition, 45.0f, 10.0f);
                }
            },
            {
                .mTime = 0.75f,
                .mPosition = handInitialPosition,
                .mCallback = [&handAnimation] () {
                    handAnimation.BeginTouch(4.5f);
                }
            },
            {
                .mTime = 1.0f,
                .mPosition = handInitialPosition
            },
            {
                .mTime = 2.5f,
                .mPosition = handLeftPosition
            },
            {
                .mTime = 4.5f,
                .mPosition = handRightPosition
            },
            {
                .mTime = 5.5f,
                .mPosition = handInitialPosition
            },
            {
                .mTime = animationDuration
            }
        };
        
        auto& handPhtAnimation {
            animationSystem.CreateAnimation(handAnimation.GetSceneObject(), handAnimationKeyframes)
        };
        handPhtAnimation.SetInterpolation(Pht::Interpolation::Cosine);
    }
    
    if (childAnimations.mRotatePiece) {
        std::vector<Pht::Keyframe> lPieceAnimationKeyframes {
            {.mTime = 0.0f, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
            {.mTime = 1.0f, .mRotation = Pht::Vec3{0.0f, 0.0f, -90.0f}},
            {.mTime = 2.2f, .mRotation = Pht::Vec3{0.0f, 0.0f, -180.0f}},
            {.mTime = 3.2f, .mRotation = Pht::Vec3{0.0f, 0.0f, -270.0f}},
            {.mTime = 5.2f, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
            {.mTime = animationDuration}
        };
        auto& lPieceAnimation {animationSystem.CreateAnimation(lPiece, lPieceAnimationKeyframes)};
        lPieceAnimation.SetInterpolation(Pht::Interpolation::None);

        std::vector<Pht::Keyframe> handAnimationKeyframes {
            {
                .mTime = 0.0f,
                .mCallback = [&handAnimation] () {
                    handAnimation.StartInNotTouchingScreenState({1.5f, -1.0f, UiLayer::root},
                                                                45.0f,
                                                                10.0f);
                }
            },
            {
                .mTime = 0.75f,
                .mCallback = [&handAnimation] () {
                    handAnimation.BeginTouch(0.0f);
                }
            },
            {
                .mTime = 2.0f,
                .mCallback = [&handAnimation] () {
                    handAnimation.BeginTouch(0.0f);
                }
            },
            {
                .mTime = 3.0f,
                .mCallback = [&handAnimation] () {
                    handAnimation.BeginTouch(0.0f);
                }
            },
            {
                .mTime = 5.0f,
                .mCallback = [&handAnimation] () {
                    handAnimation.BeginTouch(0.0f);
                }
            },
            {
                .mTime = animationDuration
            }
        };
        animationSystem.CreateAnimation(handAnimation.GetSceneObject(), handAnimationKeyframes);
    }

    return rootAnimation;
}

Pht::Animation& HowToPlayDialogView::CreateMovePieceDownAnimation(Pht::SceneObject& parent,
                                                                  const PieceResources& pieceResources,
                                                                  const LevelResources& levelResources,
                                                                  HandAnimation& handAnimation) {
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition({0.0f, 1.5f, 0.0f});
    container.GetTransform().SetScale(1.15f);
    parent.AddChild(container);
    
    CreateFieldQuad(container);
    
    auto animationDuration {6.0f};

    auto& animationSystem {mEngine.GetAnimationSystem()};
    auto& rootAnimation {
        animationSystem.CreateAnimation(container, {{.mTime = 0.0f}, {.mTime = animationDuration}})
    };
    
    handAnimation.Init(container);
    
    Pht::Vec3 lPieceInitialPosition {-0.5f, 3.0f, UiLayer::block};
    
    auto& lPiece {CreateLPiece(lPieceInitialPosition, container, pieceResources)};
    CreateTwoBlocks({2.5f, -2.0f, UiLayer::block}, BlockColor::Green, container, pieceResources);
    CreateThreeGrayBlocks({-2.0f, -3.0f, UiLayer::block}, container, levelResources);
    CreateThreeGrayBlocks({2.0f, -3.0f, UiLayer::block}, container, levelResources);
    CreateThreeGrayBlocksWithGap({-1.5f, -4.0f, UiLayer::block}, container, levelResources);
    CreateThreeGrayBlocks({2.0f, -4.0f, UiLayer::block}, container, levelResources);

    std::vector<Pht::Keyframe> lPieceAnimationKeyframes {
        {.mTime = 0.0f, .mPosition = lPieceInitialPosition},
        {.mTime = 1.2f, .mPosition = Pht::Vec3{-0.5f, 2.0f, UiLayer::block}},
        {.mTime = 1.8f, .mPosition = Pht::Vec3{-0.5f, 1.0f, UiLayer::block}},
        {.mTime = 2.4f, .mPosition = Pht::Vec3{-0.5f, 0.0f, UiLayer::block}},
        {.mTime = animationDuration}
    };
    auto& lPieceAnimation {animationSystem.CreateAnimation(lPiece, lPieceAnimationKeyframes)};
    lPieceAnimation.SetInterpolation(Pht::Interpolation::None);

    Pht::Vec3 handInitialPosition {2.25f, 1.5f, UiLayer::root};
    Pht::Vec3 handAfterDragDownPosition {2.25f, -1.5f, UiLayer::root};
    Pht::Vec3 handAfterSwipeUpPosition {2.25f, 0.1f, UiLayer::root};

    std::vector<Pht::Keyframe> handAnimationKeyframes {
        {
            .mTime = 0.0f,
            .mPosition = handInitialPosition,
            .mCallback = [&handAnimation, &handInitialPosition] () {
                handAnimation.StartInNotTouchingScreenState(handInitialPosition, 90.0f, 10.0f);
            }
        },
        {
            .mTime = 0.4f,
            .mPosition = handInitialPosition,
            .mCallback = [&handAnimation] () {
                handAnimation.BeginTouch(2.6f);
            }
        },
        {
            .mTime = 0.6f,
            .mPosition = handInitialPosition
        },
        {
            .mTime = 2.6f,
            .mPosition = handAfterDragDownPosition
        },
        {
            .mTime = 3.2f,
            .mPosition = handAfterSwipeUpPosition
        },
        {
            .mTime = 4.5f,
            .mPosition = handAfterSwipeUpPosition
        },
        {
            .mTime = 5.8f,
            .mPosition = handInitialPosition
        },
        {
            .mTime = animationDuration
        }
    };

    auto& handPhtAnimation {
        animationSystem.CreateAnimation(handAnimation.GetSceneObject(), handAnimationKeyframes)
    };
    handPhtAnimation.SetInterpolation(Pht::Interpolation::Cosine);

    return rootAnimation;
}

Pht::Animation& HowToPlayDialogView::CreateSwitchPieceAnimation(Pht::SceneObject& parent,
                                                                const CommonResources& commonResources,
                                                                const PieceResources& pieceResources,
                                                                const LevelResources& levelResources,
                                                                HandAnimation& handAnimation) {
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition({0.0f, 2.5f, 0.0f});
    container.GetTransform().SetScale(1.0f);
    parent.AddChild(container);
    
    CreateFieldQuad(container);
    
    auto animationDuration {6.0f};

    auto& animationSystem {mEngine.GetAnimationSystem()};
    auto& rootAnimation {
        animationSystem.CreateAnimation(container, {{.mTime = 0.0f}, {.mTime = animationDuration}})
    };
    
    handAnimation.Init(container);
    
    auto& iPiece {CreateIPiece({0.0f, 3.3f, UiLayer::block}, container, pieceResources)};
    auto& dPiece {CreateDPiece({0.0f, 3.3f, UiLayer::block}, container, pieceResources)};
    auto& lPiece {CreateLPiece({-0.5f, 3.3f, UiLayer::block}, container, pieceResources)};
    
    CreateTwoBlocks({2.5f, -2.0f, UiLayer::block}, BlockColor::Green, container, pieceResources);
    CreateThreeGrayBlocks({-2.0f, -3.0f, UiLayer::block}, container, levelResources);
    CreateThreeGrayBlocks({2.0f, -3.0f, UiLayer::block}, container, levelResources);
    CreateThreeGrayBlocksWithGap({-1.5f, -4.0f, UiLayer::block}, container, levelResources);
    CreateThreeGrayBlocks({2.0f, -4.0f, UiLayer::block}, container, levelResources);

    auto& gameHudRectangles {commonResources.GetGameHudRectangles()};
    
    auto& selectablePieces {CreateSceneObject()};
    selectablePieces.SetRenderable(&gameHudRectangles.GetSelectablePiecesRectangle());
    selectablePieces.GetTransform().SetPosition({2.0f, -5.775f, UiLayer::panel});
    selectablePieces.GetTransform().SetScale(0.71f);
    container.AddChild(selectablePieces);
    
    Pht::Vec3 slot1Pos {-1.95f, 0.0f, UiLayer::buttonText};
    Pht::Vec3 slot2Pos {0.05f, 0.0f, UiLayer::buttonText};
    Pht::Vec3 slot3Pos {2.4f, 0.0f, UiLayer::buttonText};
    auto& dPreviewPiece {CreateDPreviewPiece(slot1Pos, selectablePieces, pieceResources)};
    auto& iPreviewPiece {CreateIPreviewPiece(slot2Pos, selectablePieces, pieceResources)};
    auto& lPreviewPiece {CreateLPreviewPiece(slot3Pos, selectablePieces, pieceResources)};

    auto& nextPieces {CreateSceneObject()};
    nextPieces.SetRenderable(&gameHudRectangles.GetNextPiecesRectangle());
    nextPieces.GetTransform().SetPosition({-2.6f, -5.775f, UiLayer::panel});
    nextPieces.GetTransform().SetScale(0.71f);
    container.AddChild(nextPieces);
    
    auto switch1Time {1.2f};
    auto switch2Time {3.05f};
    
    std::vector<Pht::Keyframe> dPieceKeyframes {
        {.mTime = 0.0f, .mIsVisible = false},
        {.mTime = switch2Time, .mIsVisible = true},
        {.mTime = animationDuration - 0.2f, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(dPiece, dPieceKeyframes);

    std::vector<Pht::Keyframe> iPieceKeyframes {
        {.mTime = 0.0f, .mIsVisible = false},
        {.mTime = switch1Time, .mIsVisible = true},
        {.mTime = switch2Time, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(iPiece, iPieceKeyframes);

    std::vector<Pht::Keyframe> lPieceKeyframes {
        {.mTime = 0.0f, .mIsVisible = true},
        {.mTime = switch1Time, .mIsVisible = false},
        {.mTime = animationDuration - 0.2f, .mIsVisible = true},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(lPiece, lPieceKeyframes);

    std::vector<Pht::Keyframe> dPreviewPieceKeyframes {
        {.mTime = 0.0f, .mPosition = slot1Pos},
        {.mTime = switch1Time, .mPosition = slot2Pos},
        {.mTime = switch2Time, .mPosition = slot3Pos},
        {.mTime = animationDuration - 0.2f, .mPosition = slot1Pos},
        {.mTime = animationDuration}
    };
    auto& dPreviewPieceAnimation {
        animationSystem.CreateAnimation(dPreviewPiece, dPreviewPieceKeyframes)
    };
    dPreviewPieceAnimation.SetInterpolation(Pht::Interpolation::None);
    
    std::vector<Pht::Keyframe> iPreviewPieceKeyframes {
        {.mTime = 0.0f, .mPosition = slot2Pos},
        {.mTime = switch1Time, .mPosition = slot3Pos},
        {.mTime = switch2Time, .mPosition = slot1Pos},
        {.mTime = animationDuration - 0.2f, .mPosition = slot2Pos},
        {.mTime = animationDuration}
    };
    auto& iPreviewPieceAnimation {
        animationSystem.CreateAnimation(iPreviewPiece, iPreviewPieceKeyframes)
    };
    iPreviewPieceAnimation.SetInterpolation(Pht::Interpolation::None);

    std::vector<Pht::Keyframe> lPreviewPieceKeyframes {
        {.mTime = 0.0f, .mPosition = slot3Pos},
        {.mTime = switch1Time, .mPosition = slot1Pos},
        {.mTime = switch2Time, .mPosition = slot2Pos},
        {.mTime = animationDuration - 0.2f, .mPosition = slot3Pos},
        {.mTime = animationDuration}
    };
    auto& lPreviewPieceAnimation {
        animationSystem.CreateAnimation(lPreviewPiece, lPreviewPieceKeyframes)
    };
    lPreviewPieceAnimation.SetInterpolation(Pht::Interpolation::None);

    Pht::Vec3 handInitialPosition {2.5f, -4.7f, UiLayer::root};
    Pht::Vec3 handBeforeSwipePosition {2.8f, -3.3f, UiLayer::root};
    Pht::Vec3 handAfterSwipePosition {3.0f, -1.0f, UiLayer::root};
    
    std::vector<Pht::Keyframe> handAnimationKeyframes {
        {
            .mTime = 0.0f,
            .mPosition = handInitialPosition,
            .mCallback = [&handAnimation, &handInitialPosition] () {
                handAnimation.StartInNotTouchingScreenState(handInitialPosition, 145.0f, 10.0f);
            }
        },
        {
            .mTime = 1.0f,
            .mPosition = handInitialPosition,
            .mCallback = [&handAnimation] () {
                handAnimation.BeginTouch(0.0f);
            }
        },
        {
            .mTime = 1.4f,
            .mPosition = handInitialPosition,
            .mRotation = Pht::Vec3{0.0f, 0.0f, 145.0f}
        },
        {
            .mTime = 2.25f,
            .mPosition = handBeforeSwipePosition,
            .mRotation = Pht::Vec3{0.0f, 0.0f, 90.0f}
        },
        {
            .mTime = 2.5f,
            .mPosition = handBeforeSwipePosition,
            .mCallback = [&handAnimation] () {
                handAnimation.BeginTouch(0.45f);
            }
        },
        {
            .mTime = 2.7f,
            .mPosition = handBeforeSwipePosition,
        },
        {
            .mTime = 3.1f,
            .mPosition = handAfterSwipePosition,
        },
        {
            .mTime = 3.4f,
            .mPosition = handAfterSwipePosition,
            .mRotation = Pht::Vec3{0.0f, 0.0f, 90.0f}
        },
        {
            .mTime = 4.8f,
            .mPosition = handInitialPosition,
            .mRotation = Pht::Vec3{0.0f, 0.0f, 145.0f}
        },
        {
            .mTime = animationDuration - 0.4f,
            .mCallback = [&handAnimation] () {
                handAnimation.BeginTouch(0.0f);
            }
        },
        {
            .mTime = animationDuration
        }
    };
    
    auto& handPhtAnimation {
        animationSystem.CreateAnimation(handAnimation.GetSceneObject(), handAnimationKeyframes)
    };
    handPhtAnimation.SetInterpolation(Pht::Interpolation::Cosine);

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

Pht::SceneObject& HowToPlayDialogView::CreateIPiece(const Pht::Vec3& position,
                                                    Pht::SceneObject& parent,
                                                    const PieceResources& pieceResources) {
    auto& iPiece {CreateSceneObject()};
    parent.AddChild(iPiece);
    
    iPiece.GetTransform().SetPosition(position);
    
    auto& blockRenderable {
        pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                BlockColor::Green,
                                                BlockBrightness::Normal)
    };

    auto halfCellSize {0.5f};
    CreateBlock({-halfCellSize * 2.0f, 0.0f, 0.0f}, blockRenderable, iPiece);
    CreateBlock({0.0f, 0.0f, 0.0f}, blockRenderable, iPiece);
    CreateBlock({halfCellSize * 2.0f, 0.0f, 0.0f}, blockRenderable, iPiece);
    
    auto& weldRenderable {
        pieceResources.GetWeldRenderableObject(WeldRenderableKind::Normal,
                                               BlockColor::Green,
                                               BlockBrightness::Normal)
    };

    CreateWeld({-halfCellSize, 0.0f, halfCellSize}, weldRenderable, 0.0f, iPiece);
    CreateWeld({halfCellSize, 0.0f, halfCellSize}, weldRenderable, 0.0f, iPiece);
    
    return iPiece;
}

Pht::SceneObject& HowToPlayDialogView::CreateDPiece(const Pht::Vec3& position,
                                                    Pht::SceneObject& parent,
                                                    const PieceResources& pieceResources) {
    auto& dPiece {CreateSceneObject()};
    parent.AddChild(dPiece);
    
    dPiece.GetTransform().SetPosition(position);
    
    auto& blockRenderable {
        pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                BlockColor::Red,
                                                BlockBrightness::Normal)
    };

    auto halfCellSize {0.5f};
    CreateBlock({-halfCellSize * 2.0f, halfCellSize, 0.0f}, blockRenderable, dPiece);
    CreateBlock({0.0f, halfCellSize, 0.0f}, blockRenderable, dPiece);
    CreateBlock({halfCellSize * 2.0f, halfCellSize, 0.0f}, blockRenderable, dPiece);
    CreateBlock({0.0f, -halfCellSize, 0.0f}, blockRenderable, dPiece);
    CreateBlock({halfCellSize * 2.0f, -halfCellSize, 0.0f}, blockRenderable, dPiece);


    auto& weldRenderable {
        pieceResources.GetWeldRenderableObject(WeldRenderableKind::Normal,
                                               BlockColor::Red,
                                               BlockBrightness::Normal)
    };

    CreateWeld({-halfCellSize, halfCellSize, halfCellSize}, weldRenderable, 0.0f, dPiece);
    CreateWeld({halfCellSize, halfCellSize, halfCellSize}, weldRenderable, 0.0f, dPiece);
    CreateWeld({halfCellSize, -halfCellSize, halfCellSize}, weldRenderable, 0.0f, dPiece);
    CreateWeld({0.0f, 0.0f, halfCellSize}, weldRenderable, 90.0f, dPiece);
    CreateWeld({halfCellSize * 2.0f, 0.0f, halfCellSize}, weldRenderable, 90.0f, dPiece);
    
    return dPiece;
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

Pht::SceneObject& HowToPlayDialogView::CreateLPreviewPiece(const Pht::Vec3& position,
                                                           Pht::SceneObject& parent,
                                                           const PieceResources& pieceResources) {
    auto& lPiece {CreateSceneObject()};
    parent.AddChild(lPiece);
    
    auto& transform {lPiece.GetTransform()};
    transform.SetPosition(position);
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    transform.SetScale(0.60f);
    
    auto& blockRenderable {
        pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                BlockColor::Yellow,
                                                BlockBrightness::Normal)
    };

    auto halfCellSize {0.5f};
    CreateBlock({-halfCellSize, halfCellSize, 0.0f}, blockRenderable, lPiece);
    CreateBlock({halfCellSize, -halfCellSize, 0.0f}, blockRenderable, lPiece);
    CreateBlock({halfCellSize, halfCellSize, 0.0f}, blockRenderable, lPiece);
    
    return lPiece;
}

Pht::SceneObject& HowToPlayDialogView::CreateDPreviewPiece(const Pht::Vec3& position,
                                                           Pht::SceneObject& parent,
                                                           const PieceResources& pieceResources) {
    auto& dPiece {CreateSceneObject()};
    parent.AddChild(dPiece);
    
    auto& transform {dPiece.GetTransform()};
    transform.SetPosition(position);
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    transform.SetScale(0.6f);
    
    auto& blockRenderable {
        pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                BlockColor::Red,
                                                BlockBrightness::Normal)
    };

    auto halfCellSize {0.5f};
    CreateBlock({-halfCellSize * 2.0f, halfCellSize, 0.0f}, blockRenderable, dPiece);
    CreateBlock({0.0f, halfCellSize, 0.0f}, blockRenderable, dPiece);
    CreateBlock({halfCellSize * 2.0f, halfCellSize, 0.0f}, blockRenderable, dPiece);
    CreateBlock({0.0f, -halfCellSize, 0.0f}, blockRenderable, dPiece);
    CreateBlock({halfCellSize * 2.0f, -halfCellSize, 0.0f}, blockRenderable, dPiece);

    return dPiece;
}

Pht::SceneObject& HowToPlayDialogView::CreateIPreviewPiece(const Pht::Vec3& position,
                                                           Pht::SceneObject& parent,
                                                           const PieceResources& pieceResources) {
    auto& iPiece {CreateSceneObject()};
    parent.AddChild(iPiece);
    
    auto& transform {iPiece.GetTransform()};
    transform.SetPosition(position);
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    transform.SetScale(0.6f);
    
    auto& blockRenderable {
        pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                BlockColor::Green,
                                                BlockBrightness::Normal)
    };

    auto halfCellSize {0.5f};
    CreateBlock({-halfCellSize * 2.0f, 0.0f, 0.0f}, blockRenderable, iPiece);
    CreateBlock({0.0f, 0.0f, 0.0f}, blockRenderable, iPiece);
    CreateBlock({halfCellSize * 2.0f, 0.0f, 0.0f}, blockRenderable, iPiece);
    
    return iPiece;
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

Pht::SceneObject& HowToPlayDialogView::CreateLPieceGhostPiece(const Pht::Vec3& position,
                                                              float rotation,
                                                              Pht::SceneObject& parent,
                                                              const LevelResources& levelResources) {
    auto& ghostPiece {CreateSceneObject()};
    parent.AddChild(ghostPiece);
    
    auto& transform {ghostPiece.GetTransform()};
    transform.SetPosition(position);
    transform.SetScale(0.8f);
    transform.SetRotation({0.0f, 0.0f, rotation});
    
    auto& pieceTypes {levelResources.GetPieceTypes()};
    auto i {pieceTypes.find("L")};
    if (i != std::end(pieceTypes)) {
        ghostPiece.SetRenderable(i->second->GetGhostPieceRenderable());
    }
    
    return ghostPiece;
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
    icon.GetTransform().SetScale(1.1f);
    parent.AddChild(icon);

    CreateIcon("hand.png", {0.0f, 0.03f, 0.0f}, {0.9f, 0.9f}, icon);
    CreateIcon("circle.png", {-0.03f, 0.42f, 0.0f}, {0.36f, 0.36f}, icon);
    CreateIcon("circle.png", {-0.03f, 0.42f, 0.0f}, {0.42f, 0.42f}, icon);
}

void HowToPlayDialogView::CreateSwipeIcon(const Pht::Vec3& position, Pht::SceneObject& parent) {
    auto& icon {CreateSceneObject()};
    icon.GetTransform().SetPosition(position);
    icon.GetTransform().SetScale(1.1f);
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
    
    if (page.mHandAnimation) {
        page.mHandAnimation->Update();
    }
}

void HowToPlayDialogView::OnDeactivate() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}


#include "GameHud.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "TextComponent.hpp"
#include "ObjMesh.hpp"
#include "QuadMesh.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "Field.hpp"
#include "LevelResources.hpp"
#include "PieceResources.hpp"
#include "GameHudRectangles.hpp"
#include "GameHudResources.hpp"
#include "GameHudController.hpp"
#include "UiLayer.hpp"
#include "CommonResources.hpp"
#include "StringUtils.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 lightDirectionA {0.6f, 1.0f, 1.0f};
    const Pht::Vec3 lightDirectionB {0.4f, 1.0f, 1.0f};
    constexpr auto lightAnimationDuration = 5.0f;
    constexpr auto cellSize = 1.25f;
    
    template <typename T, typename U>
    void CreatePreviewPieces(T& previewPieces,
                             U& previewPieceRelativePositions,
                             Pht::SceneObject& parentObject,
                             Pht::Scene& scene) {
        for (auto i = 0; i < previewPieces.size(); ++i) {
            auto& piece = previewPieces[i];
            
            piece.mSceneObject = &scene.CreateSceneObject(parentObject);
            auto& transform = piece.mSceneObject->GetTransform();
            transform.SetPosition(previewPieceRelativePositions[i]);
            transform.SetRotation({10.0f, 0.0f, 0.0f});

            piece.mSceneObjectPool =
                std::make_unique<SceneObjectPool>(SceneObjectPoolKind::PreviewPieceBlocks,
                                                  *piece.mSceneObject);
            piece.mSceneObjectPool->SetIsActive(false);
        }
    }
    
    std::unique_ptr<Pht::Button> CreatePieceButton(const Pht::Vec3& position,
                                                   const Pht::Vec2& size,
                                                   Pht::SceneObject& parentObject,
                                                   Pht::Scene& scene,
                                                   Pht::IEngine& engine) {
        auto& buttonSceneObject = scene.CreateSceneObject(parentObject);
        buttonSceneObject.SetIsVisible(false);
        buttonSceneObject.GetTransform().SetPosition(position);

        auto buttonSize = size * GameHud::selectablePiecesContainerScale;
        return std::make_unique<Pht::Button>(buttonSceneObject, buttonSize, engine);
    }
    
    float CalculateLowerHudObjectYPosition(Pht::IEngine& engine) {
        auto& renderer = engine.GetRenderer();
        auto bottomPadding = renderer.GetBottomPaddingHeight();
        auto yPosition = -renderer.GetHudFrustumSize().y / 2.0f + bottomPadding + 1.54f;
        if (bottomPadding == 0.0f) {
            yPosition += 1.05f;
        }
        
        return yPosition;
    }
    
    int WriteIntegerAtBeginningOfString(int value, std::string& str) {
        constexpr auto bufSize = 64;
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%d", value);
        auto numDigits = std::strlen(buffer);
        
        assert(str.size() >= numDigits);

        for (auto i = 0; i < numDigits; ++i) {
            str[i] = buffer[i];
        }
        
        return static_cast<int>(numDigits);
    }

    void FillStringWithSpaces(std::string& str) {
        auto strSize = str.size();
        for (auto i = 0; i < strSize; ++i) {
            str[i] = ' ';
        }
    }
}

GameHud::GameHud(Pht::IEngine& engine,
                 const GameLogic& gameLogic,
                 const Field& field,
                 const LevelResources& levelResources,
                 const PieceResources& pieceResources,
                 const GameHudResources& gameHudResources,
                 GameHudController& gameHudController,
                 const CommonResources& commonResources,
                 Pht::Scene& scene,
                 Pht::SceneObject& parentObject,
                 int hudLayer,
                 const Level& level) :
    mEngine {engine},
    mGameLogic {gameLogic},
    mField {field},
    mPieceResources {pieceResources},
    mLevelObjective {level.GetObjective()},
    mNextPreviewPiecesRelativePositions {
        Pht::Vec3{0.0f, 0.9f, UiLayer::block},
        Pht::Vec3{0.0f, -0.9f, UiLayer::block},
        Pht::Vec3{0.0f, -1.88f, UiLayer::block}
    },
    mSelectablePreviewPiecesRelativePositions {
        Pht::Vec3{-3.53f, 0.0f, UiLayer::block},
        Pht::Vec3{-2.07f, 0.0f, UiLayer::block},
        Pht::Vec3{0.72f, 0.0f, UiLayer::block},
        Pht::Vec3{3.51f, 0.0f, UiLayer::block},
        Pht::Vec3{4.71f, 0.0f, UiLayer::block}
    } {

    mNext2PiecesPreviousFrame.fill(nullptr);
    mSelectablePiecesPreviousFrame.fill(nullptr);
    
    gameHudController.SetHudEventListener(*this);

    CreateLightAndCamera(scene, parentObject, hudLayer);

    mUpperContainer = &scene.CreateSceneObject();
    parentObject.AddChild(*mUpperContainer);
    
    CreateUpperBarObject(scene, *mUpperContainer, commonResources);
    CreateProgressObject(scene, *mUpperContainer, commonResources, levelResources, gameHudResources);
    CreateStarMeterObject(scene, *mUpperContainer, commonResources);
    CreateMovesObject(scene, *mUpperContainer, commonResources, gameHudResources);
    CreateLowerBarObject(scene, parentObject, commonResources);
    CreateSelectablePiecesObject(scene, parentObject, commonResources.GetGameHudRectangles());
    CreateNextPiecesObject(scene, parentObject, commonResources);
}

void GameHud::CreateLightAndCamera(Pht::Scene& scene,
                                   Pht::SceneObject& parentObject,
                                   int hudLayer) {
    auto& lightSceneObject = scene.CreateSceneObject();
    lightSceneObject.SetIsVisible(false);
    auto lightComponent = std::make_unique<Pht::LightComponent>(lightSceneObject);
    mLight = lightComponent.get();
    lightSceneObject.SetComponent<Pht::LightComponent>(std::move(lightComponent));
    parentObject.AddChild(lightSceneObject);
    
    auto& cameraSceneObject = scene.CreateSceneObject();
    cameraSceneObject.SetIsVisible(false);
    cameraSceneObject.GetTransform().SetPosition({0.0f, 0.0f, 15.5f});
    auto cameraComponent = std::make_unique<Pht::CameraComponent>(cameraSceneObject);
    auto* camera = cameraComponent.get();
    cameraSceneObject.SetComponent<Pht::CameraComponent>(std::move(cameraComponent));
    parentObject.AddChild(cameraSceneObject);
    
    auto* hudRenderPass = scene.GetRenderPass(hudLayer);
    assert(hudRenderPass);
    hudRenderPass->SetLight(mLight);
    hudRenderPass->SetCamera(camera);
}

void GameHud::CreateUpperBarObject(Pht::Scene& scene,
                                   Pht::SceneObject& parentObject,
                                   const CommonResources& commonResources) {
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetLargeDarkMenuWindow();
    auto& renderer = mEngine.GetRenderer();
    auto frustumSize = renderer.GetHudFrustumSize();
    auto& upperBar = scene.CreateSceneObject();
    upperBar.SetRenderable(&menuWindow.GetRenderable());
    parentObject.AddChild(upperBar);

    auto topPadding = renderer.GetTopPaddingHeight();
    Pht::Vec3 position {0.0f, frustumSize.y / 2.0f - topPadding + 0.6f, UiLayer::tutorialWindow};
    upperBar.GetTransform().SetPosition(position);
    upperBar.GetTransform().SetScale({2.0f, 0.16f, 1.0f});

    auto shadowWidth = frustumSize.x;
    auto shadowHeight = 0.25f;
    Pht::QuadMesh::Vertices shadowVertices {
        {{-shadowWidth / 2.0f, -shadowHeight / 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{shadowWidth / 2.0f, -shadowHeight / 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{shadowWidth / 2.0f, shadowHeight / 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.5f}},
        {{-shadowWidth / 2.0f, shadowHeight / 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.5f}},
    };
    Pht::Material shadowMaterial;
    auto& shadow = scene.CreateSceneObject(Pht::QuadMesh {shadowVertices}, shadowMaterial);
    parentObject.AddChild(shadow);
    auto shadowY = position.y - 1.75f;
    Pht::Vec3 shadowPosition {0.0f, shadowY, UiLayer::slidingTextRectangle};
    shadow.GetTransform().SetPosition(shadowPosition);
}

void GameHud::CreateProgressObject(Pht::Scene& scene,
                                   Pht::SceneObject& parentObject,
                                   const CommonResources& commonResources,
                                   const LevelResources& levelResources,
                                   const GameHudResources& gameHudResources) {
    auto& progressContainer = scene.CreateSceneObject();
    auto& renderer = mEngine.GetRenderer();
    auto frustumSize = renderer.GetHudFrustumSize();
    auto topPadding = renderer.GetTopPaddingHeight();
    
    Pht::Vec3 position {
        -frustumSize.x / 2.0f + 1.9f,
        frustumSize.y / 2.0f - topPadding - 0.67f,
        UiLayer::root
    };
    
    mProgressContainer = &progressContainer;
    progressContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(progressContainer);

    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize20(PotentiallyZoomedScreen::Yes),
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.27f, 0.27f, 0.27f, 0.5f}
    };

    // Warning! Must be at least seven spaces to fit digits.
    const std::string placeholder(7, ' ');
    
    mProgressText = &scene.CreateText(placeholder, textProperties);
    auto& progressTextSceneObject = mProgressText->GetSceneObject();
    
    switch (mLevelObjective) {
        case Level::Objective::Clear:
            CreateGrayBlock(scene, progressContainer, commonResources);
            progressTextSceneObject.GetTransform().SetPosition({-0.48f, -0.15f, UiLayer::text});
            mProgressGoal = mField.CalculateNumLevelBlocks();
            break;
        case Level::Objective::BringDownTheAsteroid:
            CreateAsteroid(scene, progressContainer, levelResources);
            CreateDownArrow(scene, progressContainer, gameHudResources);
            progressTextSceneObject.GetTransform().SetPosition({0.15f, -0.15f, UiLayer::text});
            mProgressGoal = mField.CalculateAsteroidRow().GetValue();
            break;
        case Level::Objective::Build:
            CreateBlueprintSlot(scene, progressContainer, levelResources);
            progressTextSceneObject.GetTransform().SetPosition({-0.48f, -0.1f, UiLayer::text});
            mProgressGoal = mField.CalculateNumEmptyBlueprintSlots();
            break;
    }

    if (mLevelObjective == Level::Objective::BringDownTheAsteroid) {
        mProgressGoalString = "%";
    } else {
        mProgressGoalString = "/" + std::to_string(mProgressGoal);
    }
    
    progressContainer.AddChild(progressTextSceneObject);
    
    Pht::SceneObjectUtils::ScaleRecursively(*mProgressContainer, 1.1f);
}

void GameHud::CreateGrayBlock(Pht::Scene& scene,
                              Pht::SceneObject& progressContainer,
                              const CommonResources& commonResources) {
    auto& grayBlock =
        scene.CreateSceneObject(Pht::ObjMesh {"cube_428.obj", 1.25f},
                                commonResources.GetMaterials().GetGrayYellowMaterial());
    
    auto& transform = grayBlock.GetTransform();
    transform.SetPosition({-1.05f, 0.0f, UiLayer::block});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    transform.SetScale(0.52f);
    
    progressContainer.AddChild(grayBlock);
}

void GameHud::CreateAsteroid(Pht::Scene& scene,
                             Pht::SceneObject& progressContainer,
                             const LevelResources& levelResources) {
    auto& asteroid = scene.CreateSceneObject();
    asteroid.SetRenderable(&levelResources.GetAsteroidFragmentRenderable());
    
    auto& transform = asteroid.GetTransform();
    transform.SetPosition({-1.0f, 0.0f, UiLayer::block});
    transform.SetRotation({-25.0f, 45.0f, -12.0f});
    transform.SetScale(2.7f);
    
    progressContainer.AddChild(asteroid);
}

void GameHud::CreateDownArrow(Pht::Scene& scene,
                              Pht::SceneObject& progressContainer,
                              const GameHudResources& gameHudResources) {
    auto& arrow = scene.CreateSceneObject();
    arrow.SetRenderable(&gameHudResources.GetDownArrowRenderable());
    arrow.GetTransform().SetPosition({-0.25f, 0.05f, UiLayer::text});
    progressContainer.AddChild(arrow);
}

void GameHud::CreateBlueprintSlot(Pht::Scene& scene,
                                  Pht::SceneObject& progressContainer,
                                  const LevelResources& levelResources) {
    auto& blueprintSlotContainer = scene.CreateSceneObject();
    auto& transform = blueprintSlotContainer.GetTransform();
    transform.SetPosition({-1.0f, 0.15f, UiLayer::block});
    transform.SetScale(0.5f);
    progressContainer.AddChild(blueprintSlotContainer);

    auto& blueprintSlot = scene.CreateSceneObject();
    blueprintSlot.SetRenderable(&levelResources.GetBlueprintSlotNonDepthWritingRenderable());
    blueprintSlotContainer.AddChild(blueprintSlot);

    auto& fieldCell = scene.CreateSceneObject();
    fieldCell.SetRenderable(&levelResources.GetFieldCellRenderable());
    fieldCell.GetTransform().SetPosition({0.0f, 0.0f, -0.005f});
    blueprintSlotContainer.AddChild(fieldCell);
}

void GameHud::CreateStarMeterObject(Pht::Scene& scene,
                                    Pht::SceneObject& parentObject,
                                    const CommonResources& commonResources) {
    auto& renderer = mEngine.GetRenderer();
    auto frustumSize = renderer.GetHudFrustumSize();
    auto topPadding = renderer.GetTopPaddingHeight();

    Pht::ObjMesh starMesh {"star.obj", 0.05f};
    auto& goldMaterial = commonResources.GetMaterials().GetGoldMaterial();
    auto& goldStar = scene.CreateSceneObject(starMesh, goldMaterial);
    parentObject.AddChild(goldStar);
    Pht::Vec3 goldStarPosition {-1.5f, frustumSize.y / 2.0f - topPadding - 0.5f, UiLayer::block};
    goldStar.GetTransform().SetPosition(goldStarPosition);
    goldStar.GetTransform().SetRotation({90.0f, 0.0f, 0.0f});

    auto greyMaterial = commonResources.GetMaterials().GetDarkGrayMaterial();
    auto& greyStar = scene.CreateSceneObject(starMesh, greyMaterial);
    parentObject.AddChild(greyStar);
    Pht::Vec3 greyStarPosition {0.0f, frustumSize.y / 2.0f - topPadding - 0.5f, UiLayer::block};
    greyStar.GetTransform().SetPosition(greyStarPosition);
    greyStar.GetTransform().SetRotation({90.0f, 0.0f, 0.0f});

    auto& greyStar2 = scene.CreateSceneObject(starMesh, greyMaterial);
    parentObject.AddChild(greyStar2);
    Pht::Vec3 greyStar2Position {1.5f, frustumSize.y / 2.0f - topPadding - 0.5f, UiLayer::block};
    greyStar2.GetTransform().SetPosition(greyStar2Position);
    greyStar2.GetTransform().SetRotation({90.0f, 0.0f, 0.0f});
}

void GameHud::CreateMovesObject(Pht::Scene& scene,
                                Pht::SceneObject& parentObject,
                                const CommonResources& commonResources,
                                const GameHudResources& gameHudResources) {
    mMovesContainer = &scene.CreateSceneObject(parentObject);
    auto& renderer = mEngine.GetRenderer();
    auto frustumSize = renderer.GetHudFrustumSize();
    auto topPadding = renderer.GetTopPaddingHeight();
    
    Pht::Vec3 position {
        frustumSize.x / 2.0f - 1.4f,
        frustumSize.y / 2.0f - topPadding - 0.67f,
        UiLayer::root
    };

    mMovesContainer->GetTransform().SetPosition(position);
    mMovesIconContainer = &scene.CreateSceneObject(*mMovesContainer);

    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::Yes),
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.27f, 0.27f, 0.27f, 0.5f},
        Pht::SnapToPixel::No
    };
    textProperties.mAlignment = Pht::TextAlignment::CenterX;
    
    mMovesTextContainer = &scene.CreateSceneObject(*mMovesContainer);
    mMovesTextContainer->GetTransform().SetPosition({0.35f, 0.125f, UiLayer::root});

    std::string text {"   "};   // Warning! Must be three spaces to fit digits.
    mMovesText = &scene.CreateText(text, textProperties, *mMovesTextContainer);
    mMovesTextSceneObject = &mMovesText->GetSceneObject();
    mMovesTextSceneObject->GetTransform().SetPosition({0.0f, -0.285f, UiLayer::text});
    
    mBlueMovesIcon = &CreateMovesIcon(scene,
                                      *mMovesIconContainer,
                                      gameHudResources.GetBlueArrowMeshRenderable());
    mYellowMovesIcon = &CreateMovesIcon(scene,
                                        *mMovesIconContainer,
                                        gameHudResources.GetYellowArrowMeshRenderable());
    
    mMovesTextScaleFactor =
        static_cast<float>(commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes).GetSize()) /
        static_cast<float>(commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::Yes).GetSize());
    mMovesContainer->GetTransform().SetScale(movesContainerScale);
    mMovesTextContainer->GetTransform().SetScale(movesTextStaticScale);
    Pht::SceneObjectUtils::ScaleRecursively(*mMovesTextSceneObject,
                                            movesContainerScale * movesTextStaticScale * mMovesTextScaleFactor);
}

Pht::SceneObject& GameHud::CreateMovesIcon(Pht::Scene& scene,
                                           Pht::SceneObject& movesContainer,
                                           Pht::RenderableObject& arrowRenderable) {
    auto& movesIcon = scene.CreateSceneObject();
    movesContainer.AddChild(movesIcon);

    auto& baseTransform = movesIcon.GetTransform();
    baseTransform.SetPosition({-0.9f, 0.05f, UiLayer::root});
    baseTransform.SetRotation({-29.1f, -29.1f, 0.0f});

    CreateArrow({0.0f, 0.27f, 0.0f}, {90.0f, 0.0f, 90.0f}, arrowRenderable, scene, movesIcon);
    CreateArrow({0.05f, -0.27f, 0.0f}, {270.0f, 0.0f, 90.0f}, arrowRenderable, scene, movesIcon);
    
    scene.ConvertSceneObjectToStaticBatch(movesIcon, std::string{"GameHudMovesIcon"});
    return movesIcon;
}

void GameHud::CreateArrow(const Pht::Vec3& position,
                          const Pht::Vec3& rotation,
                          Pht::RenderableObject& renderable,
                          Pht::Scene& scene,
                          Pht::SceneObject& parent) {
    auto& arrow = scene.CreateSceneObject();
    arrow.GetTransform().SetPosition(position);
    arrow.GetTransform().SetRotation(rotation);
    arrow.SetRenderable(&renderable);
    parent.AddChild(arrow);
}

void GameHud::CreateNextPiecesObject(Pht::Scene& scene,
                                     Pht::SceneObject& parentObject,
                                     const CommonResources& commonResources) {
    mNextPiecesContainer = &scene.CreateSceneObject(parentObject);
    
    auto& hudFrustumWidth = mEngine.GetRenderer().GetHudFrustumSize().x;

    Pht::Vec3 position {
        hudFrustumWidth / 2.0f - 0.85f,
        CalculateLowerHudObjectYPosition(mEngine),
        UiLayer::root
    };

    mNextPiecesContainer->GetTransform().SetPosition(position);
    
    mNextPiecesSceneObject = &scene.CreateSceneObject(*mNextPiecesContainer);
    auto& nextPiecesSceneObjectTransform = mNextPiecesSceneObject->GetTransform();
    nextPiecesSceneObjectTransform.SetScale(nextPiecesContainerScale);
    nextPiecesSceneObjectTransform.SetPosition({0.0f, -1.15f, 0.0f});

    auto& nextPiecesRectangle = scene.CreateSceneObject(*mNextPiecesContainer);
    auto& hudRectangles = commonResources.GetGameHudRectangles();
    nextPiecesRectangle.SetRenderable(&hudRectangles.GetNextPiecesRectangle());
    nextPiecesRectangle.GetTransform().SetPosition({-0.22f, -0.75f, UiLayer::piecesRectangle});
    
    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize20(PotentiallyZoomedScreen::Yes),
        0.9f,
        Pht::Vec4{0.9f, 0.9f, 0.9f, 1.0f}
    };
    
    textProperties.mItalicSlant = 0.05f;
    
    auto& nextText = scene.CreateText("NEXT", textProperties);
    auto& nextTextSceneobject = nextText.GetSceneObject();
    nextTextSceneobject.GetTransform().SetPosition({-0.5f, 0.44f, UiLayer::text});
    mNextPiecesContainer->AddChild(nextTextSceneobject);

    CreatePreviewPieces(mNextPreviewPieces,
                        mNextPreviewPiecesRelativePositions,
                        *mNextPiecesSceneObject,
                        scene);
}

void GameHud::CreateLowerBarObject(Pht::Scene& scene,
                                   Pht::SceneObject& parentObject,
                                   const CommonResources& commonResources) {
    auto& renderer = mEngine.GetRenderer();
    auto frustumSize = renderer.GetHudFrustumSize();
/*
    auto& upperBar = scene.CreateSceneObject();
    upperBar.SetRenderable(&menuWindow.GetRenderable());
    parentObject.AddChild(upperBar);
 
    auto topPadding = renderer.GetTopPaddingHeight();
    Pht::Vec3 position {0.0f, frustumSize.y / 2.0f - topPadding + 0.85f, UiLayer::tutorialWindow};
    upperBar.GetTransform().SetPosition(position);
    upperBar.GetTransform().SetScale({2.0f, 0.25f, 1.0f});
    // upperBar.GetTransform().SetRotation({0.0f, 0.0f, 90.0f});
    // upperBar.GetTransform().SetScale({0.25f, 2.0f, 1.0f});
*/
/*
    // auto& lowerBar = scene.CreateSceneObject(Pht::QuadMesh {frustumSize.x, 4.0f}, barMaterial);
    auto& lowerBar = scene.CreateSceneObject(Pht::QuadMesh {frustumSize.x, 5.3f}, barMaterial);
    mUpperContainer->AddChild(lowerBar);

    // auto lowerBarY = CalculateLowerHudObjectYPosition(mEngine) - 1.5f;
    auto lowerBarY = CalculateLowerHudObjectYPosition(mEngine) - 0.8f;
    Pht::Vec3 lowerBarPosition {0.0f, lowerBarY, UiLayer::tutorialWindow};
    lowerBar.GetTransform().SetPosition(lowerBarPosition);
*/
/*
    auto width = frustumSize.x;
    auto height = 5.3f;
    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.5f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.5f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
    };
    Pht::Material material;
    auto& lowerBar = scene.CreateSceneObject(Pht::QuadMesh {vertices}, material);
    mUpperContainer->AddChild(lowerBar);
    auto lowerBarY = CalculateLowerHudObjectYPosition(mEngine) - 0.8f;
    Pht::Vec3 lowerBarPosition {0.0f, lowerBarY, UiLayer::tutorialWindow};
    lowerBar.GetTransform().SetPosition(lowerBarPosition);
*/
/*
    auto width = frustumSize.x;
    auto height = 5.3f;
    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.175f, 0.35f, 0.75f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.175f, 0.35f, 0.75f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.175f, 0.35f, 0.25f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.175f, 0.35f, 0.25f}},
    };
    Pht::Material material;
    auto& lowerBar = scene.CreateSceneObject(Pht::QuadMesh {vertices}, material);
    mUpperContainer->AddChild(lowerBar);
    auto lowerBarY = CalculateLowerHudObjectYPosition(mEngine) - 0.8f;
    Pht::Vec3 lowerBarPosition {0.0f, lowerBarY, UiLayer::tutorialWindow};
    lowerBar.GetTransform().SetPosition(lowerBarPosition);
*/
/*
    auto width = frustumSize.x;
    auto height = 5.3f;
    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.75f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.75f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.25f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.25f}},
    };
    Pht::Material material;
    auto& lowerBar = scene.CreateSceneObject(Pht::QuadMesh {vertices}, material);
    mUpperContainer->AddChild(lowerBar);
    auto lowerBarY = CalculateLowerHudObjectYPosition(mEngine) - 0.8f;
    Pht::Vec3 lowerBarPosition {0.0f, lowerBarY, UiLayer::tutorialWindow};
    lowerBar.GetTransform().SetPosition(lowerBarPosition);
*/
/*
    auto width = frustumSize.x;
    auto height = 5.3f;
    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.65f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.65f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.15f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.15f}},
    };
    Pht::Material material;
    auto& lowerBar = scene.CreateSceneObject(Pht::QuadMesh {vertices}, material);
    mUpperContainer->AddChild(lowerBar);
    auto lowerBarY = CalculateLowerHudObjectYPosition(mEngine) - 0.8f;
    Pht::Vec3 lowerBarPosition {0.0f, lowerBarY, UiLayer::tutorialWindow};
    lowerBar.GetTransform().SetPosition(lowerBarPosition);
*/
/*
    auto width = frustumSize.x;
    auto height = 5.3f;
    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.7f, 0.5f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.7f, 0.5f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.7f, 0.15f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.7f, 0.15f}},
    };
    Pht::Material material;
    auto& lowerBar = scene.CreateSceneObject(Pht::QuadMesh {vertices}, material);
    parentObject.AddChild(lowerBar);
    auto lowerBarY = CalculateLowerHudObjectYPosition(mEngine) - 0.8f;
    Pht::Vec3 lowerBarPosition {0.0f, lowerBarY, UiLayer::tutorialWindow};
    lowerBar.GetTransform().SetPosition(lowerBarPosition);
*/
/*
    auto width = frustumSize.x;
    auto height = 5.3f;
    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.75f, 0.25f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.75f, 0.25f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.75f, 0.15f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.75f, 0.15f}},
    };
    Pht::Material material;
    auto& lowerBar = scene.CreateSceneObject(Pht::QuadMesh {vertices}, material);
    parentObject.AddChild(lowerBar);
    auto lowerBarY = CalculateLowerHudObjectYPosition(mEngine) - 0.8f;
    Pht::Vec3 lowerBarPosition {0.0f, lowerBarY, UiLayer::tutorialWindow};
    lowerBar.GetTransform().SetPosition(lowerBarPosition);
*/

    auto width = frustumSize.x;
    auto height = 5.3f;
    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.65f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.65f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.15f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.0f, 0.0f, 0.5f, 0.15f}},
    };
    Pht::Material material;
    auto& lowerBar = scene.CreateSceneObject(Pht::QuadMesh {vertices}, material);
    parentObject.AddChild(lowerBar);
    auto lowerBarY = CalculateLowerHudObjectYPosition(mEngine) - 0.8f;
    Pht::Vec3 lowerBarPosition {0.0f, lowerBarY, UiLayer::tutorialWindow};
    lowerBar.GetTransform().SetPosition(lowerBarPosition);

/*
    Pht::ObjMesh cubeMesh {"cube_428.obj", 18.0f};
    auto& materials = commonResources.GetMaterials();
    auto& lowerBar = scene.CreateSceneObject(cubeMesh, materials.GetLightGrayMaterial());
    parentObject.AddChild(lowerBar);
    auto lowerBarY = CalculateLowerHudObjectYPosition(mEngine) - 7.5f;
    Pht::Vec3 lowerBarPosition {0.0f, lowerBarY, -12.0f};
    lowerBar.GetTransform().SetPosition(lowerBarPosition);
    lowerBar.GetTransform().SetRotation({2.5f, 0.0f, 0.0f});
*/
}

void GameHud::CreateSelectablePiecesObject(Pht::Scene& scene,
                                           Pht::SceneObject& parentObject,
                                           const GameHudRectangles& hudRectangles) {
    mSelectablePiecesContainer = &scene.CreateSceneObject(parentObject);
    
    Pht::Vec3 position {-0.75f, CalculateLowerHudObjectYPosition(mEngine), UiLayer::root};
    mSelectablePiecesContainer->GetTransform().SetPosition(position);
    
    mSelectablePiecesSceneObject = &scene.CreateSceneObject(*mSelectablePiecesContainer);
    mSelectablePiecesSceneObject->GetTransform().SetScale(selectablePiecesContainerScale);

    mSelectablePiecesRectangle = &scene.CreateSceneObject(*mSelectablePiecesContainer);
    mSelectablePiecesRectangle->SetRenderable(&hudRectangles.GetSelectablePiecesRectangle());
    mSelectablePiecesRectangle->GetTransform().SetPosition({0.375f, 0.0f, UiLayer::piecesRectangle});
    
    mPressedSelectablePiecesRectangle = &scene.CreateSceneObject(*mSelectablePiecesContainer);
    mPressedSelectablePiecesRectangle->SetRenderable(&hudRectangles.GetPressedSelectablePiecesRectangle());
    mPressedSelectablePiecesRectangle->GetTransform().SetPosition({0.375f, 0.0f, UiLayer::piecesRectangle});
    mPressedSelectablePiecesRectangle->SetIsVisible(false);

    CreatePreviewPieces(mSelectablePreviewPieces,
                        mSelectablePreviewPiecesRelativePositions,
                        *mSelectablePiecesSceneObject,
                        scene);
    
    mActivePieceButton = CreatePieceButton(mSelectablePreviewPiecesRelativePositions[1],
                                           {60.0f, 60.0f},
                                           *mSelectablePiecesSceneObject,
                                           scene,
                                           mEngine);
    mSelectable0Button = CreatePieceButton(mSelectablePreviewPiecesRelativePositions[2],
                                           {60.0f, 60.0f},
                                           *mSelectablePiecesSceneObject,
                                           scene,
                                           mEngine);
    mSelectable1Button = CreatePieceButton(mSelectablePreviewPiecesRelativePositions[3],
                                           {60.0f, 60.0f},
                                           *mSelectablePiecesSceneObject,
                                           scene,
                                           mEngine);
}

void GameHud::OnSwitchButtonDown() {
    mPressedSelectablePiecesRectangle->SetIsVisible(true);
    mSelectablePiecesRectangle->SetIsVisible(false);
}

void GameHud::OnSwitchButtonUp() {
    mPressedSelectablePiecesRectangle->SetIsVisible(false);
    mSelectablePiecesRectangle->SetIsVisible(true);
}

void GameHud::Update() {
    UpdateLightAnimation();
    UpdateProgress();
    UpdateMovesLeft();
    UpdatePreviewPieces();
}

void GameHud::UpdateLightAnimation() {
    mLightAnimationTime += mEngine.GetLastFrameSeconds();
    if (mLightAnimationTime > lightAnimationDuration) {
        mLightAnimationTime = 0.0f;
    }
    
    auto t = (sin(mLightAnimationTime * 2.0f * 3.1415f / lightAnimationDuration) + 1.0f) / 2.0f;
    mLight->SetDirection(lightDirectionA.Lerp(t, lightDirectionB));
}

void GameHud::UpdateProgress() {
    auto progress =
        mLevelObjective == Level::Objective::BringDownTheAsteroid ?
        ((mProgressGoal - mField.CalculateAsteroidRow().GetValue()) * 100) / mProgressGoal :
        mProgressGoal - mGameLogic.GetNumObjectsLeftToClear();

    if (progress != mProgress) {
        auto& progressText = mProgressText->GetText();
        FillStringWithSpaces(progressText);
        auto numDigitsWritten = WriteIntegerAtBeginningOfString(progress, progressText);
        StringUtils::StringCopy(progressText,
                                numDigitsWritten,
                                mProgressGoalString,
                                static_cast<int>(mProgressGoalString.size()));
        mProgress = progress;
    }
}

void GameHud::UpdateMovesLeft() {
    auto movesLeft = GetMovesLeft();
    if (movesLeft != mMovesLeft) {
        auto& movesText = mMovesText->GetText();
        FillStringWithSpaces(movesText);
        WriteIntegerAtBeginningOfString(movesLeft, movesText);
        mMovesLeft = movesLeft;
    }
}

int GameHud::GetMovesLeft() const {
    switch (mNumMovesLeftSource) {
        case NumMovesLeftSource::GameLogic:
            return mGameLogic.GetMovesLeft();
        case NumMovesLeftSource::Self:
            return mMovesLeftSelf;
    }
}

void GameHud::UpdatePreviewPieces() {
    UpdateNextPreviewPieceGroup();
    UpdateSelectablePreviewPieceGroup();
}

void GameHud::UpdateNextPreviewPieceGroup() {
    auto& next2Pieces = mGameLogic.GetNextPieceGenerator().GetNext2Pieces();
    auto& positions = mNextPreviewPiecesRelativePositions;
    
    if (ShouldStartNextPieceAnimation()) {
        if (mNextPreviewPieces[2].mSceneObjectPool->IsActive()) {
            // Need to shift up the piece to the top if the previous next piece animation has not
            // finished yet.
            UpdateNextPreviewPiece(mNextPreviewPieces[0], mNext2PiecesPreviousFrame[1], positions[0]);
        }

        UpdateNextPreviewPiece(mNextPreviewPieces[1], next2Pieces[0], positions[1]);
        UpdateNextPreviewPiece(mNextPreviewPieces[2], next2Pieces[1], positions[2]);
    } else if (next2Pieces != mNext2PiecesPreviousFrame) {
        UpdateNextPreviewPiece(mNextPreviewPieces[0], next2Pieces[0], positions[0]);
        UpdateNextPreviewPiece(mNextPreviewPieces[1], next2Pieces[1], positions[1]);
        UpdateNextPreviewPiece(mNextPreviewPieces[2], nullptr, positions[2]);
    }
    
    mNext2PiecesPreviousFrame = next2Pieces;
}

bool GameHud::ShouldStartNextPieceAnimation() const {
    switch (mGameLogic.GetPreviewPieceAnimationToStart()) {
        case PreviewPieceAnimationToStart::NextPieceAndSwitch:
        case PreviewPieceAnimationToStart::NextPieceAndRefillActive:
        case PreviewPieceAnimationToStart::NextPieceAndRefillSelectable0:
        case PreviewPieceAnimationToStart::NextPieceAndRefillSelectable1:
            return true;
        default:
            return false;
    }
}

void GameHud::UpdateSelectablePreviewPieceGroup() {
    auto previewPieceAnimationToStart = mGameLogic.GetPreviewPieceAnimationToStart();
    
    auto shouldStartNextPieceAndSwitchAnimation =
        previewPieceAnimationToStart == PreviewPieceAnimationToStart::NextPieceAndSwitch;
    
    auto shouldStartSwitchPieceAnimation =
        previewPieceAnimationToStart == PreviewPieceAnimationToStart::SwitchPiece ||
        shouldStartNextPieceAndSwitchAnimation;
    
    auto shouldStartRemoveActivePieceAnimation =
        previewPieceAnimationToStart == PreviewPieceAnimationToStart::RemoveActivePiece;

    auto* activePiece = mGameLogic.GetPieceType();
    auto& selectablePieces = mGameLogic.GetSelectablePieces();
    auto& positions = mSelectablePreviewPiecesRelativePositions;

    auto piecesChanged =
        selectablePieces != mSelectablePiecesPreviousFrame ||
        activePiece != mActivePiecePreviousFrame;

    if (shouldStartSwitchPieceAnimation) {
        if (mSelectablePreviewPieces[3].mSceneObjectPool->IsActive()) {
            UpdateSelectable1PreviewPiece(mSelectablePreviewPieces[0],
                                          mActivePiecePreviousFrame,
                                          positions[1]);
        }
        
        UpdateActivePreviewPiece(mSelectablePreviewPieces[1], activePiece, positions[2]);
        UpdateSelectable0PreviewPiece(mSelectablePreviewPieces[2], selectablePieces[0], positions[3]);
        
        if (shouldStartNextPieceAndSwitchAnimation) {
            UpdateSelectable1PreviewPiece(mSelectablePreviewPieces[3], nullptr, positions[3]);
        } else {
            UpdateSelectable1PreviewPiece(mSelectablePreviewPieces[3], selectablePieces[1], positions[4]);
        }
    } else if (previewPieceAnimationToStart == PreviewPieceAnimationToStart::NextPieceAndRefillActive) {
        UpdateSelectablePreviewPiece(mSelectablePreviewPieces[0], nullptr, positions[1]);
    } else if (previewPieceAnimationToStart == PreviewPieceAnimationToStart::NextPieceAndRefillSelectable0) {
        UpdateSelectablePreviewPiece(mSelectablePreviewPieces[1], nullptr, positions[2]);
    } else if (previewPieceAnimationToStart == PreviewPieceAnimationToStart::NextPieceAndRefillSelectable1) {
        UpdateSelectablePreviewPiece(mSelectablePreviewPieces[2], nullptr, positions[3]);
        UpdateSelectablePreviewPiece(mSelectablePreviewPieces[3], nullptr, positions[3]);
    } else if (piecesChanged || shouldStartRemoveActivePieceAnimation) {
        if (shouldStartRemoveActivePieceAnimation) {
            UpdateActivePreviewPiece(mSelectablePreviewPieces[0], mActivePiecePreviousFrame, positions[1]);
        } else {
            UpdateActivePreviewPiece(mSelectablePreviewPieces[0], activePiece, positions[1]);
        }

        UpdateSelectable0PreviewPiece(mSelectablePreviewPieces[1], selectablePieces[0], positions[2]);
        UpdateSelectable1PreviewPiece(mSelectablePreviewPieces[2], selectablePieces[1], positions[3]);
        UpdateSelectablePreviewPiece(mSelectablePreviewPieces[3], nullptr, positions[4]);
    }
    
    mSelectablePiecesPreviousFrame = selectablePieces;
    mActivePiecePreviousFrame = activePiece;
}

void GameHud::UpdateActivePreviewPiece(PreviewPiece& previewPiece,
                                       const Piece* pieceType,
                                       const Pht::Vec3& position) {
    mActivePreviewPiece = &previewPiece.mSceneObjectPool->GetContainerSceneObject();
    UpdateSelectablePreviewPiece(previewPiece, pieceType, position);
}

void GameHud::UpdateSelectable0PreviewPiece(PreviewPiece& previewPiece,
                                            const Piece* pieceType,
                                            const Pht::Vec3& position) {
    mSelectable0PreviewPiece = &previewPiece.mSceneObjectPool->GetContainerSceneObject();
    UpdateSelectablePreviewPiece(previewPiece, pieceType, position);
}

void GameHud::UpdateSelectable1PreviewPiece(PreviewPiece& previewPiece,
                                            const Piece* pieceType,
                                            const Pht::Vec3& position) {
    mSelectable1PreviewPiece = &previewPiece.mSceneObjectPool->GetContainerSceneObject();
    UpdateSelectablePreviewPiece(previewPiece, pieceType, position);
}

void GameHud::UpdateSelectablePreviewPiece(PreviewPiece& previewPiece,
                                           const Piece* pieceType,
                                           const Pht::Vec3& position) {
    UpdatePreviewPiece(previewPiece, pieceType, position, selectablePiecesScale);
}

void GameHud::UpdateNextPreviewPiece(PreviewPiece& previewPiece,
                                     const Piece* pieceType,
                                     const Pht::Vec3& position) {
    UpdatePreviewPiece(previewPiece, pieceType, position, nextPiecesScale);
}

void GameHud::UpdatePreviewPiece(PreviewPiece& previewPiece,
                                 const Piece* pieceType,
                                 const Pht::Vec3& position,
                                 float slotScale) {
    previewPiece.mBombSceneObject = nullptr;
    previewPiece.mRowBombSceneObject = nullptr;
    
    auto& baseTransform = previewPiece.mSceneObject->GetTransform();
    baseTransform.SetPosition(position);
    
    auto& poolTransform = previewPiece.mSceneObjectPool->GetContainerSceneObject().GetTransform();
    poolTransform.SetScale(1.0f);
    poolTransform.SetPosition({0.0f, 0.0f, 0.0f});

    if (pieceType == nullptr) {
        previewPiece.mSceneObjectPool->SetIsActive(false);
        return;
    }
    
    previewPiece.mSceneObjectPool->SetIsActive(true);
    
    auto scale = slotScale * pieceType->GetPreviewCellSize() / cellSize;
    baseTransform.SetScale(scale);
    previewPiece.mScale = scale;
    previewPiece.mSceneObjectPool->ReclaimAll();
    
    auto pieceNumRows = pieceType->GetGridNumRows();
    auto pieceNumColumns = pieceType->GetGridNumColumns();
    auto& grid = pieceType->GetGrid(Rotation::Deg0);
    auto isBomb = pieceType->IsBomb();
    auto isRowBomb = pieceType->IsRowBomb();

    Pht::Vec3 lowerLeft {
        -static_cast<float>(pieceNumColumns) * cellSize / 2.0f + cellSize / 2.0f,
        -static_cast<float>(pieceNumRows) * cellSize / 2.0f + cellSize / 2.0f,
        0.0f
    };

    if (pieceType->NeedsRightAdjustmentInHud()) {
        lowerLeft.x += cellSize / 2.0f;
    } else if (pieceType->NeedsLeftAdjustmentInHud()) {
        lowerLeft.x -= cellSize / 2.0f;
    } else if (pieceType->NeedsDownAdjustmentInHud()) {
        lowerLeft.y -= cellSize / 2.0f;
    }

    for (auto row = 0; row < pieceNumRows; row++) {
        for (auto column = 0; column < pieceNumColumns; column++) {
            auto& subCell = grid[row][column].mFirstSubCell;
            auto blockKind = subCell.mBlockKind;
            if (blockKind != BlockKind::None) {
                auto& blockSceneObject = previewPiece.mSceneObjectPool->AccuireSceneObject();
                
                if (isBomb) {
                    blockSceneObject.SetRenderable(&mPieceResources.GetBombRenderableObject());
                    previewPiece.mBombSceneObject = &blockSceneObject;
                } else if (isRowBomb) {
                    blockSceneObject.SetRenderable(&mPieceResources.GetRowBombRenderableObject());
                    previewPiece.mRowBombSceneObject = &blockSceneObject;
                } else {
                    auto& blockRenderable =
                        mPieceResources.GetBlockRenderableObject(blockKind,
                                                                 subCell.mColor,
                                                                 BlockBrightness::Normal);
                    
                    blockSceneObject.SetRenderable(&blockRenderable);
                }
                
                Pht::Vec3 position {
                    static_cast<float>(column) * cellSize,
                    static_cast<float>(row) * cellSize,
                    0.0f
                };

                blockSceneObject.GetTransform().SetPosition(lowerLeft + position);
            }
        }
    }
}

void GameHud::OnSwitchPieceAnimationFinished() {
    auto& selectablePieces = mGameLogic.GetSelectablePieces();
    auto& selectablePositions = mSelectablePreviewPiecesRelativePositions;
    auto* activePiece = mGameLogic.GetPieceType();

    UpdateActivePreviewPiece(mSelectablePreviewPieces[0], activePiece, selectablePositions[1]);
    UpdateSelectable0PreviewPiece(mSelectablePreviewPieces[1], selectablePieces[0], selectablePositions[2]);
    UpdateSelectable1PreviewPiece(mSelectablePreviewPieces[2], selectablePieces[1], selectablePositions[3]);
    UpdateSelectablePreviewPiece(mSelectablePreviewPieces[3], nullptr, selectablePositions[4]);
}

void GameHud::OnNextPieceAnimationFinished() {
    auto& next2Pieces = mGameLogic.GetNextPieceGenerator().GetNext2Pieces();
    auto& nextPositions = mNextPreviewPiecesRelativePositions;

    UpdateNextPreviewPiece(mNextPreviewPieces[0], next2Pieces[0], nextPositions[0]);
    UpdateNextPreviewPiece(mNextPreviewPieces[1], next2Pieces[1], nextPositions[1]);
    UpdateNextPreviewPiece(mNextPreviewPieces[2], nullptr, nextPositions[2]);
}

void GameHud::RemovePreviewPiece(PreviewPieceIndex previewPieceIndex) {
    auto& selectablePositions = mSelectablePreviewPiecesRelativePositions;

    switch (previewPieceIndex) {
        case PreviewPieceIndex::Active:
            UpdateActivePreviewPiece(mSelectablePreviewPieces[0], nullptr, selectablePositions[1]);
            break;
        case PreviewPieceIndex::Selectable0:
            UpdateSelectable0PreviewPiece(mSelectablePreviewPieces[1], nullptr, selectablePositions[2]);
            break;
        case PreviewPieceIndex::Selectable1:
            UpdateSelectable1PreviewPiece(mSelectablePreviewPieces[2], nullptr, selectablePositions[3]);
            break;
        case PreviewPieceIndex::None:
            break;
    }
}

void GameHud::ShowPreviewPiece(PreviewPieceIndex previewPieceIndex) {
    auto& selectablePositions = mSelectablePreviewPiecesRelativePositions;
    
    switch (previewPieceIndex) {
        case PreviewPieceIndex::Active: {
            auto* activePiece = mGameLogic.GetPieceType();
            UpdateActivePreviewPiece(mSelectablePreviewPieces[0], activePiece, selectablePositions[1]);
            break;
        }
        case PreviewPieceIndex::Selectable0: {
            auto& selectablePieces = mGameLogic.GetSelectablePieces();
            UpdateSelectable0PreviewPiece(mSelectablePreviewPieces[1], selectablePieces[0], selectablePositions[2]);
            break;
        }
        case PreviewPieceIndex::Selectable1: {
            auto& selectablePieces = mGameLogic.GetSelectablePieces();
            UpdateSelectable1PreviewPiece(mSelectablePreviewPieces[2], selectablePieces[1], selectablePositions[3]);
            break;
        }
        case PreviewPieceIndex::None:
            break;
    }
}

void GameHud::ShowBlueMovesIcon() {
    mBlueMovesIcon->SetIsVisible(true);
    mYellowMovesIcon->SetIsVisible(false);
}

void GameHud::ShowYellowMovesIcon() {
    mYellowMovesIcon->SetIsVisible(true);
    mBlueMovesIcon->SetIsVisible(false);
}

void GameHud::SetNumMovesLeft(int movesLeft) {
    if (mNumMovesLeftSource == NumMovesLeftSource::Self) {
        mMovesLeftSelf = movesLeft;
    }
}

#include "BlastRadiusAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "OfflineRasterizer.hpp"
#include "IImage.hpp"
#include "ISceneManager.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto opacityCurveAmplitude {0.2f};
    constexpr auto frequency {0.7f};
    constexpr auto opacityTarget {1.0f - opacityCurveAmplitude};
    constexpr auto fadeInTime {0.23f};
    
    Pht::StaticVector<Pht::Vec2, 20> scalePoints {
        {0.0f, 0.0f},
        {0.1f, 0.005f},
        {0.2f, 0.01f},
        {0.3f, 0.02f},
        {0.35f, 0.035f},
        {0.4f, 0.05f},
        {0.45f, 0.065f},
        {0.5f, 0.08f},
        {0.55f, 0.115f},
        {0.6f, 0.15f},
        {0.65f, 0.225f},
        {0.7f, 0.3f},
        {0.75f, 0.41f},
        {0.8f, 0.52f},
        {0.85f, 0.62f},
        {0.9f, 0.7f},
        {0.95f, 0.87f},
        {1.0f, 1.0f},
    };

    void DrawEdgeBigBlast(Pht::OfflineRasterizer& rasterizer, float squareSide, float cellSize) {
        const auto edgeLength {cellSize * 1.1f};
        const auto edgeWidth {0.09f};
        const Pht::Vec4 edgeColor {1.0f, 1.0f, 1.0f, 1.0f};
        
        Pht::Vec2 lowerLeft1 {squareSide - edgeLength, 0.0f};
        Pht::Vec2 upperRight1 {squareSide, edgeWidth};
        rasterizer.DrawRectangle(upperRight1, lowerLeft1, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft2 {squareSide - edgeWidth, 0.0f};
        Pht::Vec2 upperRight2 {squareSide, edgeLength};
        rasterizer.DrawRectangle(upperRight2, lowerLeft2, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft3 {squareSide - edgeWidth, squareSide - edgeLength};
        Pht::Vec2 upperRight3 {squareSide, squareSide};
        rasterizer.DrawRectangle(upperRight3, lowerLeft3, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft4 {squareSide - edgeLength, squareSide - edgeWidth};
        Pht::Vec2 upperRight4 {squareSide, squareSide};
        rasterizer.DrawRectangle(upperRight4, lowerLeft4, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft5 {0.0f, squareSide - edgeWidth};
        Pht::Vec2 upperRight5 {edgeLength, squareSide};
        rasterizer.DrawRectangle(upperRight5, lowerLeft5, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft6 {0.0f, squareSide - edgeLength};
        Pht::Vec2 upperRight6 {edgeWidth, squareSide};
        rasterizer.DrawRectangle(upperRight6, lowerLeft6, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft7 {0.0f, 0.0f};
        Pht::Vec2 upperRight7 {edgeWidth, edgeLength};
        rasterizer.DrawRectangle(upperRight7, lowerLeft7, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft8 {0.0f, 0.0f};
        Pht::Vec2 upperRight8 {edgeLength, edgeWidth};
        rasterizer.DrawRectangle(upperRight8, lowerLeft8, edgeColor, Pht::DrawOver::Yes);
    }

    void DrawEdge(Pht::OfflineRasterizer& rasterizer, float squareSide, float cellSize) {
        const auto edgeLength {cellSize * 0.5f};
        const auto edgeWidth {0.09f};
        const Pht::Vec4 edgeColor {1.0f, 1.0f, 1.0f, 1.0f};
        
        Pht::Vec2 lowerLeft1 {squareSide - edgeLength - cellSize, 0.0f};
        Pht::Vec2 upperRight1 {squareSide - cellSize, edgeWidth};
        rasterizer.DrawRectangle(upperRight1, lowerLeft1, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft2 {squareSide - edgeWidth - cellSize, 0.0f};
        Pht::Vec2 upperRight2 {squareSide - cellSize, edgeLength};
        rasterizer.DrawRectangle(upperRight2, lowerLeft2, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft3 {squareSide - edgeLength, cellSize};
        Pht::Vec2 upperRight3 {squareSide, edgeWidth + cellSize};
        rasterizer.DrawRectangle(upperRight3, lowerLeft3, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft4 {squareSide - edgeWidth, cellSize};
        Pht::Vec2 upperRight4 {squareSide, cellSize + edgeLength};
        rasterizer.DrawRectangle(upperRight4, lowerLeft4, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft5 {squareSide - edgeWidth, squareSide - cellSize - edgeLength};
        Pht::Vec2 upperRight5 {squareSide, squareSide - cellSize};
        rasterizer.DrawRectangle(upperRight5, lowerLeft5, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft6 {squareSide - edgeLength, squareSide - cellSize - edgeWidth};
        Pht::Vec2 upperRight6 {squareSide, squareSide - cellSize};
        rasterizer.DrawRectangle(upperRight6, lowerLeft6, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft7 {squareSide - cellSize - edgeWidth, squareSide - edgeLength};
        Pht::Vec2 upperRight7 {squareSide - cellSize, squareSide};
        rasterizer.DrawRectangle(upperRight7, lowerLeft7, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft8 {squareSide - cellSize - edgeLength, squareSide - edgeWidth};
        Pht::Vec2 upperRight8 {squareSide - cellSize, squareSide};
        rasterizer.DrawRectangle(upperRight8, lowerLeft8, edgeColor, Pht::DrawOver::Yes);
        
        Pht::Vec2 lowerLeft9 {cellSize, squareSide - edgeWidth};
        Pht::Vec2 upperRight9 {cellSize + edgeLength, squareSide};
        rasterizer.DrawRectangle(upperRight9, lowerLeft9, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft10 {cellSize, squareSide - edgeLength};
        Pht::Vec2 upperRight10 {cellSize + edgeWidth, squareSide};
        rasterizer.DrawRectangle(upperRight10, lowerLeft10, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft11 {0.0f, squareSide - cellSize - edgeWidth};
        Pht::Vec2 upperRight11 {edgeLength, squareSide - cellSize};
        rasterizer.DrawRectangle(upperRight11, lowerLeft11, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft12 {0.0f, squareSide - cellSize - edgeLength};
        Pht::Vec2 upperRight12 {edgeWidth, squareSide - cellSize};
        rasterizer.DrawRectangle(upperRight12, lowerLeft12, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft13 {0.0f, cellSize};
        Pht::Vec2 upperRight13 {edgeWidth, cellSize + edgeLength};
        rasterizer.DrawRectangle(upperRight13, lowerLeft13, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft14 {0.0f, cellSize};
        Pht::Vec2 upperRight14 {edgeLength, cellSize + edgeWidth};
        rasterizer.DrawRectangle(upperRight14, lowerLeft14, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft15 {cellSize, 0.0f};
        Pht::Vec2 upperRight15 {cellSize + edgeWidth, edgeLength};
        rasterizer.DrawRectangle(upperRight15, lowerLeft15, edgeColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft16 {cellSize, 0.0f};
        Pht::Vec2 upperRight16 {cellSize + edgeLength, edgeWidth};
        rasterizer.DrawRectangle(upperRight16, lowerLeft16, edgeColor, Pht::DrawOver::Yes);
    }
    
    void DrawStripes(Pht::OfflineRasterizer& rasterizer, float squareSide) {
        const Pht::Vec4 fillColor {1.0f, 1.0f, 1.0f, 0.15f};
        const auto numStripes {3.5f};
        const auto stripeStep {squareSide / numStripes};
        const auto stripeWidth {(stripeStep / 2.0f) / std::sqrt(2.0f)};
        
        for (auto pos {0.0f}; pos < squareSide; pos += stripeStep) {
            Pht::Vec2 trapezoidLowerLeft {0.0f, squareSide - pos};
            Pht::Vec2 trapezoidUpperRight {pos, squareSide};
            rasterizer.DrawTiltedTrapezoid315(trapezoidUpperRight,
                                              trapezoidLowerLeft,
                                              stripeWidth,
                                              fillColor);
        }

        for (auto pos {0.0f}; pos < squareSide; pos += stripeStep) {
            Pht::Vec2 trapezoidLowerLeft {pos, 0.0f};
            Pht::Vec2 trapezoidUpperRight {squareSide, squareSide - pos};
            rasterizer.DrawTiltedTrapezoid45(trapezoidUpperRight,
                                             trapezoidLowerLeft,
                                             stripeWidth,
                                             fillColor);
        }
    }
    
    void EraseCornersFromStripes(Pht::OfflineRasterizer& rasterizer,
                                 float squareSide,
                                 float cellSize) {
        const Pht::Vec4 fillColor {0.0f, 0.0f, 0.0f, 0.0f};
        
        Pht::Vec2 lowerLeft1 {squareSide - cellSize, 0.0f};
        Pht::Vec2 upperRight1 {squareSide, cellSize};
        rasterizer.DrawRectangle(upperRight1, lowerLeft1, fillColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft2 {squareSide - cellSize, squareSide - cellSize};
        Pht::Vec2 upperRight2 {squareSide, squareSide};
        rasterizer.DrawRectangle(upperRight2, lowerLeft2, fillColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft3 {0.0f, squareSide - cellSize};
        Pht::Vec2 upperRight3 {cellSize, squareSide};
        rasterizer.DrawRectangle(upperRight3, lowerLeft3, fillColor, Pht::DrawOver::Yes);

        Pht::Vec2 lowerLeft4 {0.0f, 0.0f};
        Pht::Vec2 upperRight4 {cellSize, cellSize};
        rasterizer.DrawRectangle(upperRight4, lowerLeft4, fillColor, Pht::DrawOver::Yes);
    }
}

BlastRadiusAnimation::BlastRadiusAnimation(Pht::IEngine& engine, GameScene& scene) :
    mScene {scene} {
    
    auto cellSize {scene.GetCellSize()};
    auto squareSide {cellSize * 5.0f};
    Pht::Vec2 coordinateSystemSize {squareSide, squareSide};
    auto& renderer {engine.GetRenderer()};

    auto& renderBufferSize {renderer.GetRenderBufferSize()};
    auto& frustumSize {renderer.GetOrthographicFrustumSize()};
    
    auto xScaleFactor {static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)};
    auto yScaleFactor {static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)};
    
    Pht::IVec2 imageSize {
        static_cast<int>(squareSide * xScaleFactor),
        static_cast<int>(squareSide * yScaleFactor)
    };
    
    auto rasterizer {std::make_unique<Pht::OfflineRasterizer>(coordinateSystemSize, imageSize)};
    
    DrawStripes(*rasterizer, squareSide);
    DrawEdgeBigBlast(*rasterizer, squareSide, cellSize);
    mBigBombRadiusSceneObject = CreateSceneObject(*rasterizer->ProduceImage(), squareSide, engine);
    
    rasterizer->ClearBuffer();
    
    DrawStripes(*rasterizer, squareSide);
    EraseCornersFromStripes(*rasterizer, squareSide, cellSize);
    DrawEdge(*rasterizer, squareSide, cellSize);
    mBombRadiusSceneObject = CreateSceneObject(*rasterizer->ProduceImage(), squareSide, engine);
}

std::unique_ptr<Pht::SceneObject> BlastRadiusAnimation::CreateSceneObject(const Pht::IImage& image,
                                                                          float squareSide,
                                                                          Pht::IEngine& engine) {
    Pht::Material imageMaterial {image, Pht::GenerateMipmap::No};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    auto& sceneManager {engine.GetSceneManager()};
    return sceneManager.CreateSceneObject(Pht::QuadMesh {squareSide, squareSide},
                                          imageMaterial,
                                          mSceneResources);
}

void BlastRadiusAnimation::Init() {
    mScene.GetFieldBlocksContainer().AddChild(*mBombRadiusSceneObject);
    mScene.GetFieldBlocksContainer().AddChild(*mBigBombRadiusSceneObject);
    
    Stop();
}

void BlastRadiusAnimation::Start(Kind kind) {
    mState = State::FadingIn;
    mActiveKind = kind;
    mTime = 0.0f;
    
    switch (kind) {
        case Kind::Bomb:
            mBombRadiusSceneObject->SetIsVisible(true);
            mBigBombRadiusSceneObject->SetIsVisible(false);
            break;
        case Kind::BigBomb:
            mBigBombRadiusSceneObject->SetIsVisible(true);
            mBombRadiusSceneObject->SetIsVisible(false);
            break;
    }
}

void BlastRadiusAnimation::Stop() {
    mState = State::Inactive;
    mTime = 0.0f;
    mBombRadiusSceneObject->SetIsVisible(false);
    mBigBombRadiusSceneObject->SetIsVisible(false);
}

void BlastRadiusAnimation::SetPosition(const Pht::Vec2& position) {
    const auto cellSize {mScene.GetCellSize()};

    Pht::Vec3 positionInField {
        position.x * cellSize + cellSize / 2.0f,
        position.y * cellSize + cellSize / 2.0f,
        mScene.GetBlastRadiusAnimationZ()
    };
    
    mBombRadiusSceneObject->GetTransform().SetPosition(positionInField);
    mBigBombRadiusSceneObject->GetTransform().SetPosition(positionInField);
}

void BlastRadiusAnimation::Update(float dt) {
    switch (mState) {
        case State::FadingIn:
            UpdateInFadingInState(dt);
            break;
        case State::Active:
            UpdateInActiveState(dt);
            break;
        case State::Inactive:
            break;
    }
}

void BlastRadiusAnimation::UpdateInFadingInState(float dt) {
    mTime += dt;
    
    SetOpacity(opacityTarget * mTime / fadeInTime);
    
    auto normalizedTime {(fadeInTime - mTime) / fadeInTime};
    auto scale {1.0f + 1.0f * Pht::Lerp(normalizedTime, scalePoints)};

    SetScale(scale);

    if (mTime > fadeInTime) {
        mState = State::Active;
        SetOpacity(opacityTarget);
        SetScale(1.0f);
    }
}

void BlastRadiusAnimation::UpdateInActiveState(float dt) {
    mTime += dt;
    
    auto opacity {
        1.0f - opacityCurveAmplitude + opacityCurveAmplitude * std::sin(2.0f * 3.1415f * frequency * mTime)
    };
    
    SetOpacity(opacity);
}

void BlastRadiusAnimation::SetOpacity(float opacity) {
    switch (mActiveKind) {
        case Kind::Bomb:
            mBombRadiusSceneObject->GetRenderable()->GetMaterial().SetOpacity(opacity);
            break;
        case Kind::BigBomb:
            mBigBombRadiusSceneObject->GetRenderable()->GetMaterial().SetOpacity(opacity);
            break;
    }
}

void BlastRadiusAnimation::SetScale(float scale) {
    switch (mActiveKind) {
        case Kind::Bomb:
            mBombRadiusSceneObject->GetTransform().SetScale(scale);
            break;
        case Kind::BigBomb:
            mBigBombRadiusSceneObject->GetTransform().SetScale(scale);
            break;
    }
}

bool BlastRadiusAnimation::IsActive() const {
    switch (mState) {
        case State::FadingIn:
        case State::Active:
            return true;
        case State::Inactive:
            return false;
    }
}

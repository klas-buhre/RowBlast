#include "SphereMesh.hpp"

using namespace Pht;

SphereMesh::SphereMesh(float radius) :
    SphereMesh {radius, Vec2{5, 8}} {}

SphereMesh::SphereMesh(float radius, const IVec2& divisions) :
    ParametricSurface {divisions, Vec2{Pi, TwoPi}, Vec2{1.0f, 1.0f}},
    mRadius {radius} {}

SphereMesh::SphereMesh(float radius, const Vec2& textureCount) :
    SphereMesh {radius, textureCount, Vec3{0.0f, 0.0f, 0.0f}} {}

SphereMesh::SphereMesh(float radius, const Vec2& textureCount, const Vec3& translation) :
    ParametricSurface {IVec2{20, 20}, Vec2{Pi, TwoPi}, textureCount},
    mRadius {radius},
    mTranslation {translation} {}

Vec3 SphereMesh::Evaluate(const Vec2& domain) const {
    auto u {domain.x};
    auto v {domain.y};
    auto x {static_cast<float>(mRadius * sin(u) * cos(v)) + mTranslation.x};
    auto y {static_cast<float>(mRadius * cos(u)) + mTranslation.y};
    auto z {static_cast<float>(mRadius * -sin(u) * sin(v)) + mTranslation.z};
    return {x, y, z};
}

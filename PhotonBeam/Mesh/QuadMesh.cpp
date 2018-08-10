#include "QuadMesh.hpp"

#include <assert.h>

#include "MeshUtils.hpp"

using namespace Pht;

QuadMesh::QuadMesh(float xSize, float ySize, const Optional<std::string>& name) :
    QuadMesh {xSize, ySize, Vec2{1.0f, 1.0f}, 0.0f, name} {}

QuadMesh::QuadMesh(float xSize, float ySize, float tilt, const Optional<std::string>& name) :
    QuadMesh {xSize, ySize, Vec2{1.0f, 1.0f}, tilt, name} {}

QuadMesh::QuadMesh(float xSize,
                   float ySize,
                   const Vec2& textureCount,
                   float tilt,
                   const Optional<std::string>& name) :
    mName {name},
    mXSize {xSize},
    mYSize {ySize},
    mTilt {tilt},
    mTextureCount {textureCount} {}

QuadMesh::QuadMesh(const Vertices& vertices, const Optional<std::string>& name) :
    mName {name},
    mVertices {vertices} {}

Optional<std::string> QuadMesh::GetName() const {
    return {};
}

VertexBuffer QuadMesh::GetVertices(VertexFlags flags) const {
    VertexBuffer vertexBuffer {4, 6, flags};
    
    if (mXSize || mYSize) {
        auto halfX {mXSize / 2.0f};
        auto halfY {mYSize / 2.0f};
        
        MeshUtils::QuadVertices vertices {
            {-halfX, -halfY, 0.0f},
            {halfX, -halfY, 0.0f},
            {halfX + mTilt, halfY, 0.0f},
            {-halfX + mTilt, halfY, 0.0f}
        };

        auto textureCoords {MeshUtils::CreateQuadTextureCoords(mTextureCount)};
        MeshUtils::CreateQuad(vertexBuffer, vertices, textureCoords);
    } else {
        assert(flags.mColors);

        vertexBuffer.BeginFace();

        vertexBuffer.Write(mVertices.mV0.mVertex, mVertices.mV0.mColor);
        vertexBuffer.Write(mVertices.mV1.mVertex, mVertices.mV1.mColor);
        vertexBuffer.Write(mVertices.mV2.mVertex, mVertices.mV2.mColor);
        vertexBuffer.Write(mVertices.mV3.mVertex, mVertices.mV3.mColor);

        vertexBuffer.AddIndex(0);
        vertexBuffer.AddIndex(1);
        vertexBuffer.AddIndex(2);
        vertexBuffer.AddIndex(2);
        vertexBuffer.AddIndex(3);
        vertexBuffer.AddIndex(0);
    }
    
    return vertexBuffer;
}

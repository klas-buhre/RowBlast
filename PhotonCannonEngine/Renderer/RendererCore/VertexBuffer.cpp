#include "VertexBuffer.hpp"

using namespace Pht;

VertexBuffer::VertexBuffer(int vertexCount, int indexCount, VertexFlags flags) :
    mFlags {flags},
    mFloatsPerVertex {3} {
    
    if (mFlags.mNormals) {
        mFloatsPerVertex += 3;
    }
    if (mFlags.mTextureCoords) {
        mFloatsPerVertex += 2;
    }
    if (mFlags.mColors) {
        mFloatsPerVertex += 4;
    }
    if (mFlags.mPointSizes) {
        mFloatsPerVertex += 1;
    }
    
    mVertexBuffer.resize(vertexCount * mFloatsPerVertex);
    mVertexBufferPtr = &mVertexBuffer[0];
    mTriangleIndices.reserve(indexCount);
}

void VertexBuffer::BeginFace() {
    mFaceBeginVertex = mVerticesWritten;
}

void VertexBuffer::Reset() {
    mVertexBufferPtr = &mVertexBuffer[0];
    mVerticesWritten = 0;
    BeginFace();
}

void VertexBuffer::Write(const Vec3& vertex, const Vec3& normal, const Vec2& textureCoord) {
    ResizeIfNeeded();
    
    mVertexBufferPtr = vertex.Write(mVertexBufferPtr);
    
    if (mFlags.mNormals) {
        mVertexBufferPtr = normal.Write(mVertexBufferPtr);
    }
    
    if (mFlags.mTextureCoords) {
        mVertexBufferPtr = textureCoord.Write(mVertexBufferPtr);
    }
    
    mVerticesWritten++;
}

void VertexBuffer::Write(const Vec3& vertex, const Vec4& color, float pointSize) {
    ResizeIfNeeded();

    mVertexBufferPtr = vertex.Write(mVertexBufferPtr);
    
    if (mFlags.mColors) {
        mVertexBufferPtr = color.Write(mVertexBufferPtr);
    }
    
    if (mFlags.mPointSizes) {
        *mVertexBufferPtr = pointSize;
        ++mVertexBufferPtr;
    }
    
    mVerticesWritten++;
}

void VertexBuffer::AddIndex(unsigned short index) {
    mTriangleIndices.push_back(mFaceBeginVertex + index);
}

const float* VertexBuffer::GetVertexBuffer() const {
    return &mVertexBuffer[0];
}

const unsigned short* VertexBuffer::GetIndexBuffer() const {
    return &mTriangleIndices[0];
}

int VertexBuffer::GetVertexBufferSize() const {
    return mVerticesWritten * mFloatsPerVertex;
}

int VertexBuffer::GetIndexBufferSize() const {
    return static_cast<int>(mTriangleIndices.size());
}

void VertexBuffer::ResizeIfNeeded() {
    if (mVerticesWritten * mFloatsPerVertex >= mVertexBuffer.size()) {
        mVertexBuffer.resize(mVertexBuffer.size() * 2);
        mVertexBufferPtr = &mVertexBuffer[0] + (mVerticesWritten * mFloatsPerVertex);
    }
}

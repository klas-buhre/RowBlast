#include "VertexBuffer.hpp"

using namespace Pht;

VertexBuffer::VertexBuffer(int vertexCapacity, int indexCapacity, const VertexFlags& attributeFlags) :
    mFlags {attributeFlags},
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
    
    mVertexBuffer.resize(vertexCapacity * mFloatsPerVertex);
    mVertexWritePtr = mVertexBuffer.data();
    mTriangleIndices.resize(indexCapacity);
}

VertexBuffer::VertexBuffer(const VertexBuffer& other) {
    Copy(other);
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& other) {
    if (&other == this) {
        return *this;
    }
    
    Copy(other);
    return *this;
}

void VertexBuffer::Copy(const VertexBuffer& other) {
    mFlags = other.mFlags;
    mFloatsPerVertex = other.mFloatsPerVertex;
    mVertexBuffer = other.mVertexBuffer;
    mTriangleIndices = other.mTriangleIndices;
    mSurfaceBeginVertex = other.mSurfaceBeginVertex;
    mNumVertices = other.mNumVertices;
    mNumIndices = other.mNumIndices;
    mVertexWritePtr = mVertexBuffer.data() + (mNumVertices * mFloatsPerVertex);
}

void VertexBuffer::BeginSurface() {
    mSurfaceBeginVertex = mNumVertices;
}

void VertexBuffer::Reset() {
    mVertexWritePtr = mVertexBuffer.data();
    mNumVertices = 0;
    mNumIndices = 0;
    BeginSurface();
}

void VertexBuffer::Write(const Vec3& position, const Vec3& normal, const Vec2& textureCoord) {
    ResizeIfNeeded();
    
    mVertexWritePtr = position.Write(mVertexWritePtr);
    
    if (mFlags.mNormals) {
        mVertexWritePtr = normal.Write(mVertexWritePtr);
    }
    
    if (mFlags.mTextureCoords) {
        mVertexWritePtr = textureCoord.Write(mVertexWritePtr);
    }
    
    ++mNumVertices;
}

void VertexBuffer::Write(const Vec3& position, const Vec2& textureCoord, const Vec4& color) {
    ResizeIfNeeded();

    mVertexWritePtr = position.Write(mVertexWritePtr);

    if (mFlags.mTextureCoords) {
        mVertexWritePtr = textureCoord.Write(mVertexWritePtr);
    }

    if (mFlags.mColors) {
        mVertexWritePtr = color.Write(mVertexWritePtr);
    }

    ++mNumVertices;
}

void VertexBuffer::Write(const Vec3& position, const Vec4& color, float pointSize) {
    ResizeIfNeeded();

    mVertexWritePtr = position.Write(mVertexWritePtr);
    
    if (mFlags.mColors) {
        mVertexWritePtr = color.Write(mVertexWritePtr);
    }
    
    if (mFlags.mPointSizes) {
        *mVertexWritePtr++ = pointSize;
    }
    
    ++mNumVertices;
}

void VertexBuffer::AddIndex(uint16_t index) {
    if (mNumIndices >= mTriangleIndices.size()) {
        assert(!"Vertex buffer realocation!");
        mTriangleIndices.resize(mTriangleIndices.size() * 2);
    }
    
    mTriangleIndices[mNumIndices] = mSurfaceBeginVertex + index;
    ++mNumIndices;
}

const float* VertexBuffer::GetVertexBuffer() const {
    return mVertexBuffer.data();
}

const uint16_t* VertexBuffer::GetIndexBuffer() const {
    return mTriangleIndices.data();
}

float* VertexBuffer::GetVertexBuffer() {
    return mVertexBuffer.data();
}

uint16_t* VertexBuffer::GetIndexBuffer() {
    return mTriangleIndices.data();
}

const float* VertexBuffer::GetPastVertexBufferCapacity() const {
    auto* lastElement = &mVertexBuffer.back();
    return ++lastElement;
}

const uint16_t* VertexBuffer::GetPastIndexBufferCapacity() const {
    auto* lastElement = &mTriangleIndices.back();
    return ++lastElement;
}

int VertexBuffer::GetVertexBufferSize() const {
    return mNumVertices * mFloatsPerVertex;
}

int VertexBuffer::GetIndexBufferSize() const {
    return mNumIndices;
}

void VertexBuffer::ResizeIfNeeded() {
    if (mNumVertices * mFloatsPerVertex >= mVertexBuffer.size()) {
        assert(!"Vertex buffer realocation!");
        mVertexBuffer.resize(mVertexBuffer.size() * 2);
        mVertexWritePtr = mVertexBuffer.data() + (mNumVertices * mFloatsPerVertex);
    }
}

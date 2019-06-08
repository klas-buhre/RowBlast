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
    mVertexBufferPtr = &mVertexBuffer[0];
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
    mFaceBeginVertex = other.mFaceBeginVertex;
    mNumVertices = other.mNumVertices;
    mNumIndices = other.mNumIndices;
    mVertexBufferPtr = &mVertexBuffer[0] + (mNumVertices * mFloatsPerVertex);
}

void VertexBuffer::BeginFace() {
    mFaceBeginVertex = mNumVertices;
}

void VertexBuffer::Reset() {
    mVertexBufferPtr = &mVertexBuffer[0];
    mNumVertices = 0;
    mNumIndices = 0;
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
    
    ++mNumVertices;
}

void VertexBuffer::Write(const Vec3& vertex, const Vec2& textureCoord, const Vec4& color) {
    ResizeIfNeeded();

    mVertexBufferPtr = vertex.Write(mVertexBufferPtr);

    if (mFlags.mTextureCoords) {
        mVertexBufferPtr = textureCoord.Write(mVertexBufferPtr);
    }

    if (mFlags.mColors) {
        mVertexBufferPtr = color.Write(mVertexBufferPtr);
    }

    ++mNumVertices;
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
    
    ++mNumVertices;
}

void VertexBuffer::AddIndex(unsigned short index) {
    if (mNumIndices >= mTriangleIndices.size()) {
        assert(!"Vertex buffer realocation!");
        mTriangleIndices.resize(mTriangleIndices.size() * 2);
    }
    
    mTriangleIndices[mNumIndices] = mFaceBeginVertex + index;
    ++mNumIndices;
}

const float* VertexBuffer::GetVertexBuffer() const {
    return &mVertexBuffer[0];
}

const unsigned short* VertexBuffer::GetIndexBuffer() const {
    return &mTriangleIndices[0];
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
        mVertexBufferPtr = &mVertexBuffer[0] + (mNumVertices * mFloatsPerVertex);
    }
}

#ifndef VertexBuffer_hpp
#define VertexBuffer_hpp

#include <vector>

#include "Vector.hpp"

namespace Pht {
    struct VertexFlags {
        bool mNormals {false};
        bool mTextureCoords {false};
        bool mColors {false};
        bool mPointSizes {false};
        
        bool operator==(const VertexFlags& other) const {
            return mNormals == other.mNormals && mTextureCoords == other.mTextureCoords &&
                   mColors == other.mColors && mPointSizes == other.mPointSizes;
        }
        
        bool operator!=(const VertexFlags& other) const {
            return !(*this == other);
        }
    };
    
    class VertexBuffer {
    public:
        VertexBuffer(int vertexCapacity, int indexCapacity, const VertexFlags& attributeFlags);
        VertexBuffer(const VertexBuffer& other);
        VertexBuffer& operator=(const VertexBuffer& other);

        void BeginFace();
        void Reset();
        void Write(const Vec3& vertex, const Vec3& normal, const Vec2& textureCoord);
        void Write(const Vec3& vertex, const Vec2& textureCoord, const Vec4& color);
        void Write(const Vec3& vertex, const Vec4& color, float pointSize = 0);
        void AddIndex(unsigned short index);
        const float* GetVertexBuffer() const;
        const unsigned short* GetIndexBuffer() const;
        int GetVertexBufferSize() const;
        int GetIndexBufferSize() const;
        
        int GetNumVertices() const {
            return mNumVertices;
        }
        
        const VertexFlags& GetAttributeFlags() const {
            return mFlags;
        }

    private:
        void Copy(const VertexBuffer& other);
        void ResizeIfNeeded();
        
        VertexFlags mFlags;
        int mFloatsPerVertex;
        float* mVertexBufferPtr {nullptr};
        std::vector<float> mVertexBuffer;
        std::vector<unsigned short> mTriangleIndices;
        unsigned short mFaceBeginVertex {0};
        unsigned short mNumVertices {0};
        unsigned short mNumIndices {0};
    };
}

#endif

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

        void BeginSurface();
        void Reset();
        void Write(const Vec3& position, const Vec3& normal, const Vec2& textureCoord);
        void Write(const Vec3& position, const Vec2& textureCoord, const Vec4& color);
        void Write(const Vec3& position, const Vec4& color, float pointSize = 0);
        void AddIndex(uint16_t index);
        const float* GetVertexBuffer() const;
        const uint16_t* GetIndexBuffer() const;
        float* GetVertexBuffer();
        uint16_t* GetIndexBuffer();
        const float* GetPastVertexBufferCapacity() const;
        const uint16_t* GetPastIndexBufferCapacity() const;
        int GetVertexBufferSize() const;
        int GetIndexBufferSize() const;
        
        int GetNumVertices() const {
            return mNumVertices;
        }
        
        int GetNumIndices() const {
            return mNumIndices;
        }
        
        const VertexFlags& GetAttributeFlags() const {
            return mFlags;
        }

    private:
        void Copy(const VertexBuffer& other);
        void ResizeIfNeeded();
        
        VertexFlags mFlags;
        int mFloatsPerVertex;
        float* mVertexWritePtr {nullptr};
        std::vector<float> mVertexBuffer;
        std::vector<uint16_t> mTriangleIndices;
        uint16_t mSurfaceBeginVertex {0};
        unsigned short mNumVertices {0};
        unsigned short mNumIndices {0};
    };
}

#endif

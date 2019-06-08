#ifndef VboCache_hpp
#define VboCache_hpp

#include <string>
#include <memory>

#include <OpenGLES/ES3/gl.h>

#include "VertexBuffer.hpp"

namespace Pht {
    enum class GenerateIndexBuffer {
        Yes,
        No
    };
    
    class GpuVbo {
    public:
        GpuVbo(GenerateIndexBuffer generateIndexBuffer);
        ~GpuVbo();
        
        GLuint GetVertexBufferId() const {
            return mVertexBufferId;
        }
        
        GLuint GetIndexBufferId() const {
            return mIndexBufferId;
        }
        
        int GetIndexCount() const {
            return mIndexCount;
        }
        
        int GetPointCount() const {
            return mPointCount;
        }
        
        uint32_t GetId() const {
            return mId;
        }
        
        void SetCpuSideBuffer(std::unique_ptr<VertexBuffer> buffer) {
            mCpuSideBuffer = std::move(buffer);
        }
        
        const VertexBuffer* GetCpuSideBuffer() const {
            return mCpuSideBuffer.get();
        }

    private:
        friend class RenderableObject;
        
        static uint32_t mIdCounter;
        
        uint32_t mId {mIdCounter++};
        GLuint mVertexBufferId {0};
        GLuint mIndexBufferId {0};
        int mIndexCount {0};
        int mPointCount {0};
        std::unique_ptr<VertexBuffer> mCpuSideBuffer;
    };
    
    namespace VboCache {
        std::shared_ptr<GpuVbo> Get(const std::string& meshName);
        void Add(const std::string& meshName, std::shared_ptr<GpuVbo> vbo);
    }
}

#endif

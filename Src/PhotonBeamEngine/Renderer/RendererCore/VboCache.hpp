#ifndef VboCache_hpp
#define VboCache_hpp

#include <string>
#include <memory>

#include <OpenGLES/ES3/gl.h>

namespace Pht {
    enum class GenerateIndexBuffer {
        Yes,
        No
    };
    
    class Vbo {
    public:
        Vbo(GenerateIndexBuffer generateIndexBuffer);
        ~Vbo();
        
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

    private:
        friend class RenderableObject;
        
        static uint32_t mIdCounter;
        
        uint32_t mId {mIdCounter++};
        GLuint mVertexBufferId {0};
        GLuint mIndexBufferId {0};
        int mIndexCount {0};
        int mPointCount {0};
    };
    
    namespace VboCache {
        std::shared_ptr<Vbo> Get(const std::string& meshName);
        void Add(const std::string& meshName, std::shared_ptr<Vbo> vbo);
    }
}

#endif

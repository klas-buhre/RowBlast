#ifndef VboCache_hpp
#define VboCache_hpp

#include <string>
#include <memory>

#include <OpenGLES/ES3/gl.h>

namespace Pht {
    class Vbo {
    public:
        enum class GenerateIndexBuffer {
            Yes,
            No
        };
        
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
        
    private:
        friend class RenderableObject;
        
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

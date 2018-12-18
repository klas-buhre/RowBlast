#ifndef OpenAlBuffer_hpp
#define OpenAlBuffer_hpp

#include <memory>

#include <OpenAl/al.h>

namespace Pht {
    class OpenAlBuffer {
    public:
        OpenAlBuffer(ALuint handle);
        ~OpenAlBuffer();
        
        ALuint GetHandle() const {
            return mHandle;
        }
        
    private:
        ALuint mHandle {0};
    };
    
    std::unique_ptr<OpenAlBuffer> CreateOpenAlBuffer(const std::string& filename);
}

#endif

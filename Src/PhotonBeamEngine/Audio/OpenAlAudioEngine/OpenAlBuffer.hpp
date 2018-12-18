#ifndef OpenAlBuffer_hpp
#define OpenAlBuffer_hpp

#include <memory>

#include <OpenAl/al.h>

namespace Pht {
    class OpenAlBuffer {
    public:
        static std::unique_ptr<OpenAlBuffer> Create(const std::string& filename);

        ~OpenAlBuffer();
        
        ALuint GetHandle() const {
            return mHandle;
        }
        
        explicit OpenAlBuffer(ALuint handle);
        
    private:
        ALuint mHandle {0};
    };
}

#endif

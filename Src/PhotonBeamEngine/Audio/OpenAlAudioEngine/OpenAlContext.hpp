#ifndef OpenAlContext_hpp
#define OpenAlContext_hpp

#include <OpenAl/alc.h>

namespace Pht {
    class OpenAlContext {
    public:
        OpenAlContext(ALCdevice *device);
        ~OpenAlContext();
        
    private:
        ALCcontext *mHandle {nullptr};
    };
}

#endif

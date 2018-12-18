#ifndef OpenAlContext_hpp
#define OpenAlContext_hpp

#include <OpenAl/alc.h>

namespace Pht {
    class OpenAlContext {
    public:
        explicit OpenAlContext(ALCdevice *device);
        ~OpenAlContext();
        
        void SetIsCurrent(bool isCurrent);
        void SetIsSuspended(bool isSuspended);
        
    private:
        bool IsCurrent() const;
        
        ALCcontext *mHandle {nullptr};
        bool mIsSuspended {false};
    };
}

#endif

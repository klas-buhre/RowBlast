#ifndef OpenAlSource_hpp
#define OpenAlSource_hpp

#include <memory>

#include <OpenAl/al.h>

namespace Pht {
    class OpenAlBuffer;

    class OpenAlSource {
    public:
        static std::unique_ptr<OpenAlSource> Create(const OpenAlBuffer& buffer);
    
        ~OpenAlSource();
        
        void Play();
        void Stop();
        void SetGain(float gain);
        void SetPitch(float pitch);
        void SetLoop(bool loop);
        
        ALuint GetHandle() const {
            return mHandle;
        }
        
        explicit OpenAlSource(ALuint handle);
        
    private:
        bool IsPlaying() const;
        
        ALuint mHandle {0};
    };
}

#endif

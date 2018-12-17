#ifndef OpenAlSound_hpp
#define OpenAlSound_hpp

#include "ISound.hpp"

namespace Pht {
    class OpenAlSound: public ISound {
    public:
        OpenAlSound(const std::string& filename, int maxSources);
        
        void Play() override;
        void Stop() override;
        void SetGain(float gain) override;
        void SetPitch(float pitch) override;
        void SetLoop(bool loop) override;

    private:
        
    };
}

#endif

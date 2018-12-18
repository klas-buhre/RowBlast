#ifndef OpenAlSound_hpp
#define OpenAlSound_hpp

#include <memory>
#include <vector>

#include "ISound.hpp"
#include "OpenAlBuffer.hpp"
#include "OpenAlSource.hpp"

namespace Pht {
    class OpenAlSound: public ISound {
    public:
        static std::unique_ptr<OpenAlSound> Create(const std::string& filename, int maxSources);
        
        void Play() override;
        void Stop() override;
        void SetGain(float gain) override;
        void SetPitch(float pitch) override;
        void SetLoop(bool loop) override;

    private:
        std::unique_ptr<OpenAlBuffer> mBuffer;
        std::vector<std::unique_ptr<OpenAlSource>> mSources;
        int mCurrentSourceIndex {0};
    };
}

#endif

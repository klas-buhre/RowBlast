#ifndef ISound_hpp
#define ISound_hpp

#include <memory>

namespace Pht {
    class ISound {
    public:
        virtual ~ISound() {}

        virtual void Play() = 0;
        virtual void Pause() = 0;
        virtual void Stop() = 0;
        virtual void SetLoop(bool loop) = 0;
    };
    
    std::unique_ptr<Pht::ISound> LoadSound(const std::string& filename);
}

#endif

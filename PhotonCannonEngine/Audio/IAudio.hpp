#ifndef IAudio_hpp
#define IAudio_hpp

#include <string>

namespace Pht {
    class ISound;

    class IAudio {
    public:
        virtual ~IAudio() {}

        virtual void AddSound(const std::string& filename) = 0;
        virtual ISound* GetSound(const std::string& filename) const = 0;
        virtual void PlaySound(const std::string& filename) = 0;
        virtual void AddMusic(const std::string& filename) = 0;
        virtual ISound* GetMusic(const std::string& filename) const = 0;
        virtual void PlayMusic(const std::string& filename) = 0;
        virtual void EnableSound() = 0;
        virtual void DisableSound() = 0;
        virtual void EnableMusic() = 0;
        virtual void DisableMusic() = 0;
        virtual bool IsSoundEnabled() = 0;
        virtual bool IsMusicEnabled() = 0;
    };
}

#endif

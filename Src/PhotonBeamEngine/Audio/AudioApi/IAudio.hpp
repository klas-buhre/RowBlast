#ifndef IAudio_hpp
#define IAudio_hpp

#include <string>

namespace Pht {
    class ISound;
    class IMusicTrack;
    
    using AudioResourceId = uint32_t;

    class IAudio {
    public:
        virtual ~IAudio() {}

        virtual void AddSound(const std::string& filename) = 0;
        virtual ISound* GetSound(const std::string& filename) const = 0;
        virtual void PlaySound(const std::string& filename) = 0;
        virtual void LoadMusicTrack(const std::string& filename, AudioResourceId resourceId) = 0;
        virtual void FreeMusicTrack(AudioResourceId resourceId) = 0;
        virtual IMusicTrack* GetMusicTrack(AudioResourceId resourceId) const = 0;
        virtual void PlayMusicTrack(AudioResourceId resourceId) = 0;
        virtual void EnableSound() = 0;
        virtual void DisableSound() = 0;
        virtual void EnableMusic() = 0;
        virtual void DisableMusic() = 0;
        virtual bool IsSoundEnabled() = 0;
        virtual bool IsMusicEnabled() = 0;
    };
}

#endif

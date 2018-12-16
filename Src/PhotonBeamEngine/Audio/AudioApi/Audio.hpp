#ifndef Audio_hpp
#define Audio_hpp

#include <memory>
#include <unordered_map>

#include "IAudio.hpp"
#include "ISound.hpp"
#include "IMusicTrack.hpp"

namespace Pht {
    class Audio: public IAudio {
    public:
        void AddSound(const std::string& filename) override;
        ISound* GetSound(const std::string& filename) const override;
        void PlaySound(const std::string& filename) override;
        void LoadMusicTrack(const std::string& filename, AudioResourceId resourceId) override;
        void FreeMusicTrack(AudioResourceId resourceId) override;
        IMusicTrack* GetMusicTrack(AudioResourceId resourceId) const override;
        void PlayMusicTrack(AudioResourceId resourceId, float fadeInDuration) override;
        void FadeOutActiveTrack(float fadeOutDuration) override;
        void EnableSound() override;
        void DisableSound() override;
        void EnableMusic() override;
        void DisableMusic() override;
        bool IsSoundEnabled() override;
        bool IsMusicEnabled() override;

    private:
        std::unordered_map<std::string, std::unique_ptr<ISound>> mSounds;
        std::unordered_map<AudioResourceId, std::unique_ptr<IMusicTrack>> mTracks;
        IMusicTrack* mActiveTrack {nullptr};
        bool mIsSoundEnabled {false};
        bool mIsMusicEnabled {true};
    };
}

#endif

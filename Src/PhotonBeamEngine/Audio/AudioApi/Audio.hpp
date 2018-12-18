#ifndef Audio_hpp
#define Audio_hpp

#include <memory>
#include <unordered_map>

#include "IAudio.hpp"
#include "ISound.hpp"
#include "IMusicTrack.hpp"
#include "IAudioEngine.hpp"

namespace Pht {
    class Audio: public IAudio {
    public:
        Audio();

        void LoadSound(const std::string& filename,
                       int maxSources,
                       AudioResourceId resourceId) override;
        ISound* GetSound(AudioResourceId resourceId) override;
        void PlaySound(AudioResourceId resourceId) override;
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
        std::unique_ptr<IAudioEngine> mAudioEngine;
        std::unordered_map<AudioResourceId, std::unique_ptr<ISound>> mSounds;
        std::unordered_map<AudioResourceId, std::unique_ptr<IMusicTrack>> mTracks;
        IMusicTrack* mActiveTrack {nullptr};
        bool mIsSoundEnabled {false};
        bool mIsMusicEnabled {true};
    };
}

#endif

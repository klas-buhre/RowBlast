#ifndef Audio_hpp
#define Audio_hpp

#include <memory>
#include <unordered_map>

#include "IAudio.hpp"
#include "ISound.hpp"

namespace Pht {
    class Audio: public IAudio {
    public:
        void AddSound(const std::string& filename) override;
        ISound* GetSound(const std::string& filename) const override;
        void PlaySound(const std::string& filename) override;
        void AddMusic(const std::string& filename) override;
        ISound* GetMusic(const std::string& filename) const override;
        void PlayMusic(const std::string& filename) override;
        void EnableSound() override;
        void DisableSound() override;
        void EnableMusic() override;
        void DisableMusic() override;
        bool IsSoundEnabled() override;
        bool IsMusicEnabled() override;

    private:
        std::unordered_map<std::string, std::unique_ptr<ISound>> mSounds;
        std::unordered_map<std::string, std::unique_ptr<ISound>> mTracks;
        ISound* mActiveTrack {nullptr};
        bool mIsSoundEnabled {true};
        bool mIsMusicEnabled {true};
    };
}

#endif

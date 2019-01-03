#ifndef AudioResources_hpp
#define AudioResources_hpp

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    enum class MusicTrackId {
        Map,
        Game1,
        Game2
    };
    
    enum class SoundId {
        Bomb,
        Logo,
        ButtonClick,
        StartGame,
        LeaveTitle
    };
    
    void LoadAudioResouces(Pht::IEngine& engine);
    void LoadAndPlayGameMusicTrack1(Pht::IEngine& engine);
    void LoadAndPlayGameMusicTrack2(Pht::IEngine& engine);
    void PlayMapMusicTrack(Pht::IEngine& engine);
}

#endif

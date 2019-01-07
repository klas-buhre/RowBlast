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
        Logo,
        ButtonClick,
        StartGame,
        LeaveTitle,
        SlidingTextWhoosh1,
        SlidingTextWhoosh2,
        Whoosh,
        LandPiece,
        ClearBlocksBrickImpact1,
        ClearBlocksBrickImpact2,
        ClearBlocksBrickImpact3,
        Fantastic,
        Awesome,
        Combo1,
        Combo2
    };
    
    void LoadAudioResouces(Pht::IEngine& engine);
    void LoadAndPlayGameMusicTrack1(Pht::IEngine& engine);
    void LoadAndPlayGameMusicTrack2(Pht::IEngine& engine);
    void PlayMapMusicTrack(Pht::IEngine& engine);
    void PlayClearBlocksSound(Pht::IEngine& engine);
}

#endif

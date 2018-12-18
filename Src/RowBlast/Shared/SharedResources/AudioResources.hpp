#ifndef AudioResources_hpp
#define AudioResources_hpp

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    enum class MusicTrackId {
        Map,
        Game
    };
    
    enum class SoundId {
        Blip,
        Bomb
    };
    
    void LoadAudioResouces(Pht::IEngine& engine);
}

#endif

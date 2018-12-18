#include "AudioResources.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"

using namespace RowBlast;

void RowBlast::LoadAudioResouces(Pht::IEngine& engine) {
    auto& audio {engine.GetAudio()};
    
    audio.LoadMusicTrack("map.mp4", static_cast<Pht::AudioResourceId>(MusicTrackId::Map));
    audio.LoadMusicTrack("game.mp4", static_cast<Pht::AudioResourceId>(MusicTrackId::Game));
    
    audio.LoadSound("Blip.wav", 3, static_cast<Pht::AudioResourceId>(SoundId::Blip));
    audio.LoadSound("Bomb.wav", 10, static_cast<Pht::AudioResourceId>(SoundId::Bomb));
}

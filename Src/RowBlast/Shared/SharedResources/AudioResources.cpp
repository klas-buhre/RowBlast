#include "AudioResources.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fadeInDuration {0.22f};
}

void RowBlast::LoadAudioResouces(Pht::IEngine& engine) {
    auto& audio {engine.GetAudio()};
    
    audio.LoadMusicTrack("map.mp4", static_cast<Pht::AudioResourceId>(MusicTrackId::Map));
    audio.LoadMusicTrack("game_track1.mp4", static_cast<Pht::AudioResourceId>(MusicTrackId::Game1));
    
    audio.LoadSound("Blip.wav", 3, static_cast<Pht::AudioResourceId>(SoundId::Blip));
    audio.LoadSound("Bomb.wav", 10, static_cast<Pht::AudioResourceId>(SoundId::Bomb));
}

void RowBlast::LoadAndPlayGameMusicTrack1(Pht::IEngine& engine) {
    auto& audio {engine.GetAudio()};
    auto track1AudioResourceId {static_cast<Pht::AudioResourceId>(MusicTrackId::Game1)};
    
    if (audio.GetMusicTrack(track1AudioResourceId) == nullptr) {
        audio.FreeMusicTrack(static_cast<Pht::AudioResourceId>(MusicTrackId::Game2));
        audio.LoadMusicTrack("game_track1.mp4", track1AudioResourceId);
    }

    audio.PlayMusicTrack(track1AudioResourceId, fadeInDuration);
}

void RowBlast::LoadAndPlayGameMusicTrack2(Pht::IEngine& engine) {
    auto& audio {engine.GetAudio()};
    auto track2AudioResourceId {static_cast<Pht::AudioResourceId>(MusicTrackId::Game2)};
    
    if (audio.GetMusicTrack(track2AudioResourceId) == nullptr) {
        audio.FreeMusicTrack(static_cast<Pht::AudioResourceId>(MusicTrackId::Game1));
        audio.LoadMusicTrack("game_track2.mp4", track2AudioResourceId);
    }

    audio.PlayMusicTrack(track2AudioResourceId, fadeInDuration);
}

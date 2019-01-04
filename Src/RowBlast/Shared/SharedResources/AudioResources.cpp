#include "AudioResources.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"

using namespace RowBlast;

namespace {
    constexpr auto gameMusicVolume {0.36f};
    constexpr auto mapMusicVolume {0.55f};
    constexpr auto fadeInDuration {0.22f};
    constexpr auto maxSoundGain {1.0f};
}

void RowBlast::LoadAudioResouces(Pht::IEngine& engine) {
    auto& audio {engine.GetAudio()};
    
    audio.LoadSound("logo.wav", 1, maxSoundGain, static_cast<Pht::AudioResourceId>(SoundId::Logo));
    audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Logo));
    
    audio.LoadMusicTrack("map.mp4", static_cast<Pht::AudioResourceId>(MusicTrackId::Map));
    audio.LoadMusicTrack("game_track1.mp4", static_cast<Pht::AudioResourceId>(MusicTrackId::Game1));
    
    audio.LoadSound("Bomb.wav", 10, maxSoundGain, static_cast<Pht::AudioResourceId>(SoundId::Bomb));
    audio.LoadSound("button_click.wav", 1, maxSoundGain, static_cast<Pht::AudioResourceId>(SoundId::ButtonClick));
    audio.LoadSound("start_game.wav", 1, maxSoundGain, static_cast<Pht::AudioResourceId>(SoundId::StartGame));
    audio.LoadSound("leave_title.wav", 1, 0.65f, static_cast<Pht::AudioResourceId>(SoundId::LeaveTitle));
    audio.LoadSound("sliding_text_animation.wav", 1, maxSoundGain, static_cast<Pht::AudioResourceId>(SoundId::SlidingTextAnimation));
}

void RowBlast::PlayMapMusicTrack(Pht::IEngine& engine) {
    auto& audio {engine.GetAudio()};
    audio.SetMusicVolume(mapMusicVolume);
    audio.PlayMusicTrack(static_cast<Pht::AudioResourceId>(MusicTrackId::Map), fadeInDuration);
}

void RowBlast::LoadAndPlayGameMusicTrack1(Pht::IEngine& engine) {
    auto& audio {engine.GetAudio()};
    audio.SetMusicVolume(gameMusicVolume);
    
    auto track1AudioResourceId {static_cast<Pht::AudioResourceId>(MusicTrackId::Game1)};
    
    if (audio.GetMusicTrack(track1AudioResourceId) == nullptr) {
        audio.FreeMusicTrack(static_cast<Pht::AudioResourceId>(MusicTrackId::Game2));
        audio.LoadMusicTrack("game_track1.mp4", track1AudioResourceId);
    }

    audio.PlayMusicTrack(track1AudioResourceId, fadeInDuration);
}

void RowBlast::LoadAndPlayGameMusicTrack2(Pht::IEngine& engine) {
    auto& audio {engine.GetAudio()};
    audio.SetMusicVolume(gameMusicVolume);

    auto track2AudioResourceId {static_cast<Pht::AudioResourceId>(MusicTrackId::Game2)};
    
    if (audio.GetMusicTrack(track2AudioResourceId) == nullptr) {
        audio.FreeMusicTrack(static_cast<Pht::AudioResourceId>(MusicTrackId::Game1));
        audio.LoadMusicTrack("game_track2.mp4", track2AudioResourceId);
    }

    audio.PlayMusicTrack(track2AudioResourceId, fadeInDuration);
}

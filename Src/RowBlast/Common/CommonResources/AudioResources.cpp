#include "AudioResources.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"
#include "ISound.hpp"

using namespace RowBlast;

namespace {
    constexpr auto gameMusicVolume = 0.36f;
    constexpr auto mapMusicVolume = 0.55f;
    constexpr auto fadeInDuration = 0.22f;
    constexpr auto maxSoundGain = 1.0f;
    constexpr auto clearBlocksGain = 0.83f;
}

void RowBlast::LoadAudioResouces(Pht::IEngine& engine) {
    auto& audio = engine.GetAudio();
    
    audio.LoadSound("logo.wav", 1, maxSoundGain, static_cast<Pht::AudioResourceId>(SoundId::Logo));
    audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Logo));
    
    audio.LoadMusicTrack("map.mp4", static_cast<Pht::AudioResourceId>(MusicTrackId::Map));
    audio.LoadMusicTrack("game_track1.mp4", static_cast<Pht::AudioResourceId>(MusicTrackId::Game1));

    audio.LoadSound("button_click.wav",
                    1,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::ButtonClick));
    audio.LoadSound("start_game.wav",
                    1,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::StartGame));
    audio.LoadSound("leave_title.wav",
                    1,
                    0.65f,
                    static_cast<Pht::AudioResourceId>(SoundId::LeaveTitle));
    audio.LoadSound("sliding_text_whoosh1.wav",
                    1,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::SlidingTextWhoosh1));
    audio.LoadSound("sliding_text_whoosh2.wav",
                    1,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::SlidingTextWhoosh2));
    audio.LoadSound("drop_whoosh.wav",
                    1,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::DropWhoosh));
    audio.LoadSound("rotate_whoosh.wav",
                    5,
                    0.53f,
                    static_cast<Pht::AudioResourceId>(SoundId::RotateWhoosh));
    audio.LoadSound("land_piece.wav",
                    1,
                    0.6f,
                    static_cast<Pht::AudioResourceId>(SoundId::LandPiece));
    audio.LoadSound("switch_piece.wav",
                    1,
                    0.8f,
                    static_cast<Pht::AudioResourceId>(SoundId::SwitchPiece));
    audio.LoadSound("other_moves.wav",
                    1,
                    0.475f,
                    static_cast<Pht::AudioResourceId>(SoundId::OtherMoves));
    audio.LoadSound("clear_blocks_brick_impact1.wav",
                    3,
                    clearBlocksGain,
                    static_cast<Pht::AudioResourceId>(SoundId::ClearBlocksBrickImpact1));
    audio.LoadSound("clear_blocks_brick_impact2.wav",
                    3,
                    clearBlocksGain,
                    static_cast<Pht::AudioResourceId>(SoundId::ClearBlocksBrickImpact2));
    audio.LoadSound("clear_blocks_brick_impact3.wav",
                    3,
                    clearBlocksGain,
                    static_cast<Pht::AudioResourceId>(SoundId::ClearBlocksBrickImpact3));
    audio.LoadSound("laser.wav",
                    5,
                    0.35f,
                    static_cast<Pht::AudioResourceId>(SoundId::Laser));
    audio.LoadSound("blast_bass.wav",
                    5,
                    0.5f,
                    static_cast<Pht::AudioResourceId>(SoundId::BlastBass));
    audio.LoadSound("explosion.wav",
                    5,
                    0.51f,
                    static_cast<Pht::AudioResourceId>(SoundId::Explosion));
    audio.LoadSound("fantastic.wav",
                    1,
                    0.8f,
                    static_cast<Pht::AudioResourceId>(SoundId::Fantastic));
    audio.LoadSound("awesome.wav",
                    1,
                    0.8f,
                    static_cast<Pht::AudioResourceId>(SoundId::Awesome));
    audio.LoadSound("combo1.wav",
                    1,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::Combo1));
    audio.LoadSound("combo2.wav",
                    1,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::Combo2));
    audio.LoadSound("add_move.wav",
                    1,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::AddMove));
    audio.LoadSound("all_cleared.wav",
                    1,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::AllCleared));
    audio.LoadSound("level_completed.wav",
                    1,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::LevelCompleted));
    audio.LoadSound("star.wav",
                    3,
                    0.75f,
                    static_cast<Pht::AudioResourceId>(SoundId::Star));
    audio.LoadSound("fireworks1.wav",
                    5,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::Fireworks1));
    audio.LoadSound("fireworks2.wav",
                    5,
                    maxSoundGain,
                    static_cast<Pht::AudioResourceId>(SoundId::Fireworks2));
}

void RowBlast::PlayMapMusicTrack(Pht::IEngine& engine) {
    auto& audio = engine.GetAudio();
    audio.SetMusicVolume(mapMusicVolume);
    audio.PlayMusicTrack(static_cast<Pht::AudioResourceId>(MusicTrackId::Map), fadeInDuration);
}

void RowBlast::LoadAndPlayGameMusicTrack1(Pht::IEngine& engine) {
    auto& audio = engine.GetAudio();
    audio.SetMusicVolume(gameMusicVolume);
    
    auto track1AudioResourceId = static_cast<Pht::AudioResourceId>(MusicTrackId::Game1);
    if (audio.GetMusicTrack(track1AudioResourceId) == nullptr) {
        audio.FreeMusicTrack(static_cast<Pht::AudioResourceId>(MusicTrackId::Game2));
        audio.LoadMusicTrack("game_track1.mp4", track1AudioResourceId);
    }

    audio.PlayMusicTrack(track1AudioResourceId, fadeInDuration);
}

void RowBlast::LoadAndPlayGameMusicTrack2(Pht::IEngine& engine) {
    auto& audio = engine.GetAudio();
    audio.SetMusicVolume(gameMusicVolume);

    auto track2AudioResourceId = static_cast<Pht::AudioResourceId>(MusicTrackId::Game2);
    if (audio.GetMusicTrack(track2AudioResourceId) == nullptr) {
        audio.FreeMusicTrack(static_cast<Pht::AudioResourceId>(MusicTrackId::Game1));
        audio.LoadMusicTrack("game_track2.mp4", track2AudioResourceId);
    }

    audio.PlayMusicTrack(track2AudioResourceId, fadeInDuration);
}

void RowBlast::PlayClearBlocksSound(Pht::IEngine& engine) {
    auto& audio = engine.GetAudio();
    
    switch (std::rand() % 3) {
        case 0:
            audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::ClearBlocksBrickImpact1));
            break;
        case 1:
            audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::ClearBlocksBrickImpact2));
            break;
        case 2:
            audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::ClearBlocksBrickImpact3));
            break;
        default:
            assert(false);
            break;
    }
}

void RowBlast::PlayLaserSound(Pht::IEngine& engine) {
    auto& audio = engine.GetAudio();
    audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Laser));
    audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::BlastBass));
}

void RowBlast::PlayExplosionSound(Pht::IEngine& engine) {
    auto& audio = engine.GetAudio();
    audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Explosion));
    audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::BlastBass));
}

void RowBlast::PlayFireworksSound(Pht::IEngine& engine) {
    auto& audio = engine.GetAudio();
    
    if (audio.GetSound(static_cast<Pht::AudioResourceId>(SoundId::Star))->IsPlaying()) {
        return;
    }
    
    switch (std::rand() % 2) {
        case 0:
            audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Fireworks1));
            break;
        case 1:
            audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Fireworks2));
            break;
        default:
            assert(false);
            break;
    }
}

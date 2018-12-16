#include "Audio.hpp"

#include "ISound.hpp"

using namespace Pht;

void Audio::AddSound(const std::string& filename) {
    auto sound {LoadSound(filename)};
    
    if (sound) {
        mSounds[filename] = std::move(sound);
    }
}

ISound* Audio::GetSound(const std::string& filename) const {
    auto i {mSounds.find(filename)};
    
    if (i != std::end(mSounds)) {
        return i->second.get();
    }
    
    return nullptr;
}

void Audio::PlaySound(const std::string& filename) {
    if (!mIsSoundEnabled) {
        return;
    }
    
    auto* sound {GetSound(filename)};
    
    if (sound) {
        sound->Play();
    }
}

void Audio::LoadMusicTrack(const std::string& filename, AudioResourceId resourceId) {
    auto track {Pht::LoadMusicTrack(filename)};
    mTracks[resourceId] = std::move(track);
}

void Audio::FreeMusicTrack(AudioResourceId resourceId) {
    mTracks.erase(resourceId);
}

IMusicTrack* Audio::GetMusicTrack(AudioResourceId resourceId) const {
    auto i {mTracks.find(resourceId)};
    
    if (i != std::end(mTracks)) {
        return i->second.get();
    }
    
    return nullptr;
}

void Audio::PlayMusicTrack(AudioResourceId resourceId, float fadeInDuration) {
    auto* track {GetMusicTrack(resourceId)};
    
    if (track == nullptr) {
        return;
    }

    if (mActiveTrack) {
        mActiveTrack->Stop();
    }

    mActiveTrack = track;

    if (mIsMusicEnabled) {
        if (fadeInDuration == 0.0f) {
            track->SetVolume(1.0f);
            track->Play();
        } else {
            track->SetVolume(0.0f);
            track->Play();
            track->SetVolume(1.0f, fadeInDuration);
        }
    }
}

void Audio::FadeOutActiveTrack(float fadeOutDuration) {
    if (mActiveTrack) {
        mActiveTrack->SetVolume(0.0f, fadeOutDuration);
    }
}

void Audio::EnableSound() {
    mIsSoundEnabled = true;
}

void Audio::DisableSound()  {
    mIsSoundEnabled = false;
}

void Audio::EnableMusic()  {
    mIsMusicEnabled = true;
    
    if (mActiveTrack) {
        mActiveTrack->Play();
    }
}

void Audio::DisableMusic()  {
    mIsMusicEnabled = false;
    
    if (mActiveTrack) {
        mActiveTrack->Pause();
    }
}

bool Audio::IsSoundEnabled()  {
    return mIsSoundEnabled;
}

bool Audio::IsMusicEnabled()  {
    return mIsMusicEnabled;
}

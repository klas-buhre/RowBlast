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

void Audio::AddMusic(const std::string& filename) {
    auto track {LoadSound(filename)};
    
    if (track) {
        track->SetLoop(true);
        mTracks[filename] = std::move(track);
    }
}

ISound* Audio::GetMusic(const std::string& filename) const {
    auto i {mTracks.find(filename)};
    
    if (i != std::end(mTracks)) {
        return i->second.get();
    }
    
    return nullptr;
}

void Audio::PlayMusic(const std::string& filename) {
    if (!mIsMusicEnabled) {
        return;
    }
    
    auto* track {GetMusic(filename)};
    
    if (track == nullptr) {
        return;
    }
    
    if (mActiveTrack) {
        mActiveTrack->Stop();
    }
    
    mActiveTrack = track;
    mActiveTrack->Play();
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

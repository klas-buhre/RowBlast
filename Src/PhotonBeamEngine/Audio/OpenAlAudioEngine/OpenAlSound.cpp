#include "OpenAlSound.hpp"

using namespace Pht;

void OpenAlSound::Play() {
    mCurrentSourceIndex = (mCurrentSourceIndex + 1) % mSources.size();
    mSources[mCurrentSourceIndex]->Play();
}

void OpenAlSound::Stop() {
    for (auto& source: mSources) {
        source->Stop();
    }
}

void OpenAlSound::SetGain(float gain) {
    for (auto& source: mSources) {
        source->SetGain(gain);
    }
}

void OpenAlSound::SetPitch(float pitch) {
    for (auto& source: mSources) {
        source->SetPitch(pitch);
    }
}

void OpenAlSound::SetLoop(bool loop) {
    for (auto& source: mSources) {
        source->SetLoop(loop);
    }
}

std::unique_ptr<OpenAlSound> OpenAlSound::Create(const std::string& filename, int maxSources) {
    auto buffer {OpenAlBuffer::Create(filename)};
    if (buffer == nullptr) {
        return nullptr;
    }
    
    auto sound {std::make_unique<OpenAlSound>()};

    for (auto i {0}; i < maxSources; ++i) {
        auto source {OpenAlSource::Create(*buffer)};
        if (source == nullptr) {
            return nullptr;
        }
        
        sound->mSources.push_back(std::move(source));
    }
    
    sound->mBuffer = std::move(buffer);
    return sound;
}

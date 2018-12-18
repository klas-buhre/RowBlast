#include "OpenAlSound.hpp"

using namespace Pht;

OpenAlSound::OpenAlSound(const std::string& filename, int maxSources) {
    mBuffer = OpenAlBuffer::Create(filename);
    
    if (mBuffer) {
        
    }
}

void OpenAlSound::Play() {

}

void OpenAlSound::Stop() {

}

void OpenAlSound::SetGain(float gain) {

}

void OpenAlSound::SetPitch(float pitch) {

}

void OpenAlSound::SetLoop(bool loop) {

}

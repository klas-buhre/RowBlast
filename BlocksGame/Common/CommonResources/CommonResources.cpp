#include "CommonResources.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IAudio.hpp"

using namespace BlocksGame;

const std::string CommonResources::mBlipSound {"Blip.wav"};
const std::string CommonResources::mBombSound {"Bomb.wav"};

CommonResources::CommonResources(Pht::IEngine& engine) :
    mMaterials {engine},
    mHussarFontSize22 {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(22)},
    mHussarFontSize27 {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(27)},
    mHussarFontSize30 {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(35)} {
    
    AddSounds(engine.GetAudio());
}

void CommonResources::AddSounds(Pht::IAudio& audio) {
    audio.AddSound(mBlipSound);
    audio.AddSound(mBombSound);
}

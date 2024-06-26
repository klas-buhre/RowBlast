#include "SettingsService.hpp"

// Engine includes.
#include "JsonUtil.hpp"
#include "FileStorage.hpp"

using namespace RowBlast;

namespace {
    const std::string filename {"settings.dat"};
    const std::string controlTypeMember {"controlType"};
    const std::string isSoundEnabledMember {"isSoundEnabled"};
    const std::string isMusicEnabledMember {"isMusicEnabled"};
    const std::string isGhostPieceEnabledMember {"isGhostPieceEnabled"};
    const std::string clearRowsEffectMember {"clearRowsEffect"};
    
    std::string ToString(ControlType controlType) {
        switch (controlType) {
            case ControlType::Drag:
                return "Drag";
            case ControlType::Click:
                return "Click";
            case ControlType::Swipe:
                return "Swipe";
        }
    }
    
    ControlType ReadControlType(const rapidjson::Document& document) {
        auto controlType = Pht::Json::ReadString(document, controlTypeMember);
        
        // Need to check against "Gesture" since that was the name of the combined drag and swipe
        // controls in the 1.0.1 release.
        if (controlType == "Drag" || controlType == "Gesture") {
            return ControlType::Drag;
        }
        if (controlType == "Click") {
            return ControlType::Click;
        }
        if (controlType == "Swipe") {
            return ControlType::Swipe;
        }

        assert(!"Unsupported control type");
    }
    
    std::string ToString(ClearRowsEffect clearRowsEffect) {
        switch (clearRowsEffect) {
            case ClearRowsEffect::Shrink:
                return "Shrink";
            case ClearRowsEffect::Throw:
                return "Throw";
        }
    }
    
    ClearRowsEffect ReadClearRowsEffect(const rapidjson::Document& document) {
        auto clearRowsEffect = Pht::Json::ReadString(document, clearRowsEffectMember);
        if (clearRowsEffect == "Shrink") {
            return ClearRowsEffect::Shrink;
        }
        if (clearRowsEffect == "Throw") {
            return ClearRowsEffect::Throw;
        }
        
        assert(!"Unsupported clear rows effect");
    }
}

SettingsService::SettingsService() {
    LoadState();
}

void SettingsService::SetControlType(ControlType controlType) {
    mControlType = controlType;
    SaveState();
}

void SettingsService::SetIsGhostPieceEnabled(bool isGhostPieceEnabled) {
    mIsGhostPieceEnabled = isGhostPieceEnabled;
    SaveState();
}

void SettingsService::SetIsSoundEnabled(bool isSoundEnabled) {
    mIsSoundEnabled = isSoundEnabled;
    SaveState();
}

void SettingsService::SetIsMusicEnabled(bool isMusicEnabled) {
    mIsMusicEnabled = isMusicEnabled;
    SaveState();
}

void SettingsService::SetClearRowsEffect(ClearRowsEffect clearRowsEffect) {
    mClearRowsEffect = clearRowsEffect;
    SaveState();
}

void SettingsService::SaveState() {
    rapidjson::Document document;
    auto& allocator = document.GetAllocator();
    document.SetObject();
    
    Pht::Json::AddString(document, controlTypeMember, ToString(mControlType), allocator);
    Pht::Json::AddBool(document, isSoundEnabledMember, mIsSoundEnabled, allocator);
    Pht::Json::AddBool(document, isMusicEnabledMember, mIsMusicEnabled, allocator);
    Pht::Json::AddBool(document, isGhostPieceEnabledMember, mIsGhostPieceEnabled, allocator);
    Pht::Json::AddString(document, clearRowsEffectMember, ToString(mClearRowsEffect), allocator);

    std::string jsonString;
    Pht::Json::EncodeDocument(document, jsonString);
    Pht::FileStorage::Save(filename, jsonString);
}

bool SettingsService::LoadState() {
    std::string jsonString;
    if (!Pht::FileStorage::Load(filename, jsonString)) {
        return false;
    }
    
    rapidjson::Document document;
    Pht::Json::ParseDocument(document, jsonString);
    
    mControlType = ReadControlType(document);
    mIsSoundEnabled = Pht::Json::ReadBool(document, isSoundEnabledMember);
    mIsMusicEnabled = Pht::Json::ReadBool(document, isMusicEnabledMember);
    
    // mIsGhostPieceEnabled added in release 1.0.2. Need to check if it exists before reading it.
    if (document.HasMember(isGhostPieceEnabledMember.c_str())) {
        mIsGhostPieceEnabled = Pht::Json::ReadBool(document, isGhostPieceEnabledMember);
    }

    // mClearRowsEffect added in release 1.1.4. Need to check if it exists before reading it.
    if (document.HasMember(clearRowsEffectMember.c_str())) {
        mClearRowsEffect = ReadClearRowsEffect(document);
    }
    
    return true;
}

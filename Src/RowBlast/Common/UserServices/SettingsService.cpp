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
    
    std::string ToString(ControlType controlType) {
        switch (controlType) {
            case ControlType::Click:
                return "Click";
            case ControlType::Gesture:
                return "Gesture";
        }
    }
    
    ControlType ReadControlType(const rapidjson::Document& document) {
        auto controlType = Pht::Json::ReadString(document, controlTypeMember);
        if (controlType == "Click") {
            return ControlType::Click;
        }
        if (controlType == "Gesture") {
            return ControlType::Gesture;
        }

        assert(!"Unsupported control type");
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

void SettingsService::SaveState() {
    rapidjson::Document document;
    auto& allocator = document.GetAllocator();
    document.SetObject();
    
    Pht::Json::AddString(document, controlTypeMember, ToString(mControlType), allocator);
    Pht::Json::AddBool(document, isSoundEnabledMember, mIsSoundEnabled, allocator);
    Pht::Json::AddBool(document, isMusicEnabledMember, mIsMusicEnabled, allocator);
    Pht::Json::AddBool(document, isGhostPieceEnabledMember, mIsGhostPieceEnabled, allocator);

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

    return true;
}

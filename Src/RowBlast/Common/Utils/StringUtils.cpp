#include "StringUtils.hpp"

#include <cstdio>
#include <string>

using namespace RowBlast;

void StringUtils::FormatToMinutesAndSeconds(StaticStringBuffer& buffer,
                                            std::chrono::seconds numSeconds) {
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(numSeconds);
    auto minutesCount = static_cast<int>(minutes.count());
    auto seconds = numSeconds - minutes;
    auto secondsCount = static_cast<int>(seconds.count());
    
    std::snprintf(buffer.data(), buffer.size(), "%02d:%02d", minutesCount, secondsCount);
}

int StringUtils::WriteIntegerToString(int value, std::string& str, int positionInString) {
    constexpr auto bufSize = 64;
    char buffer[bufSize];
    std::snprintf(buffer, bufSize, "%d", value);
    auto numDigits = std::strlen(buffer);
    
    assert(str.size() >= numDigits + positionInString);

    for (auto i = 0; i < numDigits; ++i) {
        str[i + positionInString] = buffer[i];
    }
    
    return static_cast<int>(numDigits);
}

void StringUtils::FillStringWithSpaces(std::string& str) {
    auto strSize = str.size();
    for (auto i = 0; i < strSize; ++i) {
        str[i] = ' ';
    }
}

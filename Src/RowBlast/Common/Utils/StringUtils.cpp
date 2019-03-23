#include "StringUtils.hpp"

#include <cstdio>

using namespace RowBlast;

void StringUtils::FormatToMinutesAndSeconds(StaticStringBuffer& buffer,
                                            std::chrono::seconds numSeconds) {
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(numSeconds);
    auto minutesCount = static_cast<int>(minutes.count());
    auto seconds = numSeconds - minutes;
    auto secondsCount = static_cast<int>(seconds.count());
    
    std::snprintf(buffer.data(), buffer.size(), "%02d:%02d", minutesCount, secondsCount);
}

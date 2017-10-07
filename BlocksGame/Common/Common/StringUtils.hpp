#ifndef StringUtils_hpp
#define StringUtils_hpp

#include <array>
#include <chrono>
#include <assert.h>

namespace BlocksGame {
    using StaticStringBuffer = std::array<char, 64>;
    
    namespace StringUtils {
        void FormatToMinutesAndSeconds(StaticStringBuffer& buffer, std::chrono::seconds seconds);
        
        template<typename A, typename B>
        void StringCopy(A& a, int aOffset, B& b, int numChars) {
            assert(a.size() >= aOffset + numChars);
            assert(b.size() >= numChars);
            
            for (auto i {0}; i < numChars; ++i) {
                a[i + aOffset] = b[i];
            }
        }
    }
}

#endif

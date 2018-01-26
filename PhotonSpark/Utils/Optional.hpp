#ifndef Optional_hpp
#define Optional_hpp

#include <assert.h>

namespace Pht {
    template<typename T>
    class Optional {
    public:
        Optional() {}
        
        Optional(const T& value) :
            mHasValue {true},
            mValue {value} {}
        
        operator bool() const {
            return mHasValue;
        }
        
        bool HasValue() const {
            return mHasValue;
        }
        
        const T& GetValue() const {
            assert(mHasValue);
            return mValue;
        }
        
        T& GetValue() {
            assert(mHasValue);
            return mValue;
        }
        
    private:
        bool mHasValue {false};
        T mValue;
    };
}

#endif
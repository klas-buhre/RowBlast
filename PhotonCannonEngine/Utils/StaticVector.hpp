#ifndef StaticVector_hpp
#define StaticVector_hpp

#include <algorithm>
#include <assert.h>

namespace Pht {
    template<typename T, int Capacity>
    class StaticVector {
    public:
        void PushBack(const T& element) {
            assert(mSize + 1 <= Capacity);
            mData[mSize++] = element;
        }

        // Warning! The destructor on the erased element is not called. Only store store POD types.
        void Erase(int index) {
            --mSize;
            assert(mSize >= 0);
            if (index < mSize) {
                for (auto i {index}; i < mSize; ++i) {
                    mData[i] = mData[i + 1];
                }
            }
        }
        
        const T* Find(const T& element) const {
            for (auto i {0}; i < mSize; ++i) {
                if (mData[i] == element) {
                    return &mData[i];
                }
            }
            return nullptr;
        }
        
        template<typename Compare>
        void Sort(Compare compare) {
            std::sort(&mData[0], &mData[mSize], compare);
        }
        
        void Reverse() {
            std::reverse(&mData[0], &mData[mSize]);
        }
        
        T& At(int index) {
            assert(index >= 0 && index < mSize);
            return mData[index];
        }
        
        const T& At(int index) const {
            assert(index >= 0 && index < mSize);
            return mData[index];
        }

        T& Back() {
            return At(mSize - 1);
        }
        
        const T& Back() const {
            return At(mSize - 1);
        }
        
        int Size() const {
            return mSize;
        }
        
        bool IsEmpty() const {
            return mSize == 0;
        }
        
        void Clear() {
            mSize = 0;
        }
        
        T* GetData() {
            return mData;
        }
        
        // For range-based for loops.
        const T* begin() const {
            return &mData[0];
        }

        const T* end() const {
            return &mData[mSize];
        }

        T* begin() {
            return &mData[0];
        }

        T* end() {
            return &mData[mSize];
        }

    private:
        int mSize {0};
        T mData[Capacity];
    };
}

#endif


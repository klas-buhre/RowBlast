#ifndef PurchasingService_hpp
#define PurchasingService_hpp

namespace RowBlast {
    class PurchasingService {
    public:
        PurchasingService();
        
    private:
        void SaveState();
        bool LoadState();

        int mBalance {0};
    };
}

#endif

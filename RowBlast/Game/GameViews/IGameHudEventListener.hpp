#ifndef IGameHudEventListener_hpp
#define IGameHudEventListener_hpp

namespace RowBlast {
    class IGameHudEventListener {
    public:
        virtual ~IGameHudEventListener() {}
        
        virtual void OnSwitchButtonDown() = 0;
        virtual void OnSwitchButtonUp() = 0;
    };
}

#endif

#pragma once
#include "types.h"
#include "nn/os.hpp"

void setup_bf2();


namespace nn {
    namespace mem {
        class StandardAllocator {
            public:
            unsigned long GetTotalFreeSize() const;
            unsigned long GetAllocatableSize() const;
            void WalkAllocatedBlocks(int (*)(void*,unsigned long, void*), void*) const;
        }; //class StandardAllocator
    } //namespace mem

    namespace init {
        nn::mem::StandardAllocator* GetAllocator();
    } //namespace init

    namespace os {
        typedef u64 TimeSpan;

        nn::os::Tick GetSystemTick();
        TimeSpan ConvertToTimeSpan(nn::os::Tick);

    } //namespace os
} //namespace nn

namespace event { 
    class UIManager{}; 
} //namespace event

namespace mm::mtl {
    namespace HeapAllocUtil {
        uint64_t heap_walk_pool(void*, void(*)(void*, unsigned long, bool, void*, unsigned long), void*);

    } //namespace HeapAllocUtil

    template <size_t N>
    class FixStr {
        public:
        char string[N];
        size_t length;
    }; //class FixStr<N>

    template <class T>
    class Singleton {
        public:
        static T* getPtr();
    }; 
} //namespace mm::mtl

namespace ml {
    class ProcDesktop {
        public:
        static mm::mtl::FixStr<128>* getBuildRevision(void);
    }; //class ProcDesktop
} //namespace ml

namespace gf {
    typedef uint64_t GF_OBJ_HANDLE;
    typedef uint32_t SAVESLOT;

    class GfGameManager {
        public:
        bool isBattle();
        void enterBattle(bool);
        gf::GF_OBJ_HANDLE* getPartyLeader();

    }; //class GfGameManager


    class GfObjUtil {
        public:
        uintptr_t getObj(gf::GF_OBJ_HANDLE*);

    }; //class GfObjUtil

    namespace GfReqCommand {
        void reqAutoSave(gf::SAVESLOT);
    } //namespace GfReqCommand

} //namespace gf

namespace tl {
    class TitleMain {
        public:
        void returnTitle(gf::SAVESLOT);
    }; //class TitleMain

} //namespace tl

namespace btl {
    class BattleGlobal {
        public:
        char data[0x8400];
        bool isEnableBattle();
    }; //class BattleGlobal


    class BattleManager {
        
    }; //class BattleManager


    class CharacterManager {
        bool IsExistBattleEnemy(bool*, gf::GF_OBJ_HANDLE*, bool);

    }; //class CharacterManager

} //namespace btl
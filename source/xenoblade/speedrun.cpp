#include "xenoblade/speedrun.hpp"
#include "xenoblade/bf2.hpp"
#include "skyline-stuff/utils/util.hpp"
#include "skyline/utils/cpputils.hpp"

#include "types.h"

GENERATE_SYM_HOOK(loadingIcon, "_ZN2gf13GfMenuObjFade11dispLoadingEbt", 
		void, bool loadState, ushort param) {
	if (loadState)
		*(char*)(mm::mtl::Singleton<event::UIManager>::getPtr() + 0x14) |= 2;
	else
		*(char*)(mm::mtl::Singleton<event::UIManager>::getPtr() + 0x14) &= ~2;
	
	loadingIconBak(loadState, param);
}

void (*load210Bak)(bool, short);
void load210Replace(bool loadState, short param) {
	if (loadState)
		*(char*)(((u64(*)(void))(uintptr_t)(skyline::utils::g_MainTextAddr + 0x14d100))() + 0x14) |= 2;
	else
		*(char*)(((u64(*)(void))(uintptr_t)(skyline::utils::g_MainTextAddr + 0x14d100))() + 0x14) &= ~2;
	return load210Bak(loadState, param);
}

static int countCallback(void*, unsigned long, void* count) {
    *(int *)count += 1;
    return 1;
}

nn::mem::StandardAllocator* allocator = nullptr;
void memDiag() {
    int count = 0;
    allocator->WalkAllocatedBlocks(countCallback, &count);
	auto allocatable = allocator->GetAllocatableSize();
	auto free = allocator->GetTotalFreeSize();
	
    LOG("There are %d blocks, %lu allocatable space, and %lu free space", count, allocatable, free);
}

void setup_speedruns() {
    uintptr_t checkSymbols = (uintptr_t)ml::ProcDesktop::getBuildRevision;
    if (checkSymbols != 0) {
        LOG("Symbols Detected. Assuming Version 2.0.0 or lower");
        allocator = nn::init::GetAllocator();
	    loadingIconHook();
    }
    else {
        mm::mtl::FixStr<128>* rev_number = (mm::mtl::FixStr<128>*)(skyline::utils::g_MainTextAddr + 0xeceb04);
        if (strcmp(rev_number->string, "Rev.295513") == 0) {
            LOG("Version detected: 2.1.0");
            allocator = (nn::mem::StandardAllocator*)(skyline::utils::g_MainTextAddr + 0x15eb5f0);
            A64HookFunction((void*)(skyline::utils::g_MainTextAddr + 0x407428), (void*)load210Replace, (void**)&load210Bak);
        }
        else {
            LOG("Unknown version string: %s", rev_number->string);
        }
    }
    return;
    u64 sleepTick = 1e9;
    while (true) {
        memDiag();
        svcSleepThread(sleepTick);
    }
}
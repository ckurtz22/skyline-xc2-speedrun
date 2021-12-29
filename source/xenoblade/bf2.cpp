#include "xenoblade/bf2.hpp"
#include "skyline-stuff/utils/util.hpp"
#include "skyline-stuff/utils/debug_util.hpp"
#include "skyline-stuff/log/prepo.hpp"
#include "nn/hid.hpp"

#include <string>


GENERATE_SYM_HOOK(gameManagerUpdate, "_ZN2gf13GfGameManager6updateERKN2fw10UpdateInfoE", 
		void, uint8_t* thisObj, uint8_t* updateInfo) {
		
	constexpr auto ACTIVATE_KEYS = nn::hid::KEY_ZL | nn::hid::KEY_L;
	constexpr auto LOAD_TITLE	= ACTIVATE_KEYS | nn::hid::KEY_B;
	constexpr auto SAVE_GAME	= ACTIVATE_KEYS | nn::hid::KEY_A;

	static auto npadFullKeyStatePrevious = nn::hid::NpadFullKeyState{};
	static auto npadFullKeyState = nn::hid::NpadFullKeyState{};
	static auto keyComboJustPressed = [](decltype(nn::hid::NpadFullKeyState::Buttons) keyCombo) {
		return npadFullKeyState.Buttons == keyCombo and
		(npadFullKeyState.Buttons != npadFullKeyStatePrevious.Buttons);
	};
	nn::hid::GetNpadState(&npadFullKeyState, nn::hid::CONTROLLER_PLAYER_1);

	
	if (keyComboJustPressed(SAVE_GAME)) {
		LOG("save game requested");
		gf::GfReqCommand::reqAutoSave((gf::SAVESLOT)0x16);
	}
	
	if (keyComboJustPressed(LOAD_TITLE)) {
		LOG("load title requested");
		tl::TitleMain::returnTitle((gf::SAVESLOT)0x16);
	}

	npadFullKeyStatePrevious = npadFullKeyState;
	gameManagerUpdateBak(thisObj, updateInfo);
}

GENERATE_INL_HOOK(checkBreakChance, reinterpret_cast<void*>(skyline::utils::g_MainTextAddr + 0x46818)) {
	register float tmp asm ("s9");
	LOG("CheckReactionChance called: %f", tmp * 100);
	return;
}

GENERATE_INL_HOOK(checkHitChance, reinterpret_cast<void*>(skyline::utils::g_MainTextAddr + 0x68b44)) {
	register float tmp asm ("s8");
	LOG("CheckHitChance called: %f", tmp * 100);
	return;
}

GENERATE_SYM_HOOK(nnabort, "_ZN2nn4diag6detail9AbortImplEPKcS3_S3_iPKNS_6ResultES3_z", 
		void, char const* p1, char const* p2, char const* p3, int p4, int const* p5, char const* p6) {

	LOG("%s %s %s %d %x %s", p1, p2, p3, p4, *p5, p6);
	dbgutil::logStackTrace();
	while (true) svcSleepThread(1e9);
}

/*
void nninitStartup(void) {
	nn::os::MemoryInfo memInfo;
	nn::os::QueryMemoryInfo(&memInfo);
	unsigned long heapSize = memInfo.TotalMemorySize - (memInfo.UsedMemorySize & 0xffffffffffe00000);
	if (nn::os::SetMemoryHeapSize(heapSize) != 0) {
		mm::MMStdBase::mmAssert("result.IsSuccess()", "C:/Monolithsoft/BF2/prof/application/BF2/main.cpp", 0x4a7);
	}

	// main_detail::gCapableMemorySize = heapSize;
	
	unsigned long allocatedHeapSpace = heapSize - 0x6400000;
	void* heapBase;
	if (nn::os::AllocateMemoryBlock(&heapBase, allocatedHeapSpace) != 0) {
		mm::MMStdBase::mmAssert("result.IsSuccess()", "C:/Monolithsoft/BF2/prof/application/BF2/main.cpp", 0x4b3);
	}

	nn::init::InitializeAllocator(heapBase, allocatedHeapSpace);
	auto allocator = nn::Init::GetAllocator();
	if (allocator != nullptr) {
		allocator->GetAllocatableSize();
		allocator->GetTotalFreeSize();
	}
}
*/

namespace nn::oe { 
	int EnableGamePlayRecording(void* buffer, unsigned long size);
}

void setup_bf2() {
	// nnabortHook();
	// void* gameRecordRegion = nullptr;
	// nn::os::AllocateMemoryBlock((u64*)&gameRecordRegion, 0x6000000);
	// nn::oe::EnableGamePlayRecording(gameRecordRegion, 0x6000000);
	// nninitStartupHook();
	gameManagerUpdateHook();
	checkBreakChanceHook();
	checkHitChanceHook();
}

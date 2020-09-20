#include "xenoblade/bf2.hpp"
#include "skyline-stuff/utils/util.hpp"
#include "skyline-stuff/utils/debug_util.hpp"
#include "skyline-stuff/log/prepo.hpp"
#include "nn/hid.hpp"

#include <string>


static int slot = 0x16;
static int save_text_timer = 0;
static int load_text_timer = 0;
static int slot_text_timer = 0;

GENERATE_SYM_HOOK(gameManagerUpdate, "_ZN2gf13GfGameManager6updateERKN2fw10UpdateInfoE", 
		void, uint8_t* thisObj, uint8_t* updateInfo) {
	
	static bool funcLoaded = false;
	static uint8_t 	(*gameOverFunc)(void) 	= nullptr;
	static void 	(*saveGameFunc)(int) 	= nullptr;
	static void 	(*titleScreenFunc)(int)	= nullptr;
	static uint 	(*drawText)(int,int,const char*,...)	= nullptr;
	if (!funcLoaded) {
		nn::ro::LookupSymbol((uintptr_t*)&gameOverFunc, "_ZN2gf13GfPlayFactory14createGameOverEv");
		nn::ro::LookupSymbol((uintptr_t*)&saveGameFunc, "_ZN2gf12GfReqCommand11reqAutoSaveENS_8SAVESLOTE");
		nn::ro::LookupSymbol((uintptr_t*)&titleScreenFunc, "_ZN2tl9TitleMain11returnTitleEN2gf8SAVESLOTE");
		nn::ro::LookupSymbol((uintptr_t*)&drawText, "_ZN2fw5debug8drawFontEiiPKcz");
		funcLoaded = true;
	}
	
	constexpr auto ACTIVATE_KEYS = nn::hid::KEY_ZL | nn::hid::KEY_L;
	constexpr auto SLOT_UP		= ACTIVATE_KEYS | nn::hid::KEY_DUP;
	constexpr auto SLOT_DOWN	= ACTIVATE_KEYS | nn::hid::KEY_DDOWN;
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
		// LOG("save game requested");
		// memDiag();
		// save_text_timer = 90;
		// saveGameFunc(slot);
	}
	
	if (keyComboJustPressed(LOAD_TITLE)) {
		LOG("load title requested");
		load_text_timer = 90;
		titleScreenFunc(slot);
	}
	
	if (keyComboJustPressed(SLOT_UP)) {
		LOG("slot up requested");
		if (slot_text_timer == 0)
			slot_text_timer = 90;
		else if (slot++ == 0x16)
			slot = 0x0;
		//else if (slot > 0x9)
		//	slot = 0x16;
	}
	
	if (keyComboJustPressed(SLOT_DOWN)) {
		LOG("slot down requested");
		if (slot_text_timer == 0)
			slot_text_timer = 90;
		//else if (slot == 0x16)
		//	slot = 0x9;
		else if (slot-- == 0x0)
			slot = 0x16;
	}
	
	if (save_text_timer > 0) {
		save_text_timer--;
		drawText(0,20,"Saving slot %d", slot);
	}
	
	if (load_text_timer > 0) {
		load_text_timer--;
		drawText(0,40,"Loading slot %d", slot);
	}
	
	if (slot_text_timer > 0) {
		slot_text_timer--;
		drawText(0,0,"Slot %d", slot);
	}
	
	npadFullKeyStatePrevious = npadFullKeyState;
	gameManagerUpdateBak(thisObj, updateInfo);
}

struct DevFileParamTh {
	char* filePath;
};

struct DevFileAccessor {
	u64 vtable;
};

struct DevFileThImpl {
	char gap_0x0[0x5b0b0];
	DevFileAccessor *archiveAccessors[8];
	DevFileAccessor *fileAccessors[8];
	
};

GENERATE_SYM_HOOK(readFile, "_ZN2ml9DevFileTh4Impl8readFileERKNS_14DevFileParamThE", 
		uintptr_t, DevFileThImpl* thisObj, DevFileParamTh* fileParam) {

	if (fileParam->filePath[0] != '/')
		LOG("%s", fileParam->filePath);
	if (fileParam->filePath[0] == 's' && fileParam->filePath[1] == 'd' && fileParam->filePath[2] == ':') {
		LOG("%lx", (u64)thisObj - skyline::utils::g_MainHeapAddr);
		
		//return 0;
		for (int i = 0; i < 8; i++) {
			if (thisObj->fileAccessors[i] != 0)
				LOG("fileAccessors %d:\t0x%lx", i, thisObj->fileAccessors[i]->vtable-skyline::utils::g_MainTextAddr);
			if (thisObj->archiveAccessors[i] != 0)
				LOG("archiveAccessors %d:\t0x%lx", i, thisObj->archiveAccessors[i]->vtable-skyline::utils::g_MainTextAddr);
		}
		return 0;
	}
	
	return readFileBak(thisObj, fileParam);
}


GENERATE_SYM_HOOK(gameLoadConst, "_ZN2gf10GfGameLoadC1ENS_8SAVESLOTE", 
		void, uintptr_t thisObj, int saveSlot) {

	LOG("GfGameLoad constructor called");
	return gameLoadConstBak(thisObj, slot);
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



void setup_testing() {
	gameManagerUpdateHook();
	gameLoadConstHook();
	checkBreakChanceHook();
	checkHitChanceHook();
}

#include "xenoblade/speedrun.hpp"

void setup_bf2() {
	setup_testing();
	// setup_speedruns();
	// loggingHook();
}

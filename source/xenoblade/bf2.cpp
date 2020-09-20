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

void setup_testing() {
	gameManagerUpdateHook();
	checkBreakChanceHook();
	checkHitChanceHook();
}

#include "xenoblade/speedrun.hpp"

void setup_bf2() {
	setup_testing();
	// setup_speedruns();
}

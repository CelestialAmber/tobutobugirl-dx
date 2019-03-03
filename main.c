#include <gb/gb.h>
#include <gb/cgb.h>
#include "gamestate.h"
#include "main.h"
#include "ram.h"
#include "sound.h"
#include "mmlgb/driver/music.h"

#include "logos.h"
#include "intro.h"
#include "title.h"
#include "select.h"
#include "game.h"
#include "winscreen.h"
#include "highscore.h"
#include "unlocked.h"
#include "jukebox.h"
#include "ending.h"
#include "wipe.h"
#include "minigamescore.h"

const UBYTE RAM_SIG[8] = {'T','O','B','U','T','O','B','U'};

const UINT16 GS_SPR_PALETTE[] = { RGB(31, 31, 31), RGB(31, 31, 31), RGB(20, 20, 20), RGB(0, 0, 0) };

void initRAM(UBYTE force_clear) {
	UBYTE initialized, i;

	ENABLE_RAM_MBC1;
	SWITCH_RAM_MBC1(0);

	// Check for signature
	initialized = 1U;
	for(i = 0U; i != 8U; ++i) {
		if(ram_data[RAM_SIG_ADDR + i] != RAM_SIG[i]) {
			initialized = 0U;
			break;
		}
	}

	// Initialize memory
	if(initialized == 0U || force_clear) {
		for(i = 0U; i != 64U; ++i) {
			ram_data[i] = 0U;
		}

		for(i = 0U; i != 8U; ++i) {
			ram_data[RAM_SIG_ADDR + i] = RAM_SIG[i];
		}

		ram_data[RAM_DASHCOUNTER] = 1U;
	}

	// Load values from ram
	for(levels_completed = 0U; levels_completed != 4U; ++levels_completed) {
		if(ram_data[levels_completed << 4] == 0U) break;
	}

	show_dashcounter = ram_data[RAM_DASHCOUNTER];

	DISABLE_RAM_MBC1;
}

void vbl_update() {
	++vbl_count;
}

void main() {
	disable_interrupts();
    DISPLAY_OFF;

	initRAM(0U);
	snd_init();

	vbl_count = 0U;
	joystate = oldjoystate = 0U;
	unlocked_bits = 0U;
	player_skin = 1U;
	ending_flags = 0U;

	last_highscore_level = 0U;
	last_highscore_slot = 5U;
	last_progress = 0U;
	selection = level = 1U;
	
	gamestate = GAMESTATE_WINSCREEN;

	SWITCH_16_8_MODE_MBC1;
	add_TIM(updateMusic);
	add_VBL(vbl_update);
	set_interrupts(TIM_IFLAG | VBL_IFLAG);

    if(_cpu == CGB_TYPE) {
        VBK_REG = 1U;
        set_sprite_palette(0U, 1U, GS_SPR_PALETTE);
        VBK_REG = 0U;
    }

    DISPLAY_ON;
	enable_interrupts();

	while(1U) {
		switch(gamestate) {
			case GAMESTATE_LOGOS:
				setGameBank(9U);
				enterLogos();
				break;
			case GAMESTATE_INGAME:
				setGameBank(1U);
				enterGame();
				break;
			case GAMESTATE_TITLE:
				setGameBank(8U);
				enterTitle();
				break;
			case GAMESTATE_SELECT:
				setGameBank(2U);
				enterSelect();
				break;
			case GAMESTATE_HIGHSCORE:
				setGameBank(2U);
				enterHighscore();
				break;
			case GAMESTATE_UNLOCKED:
				setGameBank(2U);
				enterUnlocked();
				break;
			case GAMESTATE_INTRO:
				setGameBank(3U);
				enterIntro();
				break;
			case GAMESTATE_ENDING:
				setGameBank(3U);
				enterEnding();
				break;
			case GAMESTATE_JUKEBOX:
				setGameBank(12U);
				enterJukebox();
				break;
			case GAMESTATE_WINSCREEN:
				setGameBank(7U);
				enterWinscreen();
				break;
			case GAMESTATE_WIPE:
				setGameBank(1U);
				enterWipe();
				break;
			case GAMESTATE_MINIGAME_SCORE:
				setGameBank(1U);
				enterMinigamescore();
				break;
		}
	}
}

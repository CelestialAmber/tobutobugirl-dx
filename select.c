#include <gb/gb.h>
#include "defines.h"
#include "select.h"
#include "fade.h"
#include "gamestate.h"
#include "cos.h"
#include "ram.h"
#include "sound.h"
#include "mmlgb/driver/music.h"

#include "characters.h"
#include "arrow.h"
#include "data/sprite/togglecat.h"

#include "circles.h"
#include "data/bg/catface.h"
#include "data/bg/select.h"

#include "selection1.h"
#include "selection2.h"
#include "selection3.h"
#include "selection4.h"
#include "selection_highscore.h"
#include "selection_jukebox.h"
#include "selection_locked.h"

UBYTE select_scroll_dir;
UBYTE select_cat_state;
UBYTE cat_frame_reverse;

#define CAT_ANIM_OFF 0U
#define CAT_ANIM_IN  1U
#define CAT_ANIM_ON  2U
#define CAT_ANIM_OUT 3U

extern UBYTE mainmenu_song_data;
extern UBYTE potaka_song_data;

void initSelect() {
    UBYTE i;
	disable_interrupts();
	DISPLAY_OFF;

	move_bkg(0U, 0U);
	set_sprite_data(0U, 37U, characters_data);
	set_sprite_data(37U, arrow_data_length, arrow_data);
	set_sprite_data(41U, togglecat_data_length, togglecat_data);

	set_bkg_data(0U, circles_data_length, circles_data);
	set_bkg_data(catface_tiles_offset, catface_data_length, catface_data);
	set_bkg_data_rle(select_tiles_offset, select_data_length, select_data);
	set_bkg_tiles_rle(0U, 0U, select_tiles_width, select_tiles_height, select_tiles);

    if(CGB_MODE) {
        for(i = 0U; i != 8U; ++i) {
            set_bkg_palette(i, 1U, gs_palette);
        }
    }

	ticks = 0U;
	timer = 0U;
	elapsed_time = 0U;
	circle_index = 0U;
	arrow_offset1 = 0U;
	arrow_offset2 = 0U;

	select_cat_state = CAT_ANIM_OFF;
	cat_frame = 0U;
	cat_frame_reverse = 0U;
	if(player_skin == 2U) {
		select_cat_state = CAT_ANIM_IN;
		cat_frame = 4U;
	}

	OBP0_REG = 0xD0U; // 11010000
	OBP1_REG = 0xB4U; // 11100100
	BGP_REG = 0xE4U; // 11100100

	clearSprites();
	_selectUpdateScreen();

	setMusicBank(4U);
	playMusic(&mainmenu_song_data);

	HIDE_WIN;
	SHOW_SPRITES;
	SHOW_BKG;
	SPRITES_8x8;

	DISPLAY_ON;
	enable_interrupts();
}

UBYTE *selectGetBannerData() {
	if(selection <= 4U && selection > levels_completed+1U) {
		set_bkg_data(selection_locked_tiles_offset, selection_locked_data_length, selection_locked_data);
		return selection_locked_tiles;
	} else if(selection == 1U) {
		set_bkg_data(selection1_tiles_offset, selection1_data_length, selection1_data);
		return selection1_tiles;
	} else if(selection == 2U) {
		set_bkg_data(selection2_tiles_offset, selection2_data_length, selection2_data);
		return selection2_tiles;
	} else if(selection == 3U) {
		set_bkg_data(selection3_tiles_offset, selection3_data_length, selection3_data);
		return selection3_tiles;
	} else if(selection == 4U) {
		set_bkg_data(selection4_tiles_offset, selection4_data_length, selection4_data);
		return selection4_tiles;
	} else if(selection == 5U) {
		set_bkg_data(selection_jukebox_tiles_offset, selection_jukebox_data_length, selection_jukebox_data);
		return selection_jukebox_tiles;
	} else if(selection == 6U) {
		set_bkg_data(selection_highscore_tiles_offset, selection_highscore_data_length, selection_highscore_data);
		return selection_highscore_tiles;
	}
	return 0U;
}

void _selectUpdateScreen() {
	UBYTE *data = selectGetBannerData();
	set_bkg_tiles(0U, 10U, 20U, 6U, data);
}

void selectUpdateSprites() {
	UBYTE frame;

	setSprite(24U-arrow_offset1, 69U, 37U, OBJ_PAL0);
	setSprite(32U-arrow_offset1, 69U, 39U, OBJ_PAL0);
	setSprite(24U-arrow_offset1, 77U, 38U, OBJ_PAL0);
	setSprite(32U-arrow_offset1, 77U, 40U, OBJ_PAL0);

	setSprite(136U+arrow_offset2, 69U, 39U, OBJ_PAL0 | FLIP_X);
	setSprite(144U+arrow_offset2, 69U, 37U, OBJ_PAL0 | FLIP_X);
	setSprite(136U+arrow_offset2, 77U, 40U, OBJ_PAL0 | FLIP_X);
	setSprite(144U+arrow_offset2, 77U, 38U, OBJ_PAL0 | FLIP_X);

	if(levels_completed >= 3U) {
		switch(select_cat_state) {
			case CAT_ANIM_OFF:
				if((ticks & 15U) == 15U) {
					if(cat_frame_reverse) {
						cat_frame--;
					} else {
						cat_frame++;
					}
					if(cat_frame == 0U) cat_frame_reverse = 0U;
					else if(cat_frame == 4U) cat_frame_reverse = 1U;
				}
				if(CLICKED(J_SELECT)) {
					select_cat_state = CAT_ANIM_IN;
					player_skin = 2U;
					playSound(SFX_CAT_ENABLE);
				}
				break;
			case CAT_ANIM_IN:
				if((ticks & 7U) == 7U) cat_frame++;
				if(cat_frame == 8U) select_cat_state = CAT_ANIM_ON;
				break;
			case CAT_ANIM_ON:
				if((ticks & 15U) == 15U) cat_frame++;
				if(cat_frame == 10U) cat_frame = 8U;
				if(CLICKED(J_SELECT)) {
					select_cat_state = CAT_ANIM_OUT;
					player_skin = 1U;
					playSound(SFX_CAT_DISABLE);
				}
				break;
			case CAT_ANIM_OUT:
				if((ticks & 7U) == 7U) cat_frame--;
				if(cat_frame == 4U) {
					select_cat_state = CAT_ANIM_OFF;
					cat_frame_reverse = 1U;
				}
				break;
		}

		frame = 41U + (cat_frame << 2);
		setSprite(136U, 20U, frame++, OBJ_PAL0);
		setSprite(144U, 20U, frame++, OBJ_PAL0);
		setSprite(136U, 28U, frame++, OBJ_PAL0);
		setSprite(144U, 28U, frame, OBJ_PAL0);
	}
}

void selectScrollCircles() {
	circle_index = (circle_index+1U) & 7U;
	set_bkg_data(13U, 1U, &circles_data[(circle_index << 4)]);
}

void selectFadeOut() {
	UBYTE i, x;
	UBYTE even_tiles[6U];
	UBYTE odd_tiles[6U];

	even_tiles[0] = 10U;
	even_tiles[1] = 12U;

	odd_tiles[0] = 9U;
	odd_tiles[1] = 11U;

	for(i = 0U; i != 20U; ++i) {
		disable_interrupts();
		if(select_scroll_dir == LEFT) x = i;
		else x = 19U - i;
		if(x & 1U) {
			set_bkg_tiles(x, 10U, 1U, 2U, odd_tiles);
			set_bkg_tiles(x, 12U, 1U, 2U, odd_tiles);
			set_bkg_tiles(x, 14U, 1U, 2U, odd_tiles);
		} else {
			set_bkg_tiles(x, 10U, 1U, 2U, even_tiles);
			set_bkg_tiles(x, 12U, 1U, 2U, even_tiles);
			set_bkg_tiles(x, 14U, 1U, 2U, even_tiles);
		}
		enable_interrupts();
		if(!(i & 1U)) {
			ticks++;
			if((ticks & 3U) == 3U) selectScrollCircles();
			if(select_scroll_dir == LEFT) arrow_offset1 = cos32_64[i >> 1];
			else arrow_offset2 = cos32_64[i >> 1];
			selectUpdateSprites();
			clearRemainingSprites();
			snd_update();
			wait_vbl_done();
		}
	}
	arrow_offset1 = 0U;
	arrow_offset2 = 0U;
}

void selectFadeIn() {
	UBYTE i, j, x;
	UBYTE *data;
	UBYTE *ptr;
	UBYTE tiles[6];

	disable_interrupts();
	data = selectGetBannerData();
	enable_interrupts();

	for(i = 0U; i != 20U; ++i) {
		if(select_scroll_dir == LEFT) x = i;
		else x = 19U - i;
		ptr = data + x;
		for(j = 0U; j != 6U; ++j) {
			tiles[j] = *ptr;
			ptr += 20U;
		}
		++ptr;

		disable_interrupts();
		set_bkg_tiles(x, 10U, 1U, 6U, tiles);
		enable_interrupts();
		if(i & 1U) {
			ticks++;
			if((ticks & 3U) == 3U) selectScrollCircles();
			if(select_scroll_dir == LEFT) arrow_offset1 = cos32_64[10U - (i >> 1)];
			else arrow_offset2 = cos32_64[10U - (i >> 1)];
			selectUpdateSprites();
			clearRemainingSprites();
			snd_update();
			wait_vbl_done();
		}
	}

	arrow_offset1 = 0U;
	arrow_offset2 = 0U;
}

void enterSelect() {
	UBYTE i, offset, name_index;
	initSelect();

	fadeFromWhite(6U);

	while(gamestate == GAMESTATE_SELECT) {
		updateJoystate();

		ticks++;
		if((ticks & 3U) == 3U) {
			selectScrollCircles();
		}

		timer++;
		if(timer == 60U) {
			timer = 0U;
			elapsed_time++;

			if(elapsed_time == 110U) {
				//mus_setPaused(1U);
				disable_interrupts();
				stopMusic();
				enable_interrupts();
			}

			else if(elapsed_time == 120U) {
				disable_interrupts();
				setMusicBank(4U);
				playMusic(&mainmenu_song_data);
				enable_interrupts();

				elapsed_time = 0U;
			}
		}

		if(elapsed_time == 110U && timer == 32) {
			disable_interrupts();
			setMusicBank(11U);
			playMusic(&potaka_song_data);
			enable_interrupts();
		}

		if(ISDOWN(J_RIGHT)) {
			selection++;
			select_scroll_dir = RIGHT;
			if(selection == 5U && levels_completed < 2U) selection++;
			if(selection > 6U) selection = 1U;
			playSound(SFX_MENU_SWITCH);
			selectFadeOut();
			selectFadeIn();
			selectUpdateSprites();
		}
		if(ISDOWN(J_LEFT)) {
			selection--;
			select_scroll_dir = LEFT;
			if(selection == 5U && levels_completed < 2U) selection--;
			if(selection == 0U) selection = 6U;
			playSound(SFX_MENU_SWITCH);
			selectFadeOut();
			selectFadeIn();
			selectUpdateSprites();
		}
		if(CLICKED(J_START) || CLICKED(J_A)) {
			if(selection == 5U) {
				gamestate = GAMESTATE_JUKEBOX;
				playSound(SFX_MENU_CONFIRM);
			} else if(selection == 6U) {
				gamestate = GAMESTATE_HIGHSCORE;
				playSound(SFX_MENU_CONFIRM);
			} else {
				if(selection <= levels_completed+1U) {
					level = selection;
					gamestate = GAMESTATE_INGAME;
					playSound(SFX_MENU_CONFIRM);
				} else {
					playSound(SFX_MENU_LOCKED);
				}
			}
		}
		if(CLICKED(J_B)) {
			gamestate = GAMESTATE_TITLE;
			playSound(SFX_MENU_CANCEL);
		}

		// Draw level name
		if(selection <= 4U && selection > levels_completed+1U) {
			name_index = 0U;
		} else {
			name_index = selection;
		}

		offset = 64U;
		if(level_names[name_index][5] == 10U) {
			offset += 4U;
		}
		for(i = 0U; i != 6; ++i) {
			setSprite(offset+(i << 3), 70U+cos4_16[(i+(ticks >> 1)) & 15U], level_names[name_index][i], OBJ_PAL0);
		}

		selectUpdateSprites();
		clearRemainingSprites();
		snd_update();
		wait_vbl_done();
	}

	stopMusic();
	clearRemainingSprites(); // Remove all sprites
	fadeToWhite(8U);
	wait_sound_done();
}

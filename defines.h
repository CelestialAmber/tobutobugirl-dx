#ifndef __DEFINES_H
#define __DEFINES_H

#define NONE	0U

#define UP		0U
#define RIGHT	1U
#define DOWN	2U
#define LEFT	3U

// Entities
#define E_NONE 		0U

#define E_SPIKES	1U

#define E_BIRD		2U
#define E_BAT		3U
#define E_GHOST		4U
#define E_ALIEN		5U

#define E_JUMPPAD	6U

#define E_BLIP		7U
#define E_ROCKET	8U
#define E_DRILL		9U
#define E_BALLOON	10U
#define E_UMBRELLA	11U

#define E_GRAPES	12U
#define E_PEACH		13U

#define E_CLOUD		14U

#define FIRST_ENEMY E_SEAL
#define LAST_ENEMY E_ALIEN

#define FIRST_FRUIT E_GRAPES
#define LAST_FRUIT E_PEACH

#define LAST_COLLIDABLE E_PEACH

#define MAX_ENTITIES 18U

// Entity properties
#define FLIP_X 32U
#define FLIP_Y 64U

#define OBJ_PAL0 0U
#define OBJ_PAL1 16U

#endif

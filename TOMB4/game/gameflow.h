#pragma once
#include "../global/vars.h"

void inject_gameflow(bool replace);

void DoGameflow();
void DoLevel(uchar Name, uchar Audio);
void DoTitle(uchar Name, uchar Audio);

#define DoCredits	( (long(__cdecl*)()) 0x00452B80 )

enum gf_commands
{
	CMD_FMV = 0x80,
	CMD_LEVEL,
	CMD_TITLE,
	CMD_ENDSEQ,
	CMD_PLAYCUT,
	CMD_CUT1,
	CMD_CUT2,
	CMD_CUT3,
	CMD_CUT4,
	CMD_LAYER1,
	CMD_LAYER2,
	CMD_UVROT,
	CMD_LEGEND,
	CMD_LENSFLARE,
	CMD_MIRROR,
	CMD_FOG,
	CMD_ANIMATINGMIP,
	CMD_CAMERA,
	CMD_RESETHUB,
	CMD_KEY1,
	CMD_KEY2,
	CMD_KEY3,
	CMD_KEY4,
	CMD_KEY5,
	CMD_KEY6,
	CMD_KEY7,
	CMD_KEY8,
	CMD_KEY9,
	CMD_KEY10,
	CMD_KEY11,
	CMD_KEY12,
	CMD_PUZZLE1,
	CMD_PUZZLE2,
	CMD_PUZZLE3,
	CMD_PUZZLE4,
	CMD_PUZZLE5,
	CMD_PUZZLE6,
	CMD_PUZZLE7,
	CMD_PUZZLE8,
	CMD_PUZZLE9,
	CMD_PUZZLE10,
	CMD_PUZZLE11,
	CMD_PUZZLE12,
	CMD_PICKUP1,
	CMD_PICKUP2,
	CMD_PICKUP3,
	CMD_PICKUP4,
	CMD_EXAMINE1,
	CMD_EXAMINE2,
	CMD_EXAMINE3,
	CMD_KEYCOMBO1_1,
	CMD_KEYCOMBO1_2,
	CMD_KEYCOMBO2_1,
	CMD_KEYCOMBO2_2,
	CMD_KEYCOMBO3_1,
	CMD_KEYCOMBO3_2,
	CMD_KEYCOMBO4_1,
	CMD_KEYCOMBO4_2,
	CMD_KEYCOMBO5_1,
	CMD_KEYCOMBO5_2,
	CMD_KEYCOMBO6_1,
	CMD_KEYCOMBO6_2,
	CMD_KEYCOMBO7_1,
	CMD_KEYCOMBO7_2,
	CMD_KEYCOMBO8_1,
	CMD_KEYCOMBO8_2,
	CMD_PUZZLECOMBO1_1,
	CMD_PUZZLECOMBO1_2,
	CMD_PUZZLECOMBO2_1,
	CMD_PUZZLECOMBO2_2,
	CMD_PUZZLECOMBO3_1,
	CMD_PUZZLECOMBO3_2,
	CMD_PUZZLECOMBO4_1,
	CMD_PUZZLECOMBO4_2,
	CMD_PUZZLECOMBO5_1,
	CMD_PUZZLECOMBO5_2,
	CMD_PUZZLECOMBO6_1,
	CMD_PUZZLECOMBO6_2,
	CMD_PUZZLECOMBO7_1,
	CMD_PUZZLECOMBO7_2,
	CMD_PUZZLECOMBO8_1,
	CMD_PUZZLECOMBO8_2,
	CMD_PICKUPCOMBO1_1,
	CMD_PICKUPCOMBO1_2,
	CMD_PICKUPCOMBO2_1,
	CMD_PICKUPCOMBO2_2,
	CMD_PICKUPCOMBO3_1,
	CMD_PICKUPCOMBO3_2,
	CMD_PICKUPCOMBO4_1,
	CMD_PICKUPCOMBO4_2,
};

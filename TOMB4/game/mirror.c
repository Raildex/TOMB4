
#include "game/mirror.h"
#include "game/delstuff.h"
#include "game/gameflow.h"
#include "game/gunshellstruct.h"
#include "game/hair.h"
#include "game/hairstruct.h"
#include "game/iteminfo.h"
#include "game/lara.h"
#include "game/larainfo.h"
#include "game/laramesh.h"
#include "game/matrixindices.h"
#include "game/tomb4fx.h"
#include "specific/3dmath.h"
#include "specific/function_table.h"


void Draw_Mirror_Lara() {
	HAIR_STRUCT* hair;
	GUNSHELL_STRUCT* gunshell;
	long zplane;

	zplane = gfMirrorZPlane << 1;
	hair = &hairs[0][0];

	for(int i = 0; i < 6; i++) {
		hair->pos.pos.z = zplane - hair->pos.pos.z;
		hair->pos.y_rot = 0x8000 - hair->pos.y_rot;
		hair++;
	}

	for(int i = 0; i < 24; i++) {
		gunshell = &Gunshells[i];

		if(gunshell->counter) {
			gunshell->pos.pos.z = zplane - gunshell->pos.pos.z;
		}
	}

	lara_item->pos.pos.z = zplane - lara_item->pos.pos.z;
	lara_item->pos.x_rot = -lara_item->pos.x_rot;
	lara_item->pos.y_rot = -lara_item->pos.y_rot;
	lara_item->pos.z_rot += 0x8000;
	CalcLaraMatrices(2);
	SetCullCW();
	DrawLara(lara_item, 1);
	DrawGunshells();
	SetCullCCW();
	lara_item->pos.pos.z = zplane - lara_item->pos.pos.z;
	lara_item->pos.x_rot = -lara_item->pos.x_rot;
	lara_item->pos.y_rot = -lara_item->pos.y_rot;
	lara_item->pos.z_rot += 0x8000;
	phd_PushMatrix();

	if(lara.right_arm.flash_gun) {
		memcpy(mMXPtr,lara_matrices + LM_RHAND * indices_count,sizeof(float)* indices_count);
		SetGunFlash(lara.gun_type);
	}

	if(lara.left_arm.flash_gun) {
		memcpy(mMXPtr,lara_matrices + LM_LHAND * indices_count,sizeof(float)* indices_count);

		SetGunFlash(lara.gun_type);
	}

	phd_PopMatrix();

	for(int i = 0; i < 24; i++) {
		gunshell = &Gunshells[i];

		if(gunshell->counter) {
			gunshell->pos.pos.z = zplane - gunshell->pos.pos.z;
		}
	}

	hair = &hairs[0][0];

	for(int i = 0; i < 6; i++) {
		hair->pos.pos.z = zplane - hair->pos.pos.z;
		hair->pos.y_rot = 0x8000 - hair->pos.y_rot;
		hair++;
	}
}

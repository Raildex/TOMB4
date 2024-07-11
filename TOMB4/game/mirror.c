
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
		mMXPtr[M00] = lara_matrices[LM_RHAND * indices_count + M00];
		mMXPtr[M01] = lara_matrices[LM_RHAND * indices_count + M01];
		mMXPtr[M02] = lara_matrices[LM_RHAND * indices_count + M02];
		mMXPtr[M03] = lara_matrices[LM_RHAND * indices_count + M03];
		mMXPtr[M10] = lara_matrices[LM_RHAND * indices_count + M10];
		mMXPtr[M11] = lara_matrices[LM_RHAND * indices_count + M11];
		mMXPtr[M12] = lara_matrices[LM_RHAND * indices_count + M12];
		mMXPtr[M13] = lara_matrices[LM_RHAND * indices_count + M13];
		mMXPtr[M20] = lara_matrices[LM_RHAND * indices_count + M20];
		mMXPtr[M21] = lara_matrices[LM_RHAND * indices_count + M21];
		mMXPtr[M22] = lara_matrices[LM_RHAND * indices_count + M22];
		mMXPtr[M23] = lara_matrices[LM_RHAND * indices_count + M23];
		SetGunFlash(lara.gun_type);
	}

	if(lara.left_arm.flash_gun) {
		mMXPtr[M00] = lara_matrices[LM_LHAND * indices_count + M00];
		mMXPtr[M01] = lara_matrices[LM_LHAND * indices_count + M01];
		mMXPtr[M02] = lara_matrices[LM_LHAND * indices_count + M02];
		mMXPtr[M03] = lara_matrices[LM_LHAND * indices_count + M03];
		mMXPtr[M10] = lara_matrices[LM_LHAND * indices_count + M10];
		mMXPtr[M11] = lara_matrices[LM_LHAND * indices_count + M11];
		mMXPtr[M12] = lara_matrices[LM_LHAND * indices_count + M12];
		mMXPtr[M13] = lara_matrices[LM_LHAND * indices_count + M13];
		mMXPtr[M20] = lara_matrices[LM_LHAND * indices_count + M20];
		mMXPtr[M21] = lara_matrices[LM_LHAND * indices_count + M21];
		mMXPtr[M22] = lara_matrices[LM_LHAND * indices_count + M22];
		mMXPtr[M23] = lara_matrices[LM_LHAND * indices_count + M23];
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

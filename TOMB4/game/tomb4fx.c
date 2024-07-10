
#include "game/tomb4fx.h"
#include "specific/function_stubs.h"
#include "game/draw.h"
#include "specific/3dmath.h"
#include "game/items.h"
#include "game/objects.h"
#include "specific/output.h"
#include "game/sound.h"
#include "game/delstuff.h"
#include "game/control.h"
#include "specific/specificfx.h"
#include "game/effect2.h"
#include "game/sphere.h"
#include "game/effects.h"
#include "game/lara.h"
#include "game/gameflow.h"
#include "game/fxinfo.h"
#include "game/iteminfo.h"
#include "global/types.h"
#include "game/objectinfo.h"
#include "game/gunshellstruct.h"
#include "game/smokesparks.h"
#include "game/dripstruct.h"
#include "game/roomflags.h"
#include "game/firesparks.h"
#include "game/roominfo.h"
#include "game/gfleveloptions.h"
#include "game/weapontypes.h"
#include "game/firelist.h"
#include "game/gamevector.h"
#include "game/larainfo.h"
#include "game/sparks.h"
#include "game/gunflashstruct.h"
#include "game/bloodstruct.h"
#include "game/bubblestruct.h"
#include "game/shockwavestruct.h"
#include "game/lightningstruct.h"
#include "game/nodeoffsetinfo.h"
#include "game/fvector.h"
#include "game/levelinfo.h"
#include "tomb4fx.h"
#include <string.h>
#include <stdlib.h>


NODEOFFSET_INFO NodeOffsets[16] = {
	{ -16, 40, 160, -14, 0 },
	{ -16, -8, 160, 0, 0 },
	{ 16, 200, 32, 17, 0 },
	{ -16, 200, 32, 13, 0 },
	{ 0, 128, 0, 4, 0 },
	{ 0, 128, 0, 2, 0 },
	{ -200, -30, 8, 7, 0 },

	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

long nLightnings;
LIGHTNING_STRUCT* Lightning;
long nGunshells;
GUNSHELL_STRUCT* Gunshells;
long nDrips;
DRIP_STRUCT* Drips;
long nSmokeSparks;
SMOKE_SPARKS* smoke_spark;
long nBubbles;
BUBBLE_STRUCT* Bubbles;
long nShockWaves;
SHOCKWAVE_STRUCT* ShockWaves;
long nFireSparks;
FIRE_SPARKS* fire_spark;
long nBlood;
BLOOD_STRUCT* blood;
long nGunflashes;
GUNFLASH_STRUCT* Gunflashes;
long nFires;
FIRE_LIST* fires;
short FlashFadeR = 0;
short FlashFadeG = 0;
short FlashFadeB = 0;
short FlashFader = 0;
short ScreenFade = 0;
short dScreenFade = 0;
short ScreenFadeBack = 0;
short ScreenFadedOut = 0;
short ScreenFading = 0;
short FadeScreenHeight = 0;
short DestFadeScreenHeight = 0;
short FadeClipSpeed = 0;
short ScreenFadeSpeed = 8;
char tsv_buffer[16384];

static PHD_VECTOR NodeVectors[16];

SMOKE_SPARKS* GetFreeSmokeSpark() {
	for(int i =0; i < nSmokeSparks; ++i) {
		if(!smoke_spark[i].On) {
			return &smoke_spark[i];
		}
	}
	size_t idx = nSmokeSparks;
	nSmokeSparks = (nSmokeSparks * 2) + 4;
	smoke_spark = realloc(smoke_spark, nSmokeSparks * sizeof(SMOKE_SPARKS));
	for(int i = idx; i < nSmokeSparks; ++i) {
		SMOKE_SPARKS* sptr = &smoke_spark[i];
		memset(sptr, 0, sizeof(SMOKE_SPARKS));
	}
	return &smoke_spark[idx];
}

LIGHTNING_STRUCT* GetFreeLightning() {
	for(int i =0; i < nLightnings; ++i) {
		if(!Lightning[i].Life) {
			return &Lightning[i];
		}
	}
	size_t idx = nLightnings;
	nLightnings = (nLightnings * 2) + 4;
	Lightning = realloc(Lightning, nLightnings * sizeof(LIGHTNING_STRUCT));
	for(int i = idx; i < nLightnings; ++i) {
		LIGHTNING_STRUCT* bptr = &Lightning[i];
		memset(bptr, 0, sizeof(LIGHTNING_STRUCT));
	}
	return &Lightning[idx];
}

LIGHTNING_STRUCT* TriggerLightning(PHD_VECTOR* s, PHD_VECTOR* d, char variation, long rgb, unsigned char flags, unsigned char size, unsigned char segments) {
	LIGHTNING_STRUCT* lptr;
	char* vptr;

	lptr = GetFreeLightning();
	lptr->Life = 254;
	lptr->Point[0].x = s->x;
	lptr->Point[0].y = s->y;
	lptr->Point[0].z = s->z;
	lptr->Point[1].x = ((s->x * 3) + d->x) >> 2;
	lptr->Point[1].y = ((s->y * 3) + d->y) >> 2;
	lptr->Point[1].z = ((s->z * 3) + d->z) >> 2;
	lptr->Point[2].x = ((d->x * 3) + s->x) >> 2;
	lptr->Point[2].y = ((d->y * 3) + s->y) >> 2;
	lptr->Point[2].z = ((d->z * 3) + s->z) >> 2;
	lptr->Point[3].x = d->x;
	lptr->Point[3].y = d->y;
	lptr->Point[3].z = d->z;
	vptr = &lptr->Xvel1;

	for(int j = 0; j < 6; j++)
		*vptr++ = (GetRandomControl() % variation) - (variation >> 1);

	for(int j = 0; j < 3; j++) {
		if(flags & 2)
			*vptr++ = (GetRandomControl() % variation) - (variation >> 1);
		else
			*vptr++ = 0;
	}

	lptr->Flags = flags;
	*(long*)&lptr->r = rgb;
	lptr->Segments = segments;
	lptr->Rand = variation;
	lptr->Size = size;
	return lptr;

	return 0;
}

long ExplodingDeath2(short item_number, long mesh_bits, short Flags) {
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	FX_INFO* fx;
	long* bone;
	short* rotation;
	short* frame;
	short* extra_rotation;
	long bit, poppush;
	short fx_number;

	item = GetItem(currentLevel,item_number);
	obj = GetObjectInfo(currentLevel,item->object_number);
	frame = GetBestFrame(item);
	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	rotation = frame + 9;
	gar_RotYXZsuperpack(&rotation, 0);

	if(!item->data)
		extra_rotation = no_rotation;
	else
		extra_rotation = (short*)item->data;

	bone = GetBone(currentLevel,obj->bone_index);
	bit = 1;

	if(mesh_bits & 1 && item->mesh_bits & 1) {
		if(Flags & 0x100 || !(GetRandomControl() & 3)) {
			fx_number = CreateEffect(item->room_number);

			if(fx_number != NO_ITEM) {
				fx = GetEffect(currentLevel,fx_number);
				fx->pos.x_pos = item->pos.x_pos + (long)mMXPtr[M03];
				fx->pos.y_pos = item->pos.y_pos + (long)mMXPtr[M13];
				fx->pos.z_pos = item->pos.z_pos + (long)mMXPtr[M23];
				fx->room_number = item->room_number;
				fx->pos.y_rot = (short)(GetRandomControl() << 1);
				fx->pos.x_rot = 0;

				if(Flags & 0x10)
					fx->speed = 0;
				else if(Flags & 0x20)
					fx->speed = (short)(GetRandomControl() >> 12);
				else
					fx->speed = (short)(GetRandomControl() >> 8);

				if(Flags & 0x40)
					fx->fallspeed = 0;
				else if(Flags & 0x80)
					fx->fallspeed = (short)(-(GetRandomControl() >> 12));
				else
					fx->fallspeed = (short)(-(GetRandomControl() >> 8));

				fx->frame_number = obj->mesh_index;
				fx->object_number = BODY_PART;
				fx->shade = 0x4210;
				fx->flag2 = Flags;
			}

			item->mesh_bits--;
		}
	}

	for(int i = 1; i < obj->nmeshes; i++, bone += 4) {
		poppush = bone[0];

		if(poppush & 1)
			phd_PopMatrix();

		if(poppush & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rotation, 0);

		if(poppush & 28) {
			if(poppush & 8) {
				phd_RotY(*extra_rotation);
				extra_rotation++;
			}

			if(poppush & 4) {
				phd_RotX(*extra_rotation);
				extra_rotation++;
			}

			if(poppush & 16) {
				phd_RotZ(*extra_rotation);
				extra_rotation++;
			}
		}

		bit <<= 1;

		if(bit & mesh_bits && bit & item->mesh_bits && (Flags & 0x100 || !(GetRandomControl() & 3))) {
			fx_number = CreateEffect(item->room_number);

			if(fx_number != NO_ITEM) {
				fx = GetEffect(currentLevel,fx_number);
				fx->pos.x_pos = item->pos.x_pos + (long)mMXPtr[M03];
				fx->pos.y_pos = item->pos.y_pos + (long)mMXPtr[M13];
				fx->pos.z_pos = item->pos.z_pos + (long)mMXPtr[M23];
				fx->room_number = item->room_number;
				fx->pos.y_rot = (short)(GetRandomControl() << 1);
				fx->pos.x_rot = 0;

				if(Flags & 0x10)
					fx->speed = 0;
				else if(Flags & 0x20)
					fx->speed = (short)(GetRandomControl() >> 12);
				else
					fx->speed = (short)(GetRandomControl() >> 8);

				if(Flags & 0x40)
					fx->fallspeed = 0;
				else if(Flags & 0x80)
					fx->fallspeed = (short)(-(GetRandomControl() >> 12));
				else
					fx->fallspeed = (short)(-(GetRandomControl() >> 8));

				fx->frame_number = obj->mesh_index + 2 * i;
				fx->object_number = BODY_PART;
				fx->shade = 0x4210;
				fx->flag2 = Flags;
			}

			item->mesh_bits -= bit;
		}
	}

	phd_PopMatrix();
	return !item->mesh_bits;
}

void DrawGunshells() {
	GUNSHELL_STRUCT* p;
	OBJECT_INFO* obj;

	phd_left = 0;
	phd_right = phd_winwidth;
	phd_top = 0;
	phd_bottom = phd_winheight;

	for(int i = 0; i < nGunshells; i++) {
		p = &Gunshells[i];

		if(p->counter) {
			obj = GetObjectInfo(currentLevel,p->object_number);
			phd_PushMatrix();
			phd_TranslateAbs(p->pos.x_pos, p->pos.y_pos, p->pos.z_pos);
			phd_RotYXZ(p->pos.y_rot, p->pos.x_rot, p->pos.z_rot);
			phd_PutPolygons(GetMesh(currentLevel,obj->mesh_index), -1);
			phd_PopMatrix();
		}
	}
}

void TriggerGunSmoke(long x, long y, long z, long xVel, long yVel, long zVel, long notLara, long weaponType, long shade) {
	SMOKE_SPARKS* sptr;
	unsigned char size;

	sptr = GetFreeSmokeSpark();
	sptr->On = 1;
	sptr->sShade = 0;
	sptr->dShade = (unsigned char)(4 * shade);
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = (unsigned char)(32 - 16 * notLara);
	sptr->Life = (GetRandomControl() & 3) + 40;
	sptr->sLife = sptr->Life;

	if((weaponType == WEAPON_PISTOLS || weaponType == WEAPON_REVOLVER || weaponType == WEAPON_UZI) && sptr->dShade > 64u)
		sptr->dShade = 64;

	sptr->TransType = 2;
	sptr->pos.x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->pos.y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->pos.z = (GetRandomControl() & 0x1F) + z - 16;

	if(notLara) {
		sptr->Xvel = (short)((GetRandomControl() & 0x3FF) + xVel - 512);
		sptr->Yvel = (short)((GetRandomControl() & 0x3FF) + yVel - 512);
		sptr->Zvel = (short)((GetRandomControl() & 0x3FF) + zVel - 512);
	} else {
		sptr->Xvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
		sptr->Yvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
		sptr->Zvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
	}

	sptr->Friction = 4;

	if(GetRandomControl() & 1) {
		if(GetRoom(currentLevel,lara_item->room_number)->flags & ROOM_NOT_INSIDE)
			sptr->Flags = 272;

		else
			sptr->Flags = 16;

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if(GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	} else if(GetRoom(currentLevel,lara_item->room_number)->flags & ROOM_NOT_INSIDE)
		sptr->Flags = 256;
	else
		sptr->Flags = 0;

	sptr->Gravity = -2 - (GetRandomControl() & 1);
	sptr->MaxYvel = -2 - (GetRandomControl() & 1);
	size = (GetRandomControl() & 0xF) - (weaponType != WEAPON_GRENADE ? 24 : 0) + 48;

	if(notLara) {
		sptr->Size = size >> 1;
		sptr->sSize = size >> 1;
		sptr->dSize = (size + 4) << 1;
	} else {
		sptr->sSize = size >> 2;
		sptr->Size = size >> 2;
		sptr->dSize = size;
	}

	sptr->mirror = gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom;
}

void LaraBubbles(ITEM_INFO* item) {
	PHD_VECTOR pos;

	SoundEffect(SFX_LARA_BUBBLES, &item->pos, SFX_WATER);
	pos.x = 0;
	pos.y = -4;
	pos.z = 64;
	GetLaraJointPos(&pos, 8);

	for(int i = (GetRandomControl() & 1) + 2; i > 0; i--) {
		CreateBubble((PHD_3DPOS*)&pos, item->room_number, 8, 7);

		if(gfLevelFlags & GF_MIRROR && item->room_number == gfMirrorRoom) {
			pos.z = 2 * gfMirrorZPlane - pos.z;
			CreateBubble((PHD_3DPOS*)&pos, item->room_number, 8, 7);
			pos.z = 2 * gfMirrorZPlane - pos.z;
		}
	}
}

void UpdateDrips() {
	DRIP_STRUCT* drip;
	FLOOR_INFO* floor;
	long h;

	for(int i = 0; i < nDrips; i++) {
		drip = &Drips[i];

		if(!drip->On)
			continue;

		drip->Life--;

		if(!drip->Life) {
			drip->On = 0;
			continue;
		}

		if(drip->Life < 16) {
			drip->R -= drip->R >> 3;
			drip->G -= drip->G >> 3;
			drip->B -= drip->B >> 3;
		}

		drip->Yvel += drip->Gravity;

		if(GetRoom(currentLevel,drip->RoomNumber)->flags & ROOM_NOT_INSIDE) {
			drip->pos.x += SmokeWindX >> 1;
			drip->pos.z += SmokeWindZ >> 1;
		}

		drip->pos.y += drip->Yvel >> 5;
		floor = GetFloor(drip->pos.x, drip->pos.y, drip->pos.z, &drip->RoomNumber);
		h = GetHeight(floor, drip->pos.x, drip->pos.y, drip->pos.z);

		if(GetRoom(currentLevel,drip->RoomNumber)->flags & ROOM_UNDERWATER || drip->pos.y > h)
			drip->On = 0;
	}
}

FIRE_SPARKS* GetFreeFireSpark() {
	for(int i =0; i < nFireSparks; ++i) {
		if(!fire_spark[i].On) {
			return &fire_spark[i];
		}
	}
	size_t idx = nFireSparks;
	nFireSparks = (nFireSparks * 2) + 4;
	fire_spark = realloc(fire_spark, nFireSparks * sizeof(FIRE_SPARKS));
	for(int i = idx; i < nFireSparks; ++i) {
		FIRE_SPARKS* bptr = &fire_spark[i];
		memset(bptr, 0, sizeof(FIRE_SPARKS));
	}
	return &fire_spark[idx];
}

void TriggerGlobalStaticFlame() {
	FIRE_SPARKS* sptr;

	sptr = GetFreeFireSpark();
	sptr->On = 1;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = (GetRandomControl() & 0x3F) + 96;
	sptr->dB = 64;
	sptr->sR = sptr->dR;
	sptr->sG = sptr->dG;
	sptr->sB = sptr->dB;
	sptr->ColFadeSpeed = 1;
	sptr->FadeToBlack = 0;
	sptr->Life = 8;
	sptr->sLife = 8;
	sptr->x = (GetRandomControl() & 7) - 4;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 7) - 4;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Friction = 0;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Flags = 0;
	sptr->dSize = (GetRandomControl() & 0x1F) + 0x80;
	sptr->sSize = sptr->dSize;
	sptr->Size = sptr->dSize;
}

void TriggerGlobalFireFlame() {
	FIRE_SPARKS* sptr;

	sptr = GetFreeFireSpark();
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
	sptr->Life = (GetRandomControl() & 7) + 32;
	sptr->sLife = sptr->Life;
	sptr->x = 4 * (GetRandomControl() & 0x1F) - 64;
	sptr->y = 0;
	sptr->z = 4 * (GetRandomControl() & 0x1F) - 64;
	sptr->Xvel = 2 * (GetRandomControl() & 0xFF) - 256;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = 2 * (GetRandomControl() & 0xFF) - 256;
	sptr->Friction = 5;
	sptr->Gravity = -32 - (GetRandomControl() & 0x1F);
	sptr->MaxYvel = -16 - (GetRandomControl() & 7);

	if(GetRandomControl() & 1) {
		sptr->Flags = 16;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if(GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	} else
		sptr->Flags = 0;

	sptr->Size = (GetRandomControl() & 0x1F) + 128;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 4;
}

void keep_those_fires_burning() {
	FIRE_SPARKS* sptr;

	TriggerGlobalStaticFlame();

	if(!(wibble & 0xF)) {
		TriggerGlobalFireFlame();

		if(!(wibble & 0x1F)) {
			sptr = GetFreeFireSpark();
			sptr->On = 1;
			sptr->sR = 0;
			sptr->sG = 0;
			sptr->sB = 0;
			sptr->dR = 32;
			sptr->dG = 32;
			sptr->dB = 32;
			sptr->FadeToBlack = 16;
			sptr->ColFadeSpeed = (GetRandomControl() & 7) + 32;
			sptr->Life = (GetRandomControl() & 0xF) + 57;
			sptr->sLife = sptr->Life;
			sptr->x = (GetRandomControl() & 0xF) - 8;
			sptr->y = -256 - (GetRandomControl() & 0x7F);
			sptr->z = (GetRandomControl() & 0xF) - 8;
			sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
			sptr->Yvel = -16 - (GetRandomControl() & 0xF);
			sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
			sptr->Friction = 4;

			if(GetRandomControl() & 1) {
				sptr->Flags = 16;
				sptr->RotAng = GetRandomControl() & 0xFFF;

				if(GetRandomControl() & 1)
					sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
				else
					sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
			} else
				sptr->Flags = 0;

			sptr->Gravity = -16 - (GetRandomControl() & 0xF);
			sptr->MaxYvel = -8 - (GetRandomControl() & 7);
			sptr->dSize = (GetRandomControl() & 0x7F) + 128;
			sptr->sSize = sptr->dSize >> 2;
			sptr->Size = sptr->dSize >> 2;
		}
	}
}

void UpdateFireSparks() {
	FIRE_SPARKS* sptr;
	long fade;

	keep_those_fires_burning();

	for(int i = 0; i < nFireSparks; i++) {
		sptr = &fire_spark[i];

		if(!sptr->On)
			continue;

		sptr->Life--;

		if(!sptr->Life) {
			sptr->On = 0;
			continue;
		}

		if(sptr->sLife - sptr->Life < sptr->ColFadeSpeed) {
			fade = ((sptr->sLife - sptr->Life) << 16) / sptr->ColFadeSpeed;
			sptr->R = (unsigned char)(sptr->sR + ((fade * (sptr->dR - sptr->sR)) >> 16));
			sptr->G = (unsigned char)(sptr->sG + ((fade * (sptr->dG - sptr->sG)) >> 16));
			sptr->B = (unsigned char)(sptr->sB + ((fade * (sptr->dB - sptr->sB)) >> 16));
		} else if(sptr->Life < sptr->FadeToBlack) {
			fade = ((sptr->Life - sptr->FadeToBlack) << 16) / sptr->FadeToBlack + 0x10000;
			sptr->R = (unsigned char)((fade * sptr->dR) >> 16);
			sptr->G = (unsigned char)((fade * sptr->dG) >> 16);
			sptr->B = (unsigned char)((fade * sptr->dB) >> 16);

			if(sptr->R < 8 && sptr->G < 8 && sptr->B < 8) {
				sptr->On = 0;
				continue;
			}
		} else {
			sptr->R = sptr->dR;
			sptr->G = sptr->dG;
			sptr->B = sptr->dB;
		}

		if(sptr->Flags & 0x10)
			sptr->RotAng = (sptr->RotAng + sptr->RotAdd) & 0xFFF;

		if(sptr->R < 24 && sptr->G < 24 && sptr->B < 24)
			sptr->Def = (unsigned char)(GetObjectInfo(currentLevel,DEFAULT_SPRITES)->mesh_index + 2);
		else if(sptr->R < 80 && sptr->G < 80 && sptr->B < 80)
			sptr->Def = (unsigned char)(GetObjectInfo(currentLevel,DEFAULT_SPRITES)->mesh_index + 1);
		else
			sptr->Def = (unsigned char)GetObjectInfo(currentLevel,DEFAULT_SPRITES)->mesh_index;

		fade = ((sptr->sLife - sptr->Life) << 16) / sptr->sLife;
		sptr->Yvel += sptr->Gravity;

		if(sptr->MaxYvel) {
			if(sptr->Yvel < 0 && sptr->Yvel < sptr->MaxYvel << 5 || sptr->Yvel > 0 && sptr->Yvel > sptr->MaxYvel << 5)
				sptr->Yvel = sptr->MaxYvel << 5;
		}

		if(sptr->Friction) {
			sptr->Xvel -= sptr->Xvel >> sptr->Friction;
			sptr->Zvel -= sptr->Zvel >> sptr->Friction;
		}

		sptr->x += sptr->Xvel >> 5;
		sptr->y += sptr->Yvel >> 5;
		sptr->z += sptr->Zvel >> 5;
		sptr->Size = (unsigned char)(sptr->sSize + ((fade * (sptr->dSize - sptr->sSize)) >> 16));
	}
}

void ClearFires() {
	FIRE_LIST* fire;

	for(int i = 0; i < nFires; i++) {
		fire = &fires[i];
		fire->on = 0;
	}
}

FIRE_LIST* GetFreeFire() {
	for(int i =0; i < nFires; ++i) {
		if(!fires[i].on) {
			return &fires[i];
		}
	}
	size_t idx = nFires;
	nFires = (nFires * 2) + 4;
	fires = realloc(fires, nFires * sizeof(FIRE_LIST));
	for(int i = idx; i < nFires; ++i) {
		FIRE_LIST* bptr = &fires[i];
		memset(bptr, 0, sizeof(FIRE_LIST));
	}
	return &fires[idx];
}

void AddFire(long x, long y, long z, long size, short room_number, short fade) {
	FIRE_LIST* fire;

	fire = GetFreeFire();

	if(fade)
		fire->on = (char)fade;
	else
		fire->on = 1;

	fire->pos.x = x;
	fire->pos.y = y;
	fire->pos.z = z;
	fire->size = (char)size;
	fire->room_number = room_number;
}

void S_DrawFires() {
	FIRE_LIST* fire;
	ROOM_INFO* r;
	short* bounds;
	short size;

	bounds = (short*)&tsv_buffer[0];

	for(int i = 0; i < nFires; i++) {
		fire = &fires[i];

		if(!fire->on)
			continue;

		if(fire->size == 2)
			size = 256;
		else
			size = 384;

		bounds[0] = -size;
		bounds[1] = size;
		bounds[2] = -size * 6;
		bounds[3] = size;
		bounds[4] = -size;
		bounds[5] = size;

		r = GetRoom(currentLevel,fire->room_number);
		phd_left = r->left;
		phd_right = r->right;
		phd_top = r->top;
		phd_bottom = r->bottom;

		phd_PushMatrix();
		phd_TranslateAbs(fire->pos.x, fire->pos.y, fire->pos.z);

		if(S_GetObjectInfoBounds(bounds)) {
			if(fire->on == 1)
				S_DrawFireSparks((unsigned char)fire->size, 255);
			else
				S_DrawFireSparks((unsigned char)fire->size, fire->on & 0xFF);
		}

		phd_PopMatrix();
	}

	phd_top = 0;
	phd_left = 0;
	phd_right = phd_winwidth;
	phd_bottom = phd_winheight;
}

void UpdateSmokeSparks() {
	SMOKE_SPARKS* sptr;
	long fade;

	for(int i = 0; i < nSmokeSparks; i++) {
		sptr = &smoke_spark[i];

		if(!sptr->On)
			continue;

		sptr->Life -= 2;

		if(sptr->Life <= 0) {
			sptr->On = 0;
			continue;
		}

		if(sptr->sLife - sptr->Life < sptr->ColFadeSpeed) {
			fade = ((sptr->sLife - sptr->Life) << 16) / sptr->ColFadeSpeed;
			sptr->Shade = (unsigned char)(sptr->sShade + (((sptr->dShade - sptr->sShade) * fade) >> 16));
		} else if(sptr->Life < sptr->FadeToBlack) {
			fade = ((sptr->Life - sptr->FadeToBlack) << 16) / sptr->FadeToBlack + 0x10000;
			sptr->Shade = (unsigned char)((sptr->dShade * fade) >> 16);

			if(sptr->Shade < 8) {
				sptr->On = 0;
				continue;
			}
		} else
			sptr->Shade = sptr->dShade;

		if(sptr->Shade < 24)
			sptr->Def = (unsigned char)(GetObjectInfo(currentLevel,DEFAULT_SPRITES)->mesh_index + 2);
		else if(sptr->Shade < 80)
			sptr->Def = (unsigned char)(GetObjectInfo(currentLevel,DEFAULT_SPRITES)->mesh_index + 1);
		else
			sptr->Def = (unsigned char)GetObjectInfo(currentLevel,DEFAULT_SPRITES)->mesh_index;

		if(sptr->Flags & 0x10)
			sptr->RotAng = (sptr->RotAng + sptr->RotAdd) & 0xFFF;

		fade = ((sptr->sLife - sptr->Life) << 16) / sptr->sLife;
		sptr->Yvel += sptr->Gravity;

		if(sptr->MaxYvel) {
			if(sptr->Yvel < 0 && sptr->Yvel < sptr->MaxYvel << 5 || sptr->Yvel > 0 && sptr->Yvel > sptr->MaxYvel << 5)
				sptr->Yvel = sptr->MaxYvel << 5;
		}

		if(sptr->Friction & 0xF) {
			sptr->Xvel -= sptr->Xvel >> (sptr->Friction & 0xF);
			sptr->Zvel -= sptr->Zvel >> (sptr->Friction & 0xF);
		}

		if(sptr->Friction & 0xF0)
			sptr->Yvel -= sptr->Yvel >> (sptr->Friction >> 4);

		sptr->pos.x += sptr->Xvel >> 5;
		sptr->pos.y += sptr->Yvel >> 5;
		sptr->pos.z += sptr->Zvel >> 5;

		if(sptr->Flags & 0x100) {
			sptr->pos.x += SmokeWindX >> 1;
			sptr->pos.z += SmokeWindZ >> 1;
		}

		sptr->Size = (unsigned char)(sptr->sSize + ((fade * (sptr->dSize - sptr->sSize)) >> 16));
	}
}

void TriggerShatterSmoke(long x, long y, long z) {
	SMOKE_SPARKS* sptr;

	sptr = GetFreeSmokeSpark();
	sptr->On = 1;
	sptr->sShade = 0;
	sptr->dShade = 255 - (GetRandomControl() & 0x1F);
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 24 - (GetRandomControl() & 7);
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 48;
	sptr->sLife = sptr->Life;
	sptr->pos.x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->pos.y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->pos.z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Yvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Zvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Friction = 7;

	if(GetRandomControl() & 1) {
		sptr->Flags = 16;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if(GetRandomControl() & 1)
			sptr->RotAdd = -64 - (GetRandomControl() & 0x3F);
		else
			sptr->RotAdd = (GetRandomControl() & 0x3F) + 64;
	} else if(GetRoom(currentLevel,lara_item->room_number)->flags & ROOM_NOT_INSIDE)
		sptr->Flags = 256;
	else
		sptr->Flags = 0;

	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->dSize = (GetRandomControl() & 0x3F) + 64;
	sptr->sSize = sptr->dSize >> 3;
	sptr->Size = sptr->dSize >> 3;
}

void DrawLensFlares(ITEM_INFO* item) {
	GAME_VECTOR sun;

	sun.pos.x = item->pos.x_pos;
	sun.pos.y = item->pos.y_pos;
	sun.pos.z = item->pos.z_pos;
	sun.room_number = item->room_number;
	SetUpLensFlare(0, 0, 0, &sun);
}

void DrawWeaponMissile(ITEM_INFO* item) {
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_PutPolygons_train(GetMesh(currentLevel,GetObjectInfo(currentLevel,item->object_number)->mesh_index), 0);
	phd_PopMatrix();

	if(gfLevelFlags & GF_MIRROR && item->room_number == gfMirrorRoom) {
		phd_PushMatrix();
		phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, 2 * gfMirrorZPlane - item->pos.z_pos);
		phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
		phd_PutPolygons_train(GetMesh(currentLevel,GetObjectInfo(currentLevel,item->object_number)->mesh_index), 0);
		phd_PopMatrix();
	}
}

GUNSHELL_STRUCT* GetFreeGunshell() {
	for(int i =0; i < nGunshells; ++i) {
		if(!Gunshells[i].counter) {
			return &Gunshells[i];
		}
	}
	size_t idx = nGunshells;
	nGunshells = (nGunshells * 2) + 4;
	Gunshells = realloc(Gunshells, nGunshells * sizeof(GUNSHELL_STRUCT));
	for(int i = idx; i < nGunshells; ++i) {
		GUNSHELL_STRUCT* bptr = &Gunshells[i];
		memset(bptr, 0, sizeof(GUNSHELL_STRUCT));
	}
	return &Gunshells[idx];
}

void TriggerGunShell(short leftright, short objnum, long weapon) {
	GUNSHELL_STRUCT* shell;
	PHD_VECTOR pos;
	long shade;

	if(leftright) {
		switch(weapon) {
		case WEAPON_PISTOLS:
			pos.x = 8;
			pos.y = 48;
			pos.z = 40;
			break;

		case WEAPON_UZI:
			pos.x = 8;
			pos.y = 35;
			pos.z = 48;
			break;

		case WEAPON_SHOTGUN:
			pos.x = 16;
			pos.y = 114;
			pos.z = 32;
			break;
		}

		GetLaraJointPos(&pos, 11);
	} else {
		switch(weapon) {
		case WEAPON_PISTOLS:
			pos.x = -12;
			pos.y = 48;
			pos.z = 40;
			break;

		case WEAPON_UZI:
			pos.x = -16;
			pos.y = 35;
			pos.z = 48;
			break;
		}

		GetLaraJointPos(&pos, 14);
	}

	if(lara_item->mesh_bits) {
		if(weapon == WEAPON_SHOTGUN)
			shade = 24;
		else
			shade = 16;

		TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, weapon, shade);
	}

	shell = GetFreeGunshell();
	shell->pos.x_pos = pos.x;
	shell->pos.y_pos = pos.y;
	shell->pos.z_pos = pos.z;
	shell->room_number = lara_item->room_number;
	shell->pos.x_rot = 0;
	shell->pos.y_rot = 0;
	shell->pos.z_rot = (short)GetRandomControl();
	shell->speed = (GetRandomControl() & 0x1F) + 16;
	shell->object_number = objnum;
	shell->fallspeed = -48 - (GetRandomControl() & 7);
	shell->counter = (GetRandomControl() & 0x1F) + 60;

	if(leftright) {
		if(weapon == WEAPON_SHOTGUN) {
			shell->DirXrot = lara.torso_y_rot + lara_item->pos.y_rot - (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot + 0x2800;
			shell->pos.y_rot += lara.left_arm.y_rot + lara.torso_y_rot + lara_item->pos.y_rot;

			if(shell->speed < 24)
				shell->speed += 24;
		} else
			shell->DirXrot = lara_item->pos.y_rot - (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot + 0x4800;
	} else
		shell->DirXrot = lara_item->pos.y_rot + (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot - 0x4800;
}

void UpdateGunShells() {
	GUNSHELL_STRUCT* shell;
	FLOOR_INFO* floor;
	long ox, oy, oz, c, h;
	short oroom;

	for(int i = 0; i < nGunshells; i++) {
		shell = &Gunshells[i];

		if(!shell->counter)
			continue;

		ox = shell->pos.x_pos;
		oy = shell->pos.y_pos;
		oz = shell->pos.z_pos;
		oroom = shell->room_number;
		shell->counter--;

		if(GetRoom(currentLevel,oroom)->flags & ROOM_UNDERWATER) {
			shell->fallspeed++;

			if(shell->fallspeed > 8)
				shell->fallspeed = 8;
			else if(shell->fallspeed < 0)
				shell->fallspeed >>= 1;

			shell->speed -= shell->speed >> 1;
		} else
			shell->fallspeed += 6;

		shell->pos.x_rot += 182 * ((shell->speed >> 1) + 7);
		shell->pos.y_rot += 182 * shell->speed;
		shell->pos.z_rot += 4186;
		shell->pos.x_pos += shell->speed * phd_sin(shell->DirXrot) >> (W2V_SHIFT + 1);
		shell->pos.y_pos += shell->fallspeed;
		shell->pos.z_pos += shell->speed * phd_cos(shell->DirXrot) >> (W2V_SHIFT + 1);
		floor = GetFloor(shell->pos.x_pos, shell->pos.y_pos, shell->pos.z_pos, &shell->room_number);

		if(GetRoom(currentLevel,shell->room_number)->flags & ROOM_UNDERWATER && !(GetRoom(currentLevel,oroom)->flags & ROOM_UNDERWATER)) {
			TriggerSmallSplash(shell->pos.x_pos, GetRoom(currentLevel,shell->room_number)->maxceiling, shell->pos.z_pos, 8);
			SetupRipple(shell->pos.x_pos, GetRoom(currentLevel,shell->room_number)->maxceiling, shell->pos.z_pos, (GetRandomControl() & 3) + 8, 2);
			shell->fallspeed >>= 5;
			continue;
		}

		c = GetCeiling(floor, shell->pos.x_pos, shell->pos.y_pos, shell->pos.z_pos);

		if(shell->pos.y_pos < c) {
			SoundEffect(SFX_LARA_SHOTGUN_SHELL, &shell->pos, SFX_DEFAULT);
			shell->speed -= 4;

			if(shell->speed < 8) {
				shell->counter = 0;
				continue;
			}

			shell->pos.y_pos = c;
			shell->fallspeed = -shell->fallspeed;
		}

		h = GetHeight(floor, shell->pos.x_pos, shell->pos.y_pos, shell->pos.z_pos);

		if(shell->pos.y_pos >= h) {
			SoundEffect(SFX_LARA_SHOTGUN_SHELL, &shell->pos, SFX_DEFAULT);
			shell->speed -= 8;

			if(shell->speed < 8) {
				shell->counter = 0;
				continue;
			}

			if(oy <= h)
				shell->fallspeed = -shell->fallspeed >> 1;
			else {
				shell->DirXrot += 0x8000;
				shell->pos.x_pos = ox;
				shell->pos.z_pos = oz;
			}

			shell->pos.y_pos = oy;
		}
	}
}

void TriggerSmallSplash(long x, long y, long z, long num) {
	SPARKS* sptr;
	short ang;

	while(num--) {
		sptr = GetFreeSpark();
		sptr->On = 1;
		sptr->sR = 112;
		sptr->sG = (GetRandomControl() & 0x1F) + 128;
		sptr->sB = (GetRandomControl() & 0x1F) + 128;
		sptr->dR = sptr->sR >> 1;
		sptr->dG = sptr->sG >> 1;
		sptr->dB = sptr->sB >> 1;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 8;
		sptr->Life = 24;
		sptr->sLife = 24;
		sptr->TransType = 2;
		ang = GetRandomControl() & 0xFFF;
		sptr->Xvel = -rcossin_tbl[ang << 1] >> 5;
		sptr->Yvel = -640 - (GetRandomControl() & 0xFF);
		sptr->Zvel = rcossin_tbl[(ang << 1) + 1] >> 5;
		sptr->x = x + (sptr->Xvel >> 3);
		sptr->y = y - (sptr->Yvel >> 5);
		sptr->z = z + (sptr->Zvel >> 3);
		sptr->Friction = 5;
		sptr->Flags = 0;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 0xF) + 64;
	}
}

GUNFLASH_STRUCT* GetFreeGunflash() {
	for(int i =0; i < nGunflashes; ++i) {
		if(!Gunflashes[i].on) {
			return &Gunflashes[i];
		}
	}
	size_t idx = nGunflashes;
	nGunflashes = (nGunflashes * 2) + 4;
	Gunflashes = realloc(Gunflashes, nGunflashes * sizeof(GUNFLASH_STRUCT));
	for(int i = idx; i < nGunflashes; ++i) {
		GUNFLASH_STRUCT* bptr = &Gunflashes[i];
		memset(bptr, 0, sizeof(GUNFLASH_STRUCT));
	}
	return &Gunflashes[idx];
}

void TriggerGunflash(PHD_VECTOR* pos) {
	GUNFLASH_STRUCT* flash;

	flash = GetFreeGunflash();
	phd_TranslateRel(pos->x, pos->y, pos->z);
	phd_RotX(-0x4000);
	flash->mx[M00] = mMXPtr[M00];
	flash->mx[M01] = mMXPtr[M01];
	flash->mx[M02] = mMXPtr[M02];
	flash->mx[M03] = mMXPtr[M03];
	flash->mx[M10] = mMXPtr[M10];
	flash->mx[M11] = mMXPtr[M11];
	flash->mx[M12] = mMXPtr[M12];
	flash->mx[M13] = mMXPtr[M13];
	flash->mx[M20] = mMXPtr[M20];
	flash->mx[M21] = mMXPtr[M21];
	flash->mx[M22] = mMXPtr[M22];
	flash->mx[M23] = mMXPtr[M23];
	flash->on = 1;
}

void SetGunFlash(short weapon) {
	PHD_VECTOR pos;

	switch(weapon) {
	case WEAPON_REVOLVER:
		pos.x = 0;
		pos.y = 190;
		pos.z = 50;
		break;

	case WEAPON_UZI:
		pos.x = 0;
		pos.y = 190;
		pos.z = 50;
		break;

	case WEAPON_SHOTGUN:
	case WEAPON_GRENADE:
	case WEAPON_CROSSBOW:
	case WEAPON_FLARE:
		return;

	default: // pistols
		pos.x = 0;
		pos.y = 180;
		pos.z = 40;
		break;
	}

	TriggerGunflash(&pos);
}

void DrawGunflashes() {
	GUNFLASH_STRUCT* flash;

	phd_top = 0;
	phd_left = 0;
	phd_right = phd_winwidth;
	phd_bottom = phd_winheight;
	phd_PushMatrix();
	GetRandomDraw();
	GetRandomDraw();
	GetRandomDraw();
	GetRandomDraw();

	for(int i = 0; i < nGunflashes; i++) {
		flash = &Gunflashes[i];

		if(!flash->on)
			break;

		mMXPtr[M00] = flash->mx[M00];
		mMXPtr[M01] = flash->mx[M01];
		mMXPtr[M02] = flash->mx[M02];
		mMXPtr[M03] = flash->mx[M03];
		mMXPtr[M10] = flash->mx[M10];
		mMXPtr[M11] = flash->mx[M11];
		mMXPtr[M12] = flash->mx[M12];
		mMXPtr[M13] = flash->mx[M13];
		mMXPtr[M20] = flash->mx[M20];
		mMXPtr[M21] = flash->mx[M21];
		mMXPtr[M22] = flash->mx[M22];
		mMXPtr[M23] = flash->mx[M23];
		phd_RotZ((short)(GetRandomDraw() << 1));
		GlobalAmbient = 0xFF2F2F00;
		phd_PutPolygons(GetMesh(currentLevel,GetObjectInfo(currentLevel,GUN_FLASH)->mesh_index), -1);
		flash->on = 0;
	}

	phd_PopMatrix();
}

BLOOD_STRUCT* GetFreeBlood() {
	for(int i =0; i < nBlood; ++i) {
		if(!blood[i].On) {
			return &blood[i];
		}
	}
	size_t idx = nBlood;
	nBlood = (nBlood * 2) + 4;
	blood = realloc(blood, nBlood * sizeof(BLOOD_STRUCT));
	for(int i = idx; i < nBlood; ++i) {
		BLOOD_STRUCT* bptr = &blood[i];
		memset(bptr, 0, sizeof(BLOOD_STRUCT));
	}
	return &blood[idx];
}

void UpdateBlood() {
	BLOOD_STRUCT* bptr;
	long fade;

	for(int i = 0; i < nBlood; i++) {
		bptr = &blood[i];

		if(!bptr->On)
			continue;

		bptr->Life--;

		if(bptr->Life <= 0) {
			bptr->On = 0;
			continue;
		}

		if(bptr->sLife - bptr->Life < bptr->ColFadeSpeed) {
			fade = ((bptr->sLife - bptr->Life) << 16) / bptr->ColFadeSpeed;
			bptr->Shade = (unsigned char)(bptr->sShade + ((fade * (bptr->dShade - bptr->sShade)) >> 16));
		} else {
			if(bptr->Life < bptr->FadeToBlack) {
				fade = ((bptr->Life - bptr->FadeToBlack) << 16) / bptr->FadeToBlack + 0x10000;
				bptr->Shade = (unsigned char)((bptr->dShade * fade) >> 16);

				if(bptr->Shade < 8) {
					bptr->On = 0;
					continue;
				}
			} else
				bptr->Shade = bptr->dShade;
		}

		bptr->RotAng = (bptr->RotAng + bptr->RotAdd) & 0xFFF;
		bptr->Yvel += bptr->Gravity;
		fade = ((bptr->sLife - bptr->Life) << 16) / bptr->sLife;

		if(bptr->Friction & 0xF) {
			bptr->Xvel -= bptr->Xvel >> (bptr->Friction & 0xF);
			bptr->Zvel -= bptr->Zvel >> (bptr->Friction & 0xF);
		}

		bptr->x += bptr->Xvel >> 5;
		bptr->y += bptr->Yvel >> 5;
		bptr->z += bptr->Zvel >> 5;
		bptr->Size = (unsigned char)(bptr->sSize + ((fade * (bptr->dSize - bptr->sSize)) >> 16));
	}
}

void TriggerBlood(long x, long y, long z, long angle, long num) {
	BLOOD_STRUCT* bptr;
	short ang, speed;
	unsigned char size;

	for(int i = 0; i < num; i++) {
		bptr = GetFreeBlood();
		bptr->On = 1;
		bptr->sShade = 0;
		bptr->ColFadeSpeed = 4;
		bptr->FadeToBlack = 8;
		bptr->dShade = (GetRandomControl() & 0x3F) + 48;
		bptr->Life = (GetRandomControl() & 7) + 24;
		bptr->sLife = bptr->Life;
		bptr->x = (GetRandomControl() & 0x1F) + x - 16;
		bptr->y = (GetRandomControl() & 0x1F) + y - 16;
		bptr->z = (GetRandomControl() & 0x1F) + z - 16;

		if(angle == -1)
			ang = (short)GetRandomControl();
		else
			ang = (short)((GetRandomControl() & 0x1F) + angle - 16);

		ang &= 0xFFF;
		speed = GetRandomControl() & 0xF;
		bptr->Xvel = -(speed * rcossin_tbl[ang << 1]) >> 7;
		bptr->Zvel = speed * rcossin_tbl[(ang << 1) + 1] >> 7;
		bptr->Friction = 4;
		bptr->Yvel = -128 - (GetRandomControl() & 0xFF);
		bptr->RotAng = GetRandomControl() & 0xFFF;

		if(GetRandomControl() & 1)
			bptr->RotAdd = -64 - (GetRandomControl() & 0x3F);
		else
			bptr->RotAdd = (GetRandomControl() & 0x3F) + 64;

		bptr->Gravity = (GetRandomControl() & 0x1F) + 31;
		size = (GetRandomControl() & 7) + 8;
		bptr->sSize = size;
		bptr->Size = size;
		bptr->dSize = size >> 2;
	}
}

BUBBLE_STRUCT* GetFreeBubble() {
	for(int i =0; i < nBubbles; ++i) {
		if(!Bubbles[i].size) {
			return &Bubbles[i];
		}
	}
	size_t idx = nBubbles;
	nBubbles = (nBubbles * 2) + 4;
	Bubbles = realloc(Bubbles, nBubbles * sizeof(BUBBLE_STRUCT));
	for(int i = idx; i < nBubbles; ++i) {
		BUBBLE_STRUCT* bptr = &Bubbles[i];
		memset(bptr, 0, sizeof(BUBBLE_STRUCT));
	}
	return &Bubbles[idx];
}

void CreateBubble(PHD_3DPOS* pos, short room_number, long size, long biggest) {
	BUBBLE_STRUCT* bubble;

	GetFloor(pos->x_pos, pos->y_pos, pos->z_pos, &room_number);

	if(GetRoom(currentLevel,room_number)->flags & ROOM_UNDERWATER) {
		bubble = GetFreeBubble();
		bubble->pos.x = pos->x_pos;
		bubble->pos.y = pos->y_pos;
		bubble->pos.z = pos->z_pos;
		bubble->room_number = room_number;
		bubble->speed = (GetRandomControl() & 0xFF) + 64;
		bubble->shade = 0;
		bubble->size = (short)((size + (biggest & GetRandomControl())) << 1);
		bubble->dsize = bubble->size << 4;
		bubble->vel = (GetRandomControl() & 0x1F) + 32;
	}
}

void UpdateBubbles() {
	BUBBLE_STRUCT* bubble;
	FLOOR_INFO* floor;
	long h, c;
	short room_number;

	for(int i = 0; i < nBubbles; i++) {
		bubble = &Bubbles[i];

		if(!bubble->size)
			continue;

		bubble->pad += 6;
		bubble->speed += bubble->vel;
		bubble->pos.x += (3 * phd_sin(bubble->pad << 8)) >> W2V_SHIFT;
		bubble->pos.y -= bubble->speed >> 8;
		bubble->pos.z += phd_cos(bubble->pad << 8) >> W2V_SHIFT;

		room_number = bubble->room_number;
		floor = GetFloor(bubble->pos.x, bubble->pos.y, bubble->pos.z, &room_number);
		h = GetHeight(floor, bubble->pos.x, bubble->pos.y, bubble->pos.z);

		if(bubble->pos.y > h || !floor) {
			bubble->size = 0;
			continue;
		}

		if(!(GetRoom(currentLevel,room_number)->flags & ROOM_UNDERWATER)) {
			SetupRipple(bubble->pos.x, GetRoom(currentLevel,bubble->room_number)->maxceiling, bubble->pos.z, (GetRandomControl() & 0xF) + 48, 2);
			bubble->size = 0;
			continue;
		}

		c = GetCeiling(floor, bubble->pos.x, bubble->pos.y, bubble->pos.z);

		if(c == NO_HEIGHT || bubble->pos.y <= c) {
			bubble->size = 0;
			continue;
		}

		if(bubble->size < bubble->dsize)
			bubble->size++;

		if(bubble->shade < 144)
			bubble->shade += 2;

		bubble->room_number = room_number;
	}
}

DRIP_STRUCT* GetFreeDrip() {
	for(int i =0; i < nDrips; ++i) {
		if(!Drips[i].On) {
			return &Drips[i];
		}
	}
	size_t idx = nDrips;
	nDrips = (nDrips * 2) + 4;
	Drips = realloc(Drips, nDrips * sizeof(DRIP_STRUCT));
	for(int i = idx; i < nDrips; ++i) {
		DRIP_STRUCT* bptr = &Drips[i];
		memset(bptr, 0, sizeof(DRIP_STRUCT));
	}
	return &Drips[idx];
}

void TriggerLaraDrips() {
	DRIP_STRUCT* drip;
	PHD_VECTOR pos;

	if(wibble & 0xF)
		return;

	for(int i = 14; i > 0; i--) {
		if(lara.wet[i] && !LaraNodeUnderwater[i] && (GetRandomControl() & 0x1FF) < lara.wet[i]) {
			pos.x = (GetRandomControl() & 0x1F) - 16;
			pos.y = (GetRandomControl() & 0xF) + 16;
			pos.z = (GetRandomControl() & 0x1F) - 16;
			GetLaraJointPos(&pos, i);

			drip = GetFreeDrip();
			drip->pos.x = pos.x;
			drip->pos.y = pos.y;
			drip->pos.z = pos.z;
			drip->On = 1;
			drip->R = (GetRandomControl() & 7) + 16;
			drip->G = (GetRandomControl() & 7) + 24;
			drip->B = (GetRandomControl() & 7) + 32;
			drip->Yvel = (GetRandomControl() & 0x1F) + 32;
			drip->Gravity = (GetRandomControl() & 0x1F) + 32;
			drip->Life = (GetRandomControl() & 0x1F) + 16;
			drip->RoomNumber = lara_item->room_number;
			lara.wet[i] -= 4;
		}
	}
}

SHOCKWAVE_STRUCT* GetFreeShockwave() {
	for(int i =0; i < nShockWaves; ++i) {
		if(!ShockWaves[i].life) {
			return &ShockWaves[i];
		}
	}
	size_t idx = nShockWaves;
	nShockWaves = (nShockWaves * 2) + 4;
	ShockWaves = realloc(ShockWaves, nShockWaves * sizeof(BLOOD_STRUCT));
	for(int i = idx; i < nShockWaves; ++i) {
		SHOCKWAVE_STRUCT* bptr = &ShockWaves[i];
		memset(bptr, 0, sizeof(SHOCKWAVE_STRUCT));
	}
	return &ShockWaves[idx];
}

void TriggerShockwave(PHD_VECTOR* pos, long InnerOuterRads, long speed, long bgrl, long XRotFlags) {
	SHOCKWAVE_STRUCT* sw;

	sw = GetFreeShockwave();
	sw->x = pos->x;
	sw->y = pos->y;
	sw->z = pos->z;
	sw->InnerRad = InnerOuterRads & 0xFFFF;
	sw->OuterRad = InnerOuterRads >> 16;
	sw->XRot = XRotFlags & 0xFFFF;
	sw->Flags = XRotFlags >> 16;
	sw->Speed = (short)speed;
	sw->r = CLRB(bgrl);
	sw->g = CLRG(bgrl);
	sw->b = CLRR(bgrl);
	sw->life = CLRA(bgrl);
	SoundEffect(SFX_DEMI_SIREN_SWAVE, (PHD_3DPOS*)pos, SFX_DEFAULT);
	
}

void TriggerShockwaveHitEffect(long x, long y, long z, long rgb, short dir, long speed) {
	SPARKS* sptr;
	long dx, dz, xvel, zvel;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if(dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = GetFreeSpark();
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = CLRR(rgb);
	sptr->dG = CLRG(rgb);
	sptr->dB = CLRB(rgb);
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 16;
	sptr->sLife = sptr->Life;
	speed += GetRandomControl() & 0xF;
	xvel = speed * phd_sin(dir) >> (W2V_SHIFT - 4);
	zvel = speed * phd_cos(dir) >> (W2V_SHIFT - 4);

	if(GetRandomControl() & 1)
		dir += 0x4000;
	else
		dir -= 0x4000;

	speed = (GetRandomControl() & 0x1FF) - 256;
	x += speed * phd_sin(dir) >> W2V_SHIFT;
	z += speed * phd_cos(dir) >> W2V_SHIFT;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = (short)xvel;
	sptr->Yvel = -512 - (GetRandomControl() & 0x1FF);
	sptr->Zvel = (short)zvel;
	sptr->Friction = 3;
	sptr->Flags = 538;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if(GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Scalar = 1;
	sptr->Def = (unsigned char)(GetObjectInfo(currentLevel,DEFAULT_SPRITES)->mesh_index + 14);
	sptr->MaxYvel = 0;
	sptr->Gravity = (GetRandomControl() & 0x3F) + 64;
	sptr->Size = (GetRandomControl() & 0x1F) + 32;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 2;
}

void UpdateShockwaves() {
	SHOCKWAVE_STRUCT* sw;
	short* bounds;
	long dx, dz, dist;
	short dir;

	for(int i = 0; i < nShockWaves; i++) {
		sw = &ShockWaves[i];

		if(!sw->life)
			continue;

		sw->life--;

		if(!sw->life)
			continue;

		sw->OuterRad += sw->Speed;
		sw->InnerRad += sw->Speed >> 1;
		sw->Speed -= sw->Speed >> 4;

		if(lara_item->hit_points >= 0 && sw->Flags & 3) {
			bounds = GetBestFrame(lara_item);
			dx = lara_item->pos.x_pos - sw->x;
			dz = lara_item->pos.z_pos - sw->z;
			dist = phd_sqrt(SQUARE(dx) + SQUARE(dz));

			if(sw->y > lara_item->pos.y_pos + bounds[2] && sw->y < bounds[3] + lara_item->pos.y_pos + 256 && dist > sw->InnerRad && dist < sw->OuterRad) {
				dir = (short)phd_atan(dz, dx);
				TriggerShockwaveHitEffect(lara_item->pos.x_pos, sw->y, lara_item->pos.z_pos, *(long*)&sw->r, dir, sw->Speed);
				lara_item->hit_points -= sw->Speed >> (((sw->Flags & 2) != 0) + 2);
			} else
				sw->Temp = 0;
		}
	}
}

void UpdateLightning() {
	LIGHTNING_STRUCT* lptr;
	long* pPoint;
	char* pVel;

	for(int i = 0; i < nLightnings; i++) {
		lptr = &Lightning[i];

		if(!lptr->Life)
			continue;

		lptr->Life -= 2;

		pPoint = &lptr->Point[1].x;
		pVel = &lptr->Xvel1;

		for(int j = 0; j < 9; j++) {
			*pPoint++ += *pVel << 1;
			*pVel -= *pVel >> 4;
			pVel++;
		}
	}
}

long LSpline(long x, long* knots, long nk) {
	long* k;
	long c1, c2, c3, ret, span;

	x *= nk - 3;
	span = x >> 16;

	if(span >= nk - 3)
		span = nk - 4;

	x -= 65536 * span;
	k = &knots[3 * span];
	c1 = k[3] + (k[3] >> 1) - (k[6] >> 1) - k[6] + (k[9] >> 1) + ((-k[0] - 1) >> 1);
	ret = (long long)c1 * x >> 16;
	c2 = ret + 2 * k[6] - 2 * k[3] - (k[3] >> 1) - (k[9] >> 1) + k[0];
	ret = (long long)c2 * x >> 16;
	c3 = ret + (k[6] >> 1) + ((-k[0] - 1) >> 1);
	ret = (long long)c3 * x >> 16;
	return ret + k[3];
}

void CalcLightningSpline(PHD_VECTOR* pos, PHD_VECTOR* dest, LIGHTNING_STRUCT* lptr) {
	long segments, x, y, z, xadd, yadd, zadd;

	dest->x = (short)pos->x;
	dest->y = (short)pos->y;
	dest->z = (short)pos->z;
	dest++;
	segments = lptr->Segments * 3;

	if(lptr->Flags & 1) {
		xadd = 0x10000 / (segments - 1);
		x = xadd;

		for(int i = 0; i < segments - 2; i++) {
			dest->x = (short)(LSpline(x, &pos->x, 6) + (GetRandomControl() & 0xF) - 8);
			dest->y = (short)(LSpline(x, &pos->y, 6) + (GetRandomControl() & 0xF) - 8);
			dest->z = (short)(LSpline(x, &pos->z, 6) + (GetRandomControl() & 0xF) - 8);
			dest++;
			x += xadd;
		}
	} else {
		xadd = (pos[5].x - pos->x) / (segments - 1);
		yadd = (pos[5].y - pos->y) / (segments - 1);
		zadd = (pos[5].z - pos->z) / (segments - 1);
		x = xadd + pos->x + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;
		y = yadd + pos->y + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;
		z = zadd + pos->z + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;

		for(int i = 0; i < segments - 2; i++) {
			dest->x = (short)x;
			dest->y = (short)y;
			dest->z = (short)z;
			dest++;
			x += xadd + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;
			y += yadd + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;
			z += zadd + GetRandomControl() % (lptr->Rand << 1) - lptr->Rand;
		}
	}

	dest->x = (short)pos[5].x;
	dest->y = (short)pos[5].y;
	dest->z = (short)pos[5].z;
}

void TriggerLightningGlow(long x, long y, long z, long rgb) {
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if(dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = GetFreeSpark();
	sptr->On = 1;
	sptr->sR = CLRR(rgb);
	sptr->sG = CLRG(rgb);
	sptr->sB = CLRB(rgb);
	sptr->dR = sptr->sR;
	sptr->dG = sptr->sG;
	sptr->dB = sptr->sB;
	sptr->Life = 4;
	sptr->sLife = 4;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 0;
	sptr->TransType = 2;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Zvel = 0;
	sptr->Yvel = 0;
	sptr->Xvel = 0;
	sptr->Flags = 10;
	sptr->Scalar = 3;
	sptr->MaxYvel = 0;
	sptr->Def = GetObjectInfo(currentLevel,DEFAULT_SPRITES)->mesh_index + 11;
	sptr->Gravity = 0;
	sptr->Size = (rgb >> 24) + (GetRandomControl() & 3);
	sptr->dSize = sptr->Size;
	sptr->sSize = sptr->Size;
}

void TriggerFlashSmoke(long x, long y, long z, short room_number) {
	SMOKE_SPARKS* sptr;
	long uw;

	if(GetRoom(currentLevel,room_number)->flags & ROOM_UNDERWATER) {
		TriggerExplosionBubble(x, y, z, (short)room_number);
		uw = 1;
	} else
		uw = 0;

	sptr = GetFreeSmokeSpark();
	sptr->On = 1;
	sptr->sShade = 0;
	sptr->dShade = 128;
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 16;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 0xF) + 64;
	sptr->sLife = sptr->Life;
	sptr->pos.x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->pos.y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->pos.z = (GetRandomControl() & 0x1F) + z - 16;

	if(uw) {
		sptr->Xvel = (GetRandomControl() & 0x3FF) - 512;
		sptr->Yvel = (GetRandomControl() & 0x3FF) - 512;
		sptr->Zvel = (GetRandomControl() & 0x3FF) - 512;
		sptr->Friction = 68;
	} else {
		sptr->Xvel = 2 * (GetRandomControl() & 0x3FF) - 1024;
		sptr->Yvel = -512 - (GetRandomControl() & 0x3FF);
		sptr->Zvel = 2 * (GetRandomControl() & 0x3FF) - 1024;
		sptr->Friction = 85;
	}

	if(GetRoom(currentLevel,room_number)->flags & ROOM_NOT_INSIDE)
		sptr->Flags = 272;
	else
		sptr->Flags = 16;

	sptr->RotAng = GetRandomControl() & 0xFFF;

	if(GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Size = (GetRandomControl() & 0x1F) + 64;
	sptr->sSize = sptr->Size;
	sptr->dSize = (sptr->Size + 4) << 1;
	sptr->mirror = room_number == gfMirrorRoom;
}

void S_DrawSparks() {
	SPARKS* sptr;
	FX_INFO* fx;
	ITEM_INFO* item;
	PHD_VECTOR pos;
	FVECTOR fPos;
	long* XY;
	long* Z;
	long* offsets;
	float perspz;
	long x, y, z, smallest_size;

	smallest_size = 0;

	for(int i = 0; i < 16; i++)
		NodeOffsets[i].GotIt = 0;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	XY = (long*)&tsv_buffer[0];
	Z = (long*)&tsv_buffer[512];
	offsets = (long*)&tsv_buffer[1024];

	for(int i = 0; i < nSpark; i++) {
		sptr = &spark[i];

		if(!sptr->On)
			continue;

		if(sptr->Flags & 0x40) {
			fx = GetEffect(currentLevel,sptr->FxObj);
			x = sptr->x + fx->pos.x_pos;
			y = sptr->y + fx->pos.y_pos;
			z = sptr->z + fx->pos.z_pos;

			if(sptr->sLife - sptr->Life > (GetRandomDraw() & 7) + 4) {
				sptr->x = x;
				sptr->y = y;
				sptr->z = z;
				sptr->Flags &= ~0x40;
			}
		} else if(sptr->Flags & 0x80) {
			item = GetItem(currentLevel,sptr->FxObj);

			if(sptr->Flags & 0x1000) {
				if(NodeOffsets[sptr->NodeNumber].GotIt) {
					pos.x = NodeVectors[sptr->NodeNumber].x;
					pos.y = NodeVectors[sptr->NodeNumber].y;
					pos.z = NodeVectors[sptr->NodeNumber].z;
				} else {
					pos.x = NodeOffsets[sptr->NodeNumber].x;
					pos.y = NodeOffsets[sptr->NodeNumber].y;
					pos.z = NodeOffsets[sptr->NodeNumber].z;

					if(NodeOffsets[sptr->NodeNumber].mesh_num < 0)
						GetLaraJointPos(&pos, -NodeOffsets[sptr->NodeNumber].mesh_num);
					else
						GetJointAbsPosition(item, &pos, NodeOffsets[sptr->NodeNumber].mesh_num);

					NodeOffsets[sptr->NodeNumber].GotIt = 1;
					NodeVectors[sptr->NodeNumber].x = pos.x;
					NodeVectors[sptr->NodeNumber].y = pos.y;
					NodeVectors[sptr->NodeNumber].z = pos.z;
				}

				x = sptr->x + pos.x;
				y = sptr->y + pos.y;
				z = sptr->z + pos.z;

				if(sptr->sLife - sptr->Life > (GetRandomDraw() & 3) + 8) {
					sptr->x = x;
					sptr->y = y;
					sptr->z = z;
					sptr->Flags &= ~0x1080;
				}
			} else {
				x = sptr->x + item->pos.x_pos;
				y = sptr->y + item->pos.y_pos;
				z = sptr->z + item->pos.z_pos;
			}
		} else {
			x = sptr->x;
			y = sptr->y;
			z = sptr->z;
		}

		x -= lara_item->pos.x_pos;
		y -= lara_item->pos.y_pos;
		z -= lara_item->pos.z_pos;

		if(x < -0x5000 || x > 0x5000 || y < -0x5000 || y > 0x5000 || z < -0x5000 || z > 0x5000) {
			sptr->On = 0;
			continue;
		}

		offsets[0] = x;
		offsets[1] = y;
		offsets[2] = z;
		fPos.x = mMXPtr[M00] * offsets[0] + mMXPtr[M01] * offsets[1] + mMXPtr[M02] * offsets[2] + mMXPtr[M03];
		fPos.y = mMXPtr[M10] * offsets[0] + mMXPtr[M11] * offsets[1] + mMXPtr[M12] * offsets[2] + mMXPtr[M13];
		fPos.z = mMXPtr[M20] * offsets[0] + mMXPtr[M21] * offsets[1] + mMXPtr[M22] * offsets[2] + mMXPtr[M23];
		perspz = f_persp / fPos.z;
		XY[0] = (long)(fPos.x * perspz + f_centerx);
		XY[1] = (long)(fPos.y * perspz + f_centery);
		Z[0] = (long)fPos.z;

		if(sptr->Flags & 8) {
			if(sptr->Flags & 2)
				smallest_size = 4;
		} else {
			offsets[0] = x - (sptr->Xvel >> 4);
			offsets[1] = y - (sptr->Yvel >> 4);
			offsets[2] = z - (sptr->Zvel >> 4);
			fPos.x = mMXPtr[M00] * offsets[0] + mMXPtr[M01] * offsets[1] + mMXPtr[M02] * offsets[2] + mMXPtr[M03];
			fPos.y = mMXPtr[M10] * offsets[0] + mMXPtr[M11] * offsets[1] + mMXPtr[M12] * offsets[2] + mMXPtr[M13];
			fPos.z = mMXPtr[M20] * offsets[0] + mMXPtr[M21] * offsets[1] + mMXPtr[M22] * offsets[2] + mMXPtr[M23];
			perspz = f_persp / fPos.z;
			XY[2] = (long)(fPos.x * perspz + f_centerx);
			XY[3] = (long)(fPos.y * perspz + f_centery);
			Z[1] = (long)fPos.z;
		}

		S_DrawDrawSparks(sptr, smallest_size, XY, Z);
	}

	phd_PopMatrix();
}

void SetFadeClip(short height, short speed) {
	DestFadeScreenHeight = height;
	FadeClipSpeed = speed;
}

void UpdateFadeClip() {
	if(DestFadeScreenHeight < FadeScreenHeight) {
		FadeScreenHeight -= FadeClipSpeed;

		if(DestFadeScreenHeight > FadeScreenHeight)
			FadeScreenHeight = DestFadeScreenHeight;
	} else if(DestFadeScreenHeight > FadeScreenHeight) {
		FadeScreenHeight += FadeClipSpeed;

		if(DestFadeScreenHeight < FadeScreenHeight)
			FadeScreenHeight = DestFadeScreenHeight;
	}
}

void SetScreenFadeOut(short speed, short back) {
	if(!ScreenFading) {
		ScreenFading = 1;
		ScreenFade = 0;
		dScreenFade = 255;
		ScreenFadeSpeed = speed;
		ScreenFadeBack = back;
		ScreenFadedOut = 0;
	}
}

void SetScreenFadeIn(short speed) {
	if(!ScreenFading) {
		ScreenFading = 1;
		ScreenFade = 255;
		dScreenFade = 0;
		ScreenFadeSpeed = speed;
		ScreenFadedOut = 0;
	}
}

void Fade() {
	long oldfucker;

	oldfucker = ScreenFade;

	if(dScreenFade && dScreenFade >= ScreenFade) {
		ScreenFade += ScreenFadeSpeed;

		if(ScreenFade > dScreenFade) {
			ScreenFade = dScreenFade;

			if(oldfucker >= dScreenFade) {
				ScreenFadedOut = 1;

				if(ScreenFadeBack) {
					dScreenFade = 0;
					ScreenFadeBack = 0;
				} else
					ScreenFading = 0;
			}
		}
	} else if(dScreenFade < ScreenFade) {
		ScreenFade -= ScreenFadeSpeed;

		if(ScreenFade < dScreenFade) {
			ScreenFade = dScreenFade;
			ScreenFading = 0;
		}
	}

	if(ScreenFade || dScreenFade)
		DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), RGBA(ScreenFade, ScreenFade, ScreenFade, 98), 2, 0);
}

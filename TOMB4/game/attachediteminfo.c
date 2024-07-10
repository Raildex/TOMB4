#include "game/attachediteminfo.h"
#include "control.h"
#include "game/delstuff.h"
#include "game/draw.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/levelinfo.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/sphere.h"
#include "specific/3dmath.h"
#include "specific/output.h"

void AttachedItemControl(short item_num) {
	ITEM_INFO* item = GetItem(currentLevel, item_num);
	ATTACHEDITEM_INFO* info = item->data;
	if(!item)
		return;
	PHD_3DPOS pos;
	pos.x_pos = -16;
	pos.y_pos = 64;
	pos.z_pos = 0;
	pos.x_rot = 0;
	pos.y_rot = 0;
	pos.z_rot = 16384 / 4;
	ITEM_INFO* carrier = GetItem(currentLevel, info->item_carrier);
	GetJointAbsPositionRotation(carrier, &pos, info->node);
	item->pos = pos;
	if(item->room_number != carrier->room_number) {
		ItemNewRoom(item_num, carrier->room_number);
	}
}

void DrawAttachedItem(ITEM_INFO* item) {
	ATTACHEDITEM_INFO* info = item->data;
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotX(item->pos.x_rot);
	phd_RotZ(item->pos.z_rot);
	phd_RotY(item->pos.y_rot);
	phd_PutPolygons(GetMesh(currentLevel, GetObjectInfo(currentLevel, info->object)->mesh_index), -1);
	phd_PopMatrix();
}
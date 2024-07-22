
#include "specific/drawroom.h"
#include "game/camera.h"
#include "game/control.h"
#include "game/draw.h"
#include "game/dynamic.h"
#include "game/effect2.h"
#include "game/fogbulbstruct.h"
#include "game/fvector.h"
#include "game/gameflow.h"
#include "game/gfleveloptions.h"
#include "game/levelinfo.h"
#include "game/lightinfo.h"
#include "game/lighttypes.h"
#include "game/meshdata.h"
#include "game/pclightinfo.h"
#include "game/polyface.h"
#include "game/roomdynamic.h"
#include "game/roominfo.h"
#include "game/texturestruct.h"
#include "game/watertab.h"
#include "specific/3dmath.h"
#include "specific/file.h"
#include "specific/function_stubs.h"
#include "specific/function_table.h"
#include "specific/gamemain.h"
#include "specific/lighting.h"
#include "specific/polyinsert.h"
#include "specific/windows/d3dmatrix.h"
#include "specific/windows/dxshell.h"
#include "specific/windows/texturebucket.h"
#include "specific/windows/winmain.h"
#include <string.h>
#include <d3dtypes.h>
#include <math.h>

static ROOM_DYNAMIC RoomDynamics[MAX_DYNAMICS];
static long nRoomDynamics;

MESH_DATA** mesh_vtxbuf;
long nBuckets;
TEXTUREBUCKET* Bucket;
float clip_left;
float clip_top;
float clip_right;
float clip_bottom;
long bWaterEffect;
long num_level_meshes;
WATERTAB WaterTable[22][64];
short clipflags[8192];
float vert_wibble_table[32];
static unsigned char water_abs[4] = { 4, 8, 12, 16 };
static short water_shimmer[4] = { 31, 63, 95, 127 };
static short water_choppy[4] = { 16, 53, 90, 127 };

long water_color_R = 128;
long water_color_G = 224;
long water_color_B = 255;

void ProcessRoomDynamics(ROOM_INFO* r) {
	// Collect dynamic lights for room lighting

	ROOM_DYNAMIC* l;
	DYNAMIC* d;
	float falloff;

	nRoomDynamics = 0;
	l = RoomDynamics;

	for(int i = 0; i < MAX_DYNAMICS; i++) {
		d = &dynamics[i];

		if(!d->on) {
			continue;
		}

		falloff = (float)((d->falloff >> 1) + (d->falloff >> 3));
		l->x = d->pos.x - r->posx;
		l->y = d->pos.y - r->posy;
		l->z = d->pos.z - r->posz;
		l->r = (float)d->r * (1.0F / 255.0F);
		l->g = (float)d->g * (1.0F / 255.0F);
		l->b = (float)d->b * (1.0F / 255.0F);
		l->falloff = falloff;
		l->inv_falloff = 1.0F / l->falloff;
		l->sqr_falloff = SQUARE(l->falloff);
		l++;
		nRoomDynamics++;
	}
}

void ProcessRoomVertices(ROOM_INFO* r) {
	// Transform, project, and light vertices, and store them in MyVertexBuffer.

	ROOM_DYNAMIC* l;
	FVECTOR lPos;
	FVECTOR vPos;
	FVECTOR vtx;
	FVECTOR n;
	short* clip;
	static float DistanceFogStart;
	float zv, fR, fG, fB, val, val2, num;
	long cR, cG, cB, sA, sR, sG, sB, rndoff, col;
	short clipFlag;
	unsigned char rnd, abs;
	char shimmer;

	clip = clipflags;

	if(gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6) {
		DistanceFogStart = 12.0F * 1024.0F;
	} else {
		DistanceFogStart = 12 * 1024.0F;
	}

	num = 255.0F / DistanceFogStart;

	for(int i = 0; i < r->nVerts; i++) {
		vtx.x = r->x + r->verts[i].x;
		vtx.y = r->y + r->verts[i].y;
		vtx.z = r->z + r->verts[i].z;
		n.x = r->vnormals[i].x;
		n.y = r->vnormals[i].y;
		n.z = r->vnormals[i].z;

		rndoff = (long)((vtx.x / 64.0F) + (vtx.y / 64.0F) + (vtx.z / 128.0F)) & 0xFC;

		if(i < r->nWaterVerts) {
			rnd = WaterTable[r->MeshEffect][rndoff & 0x3F].random;
			vtx.y += WaterTable[r->MeshEffect][((wibble >> 2) + rnd) & 0x3F].choppy;
		}

		vPos.x = vtx.x * D3DMView._11 + vtx.y * D3DMView._21 + vtx.z * D3DMView._31 + D3DMView._41;
		vPos.y = vtx.x * D3DMView._12 + vtx.y * D3DMView._22 + vtx.z * D3DMView._32 + D3DMView._42;
		vPos.z = vtx.x * D3DMView._13 + vtx.y * D3DMView._23 + vtx.z * D3DMView._33 + D3DMView._43;

		MyVertexBuffer[i].tu = vPos.x;
		MyVertexBuffer[i].tv = vPos.y;

		clipFlag = 0;

		if(vPos.z < f_mznear) {
			clipFlag = -128;
		} else {
			zv = f_mpersp / vPos.z;

			if(gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6) {
				if(vPos.z > FogEnd) {
					clipFlag = 16;
					vPos.z = f_zfar;
				}
			}

			vPos.x = vPos.x * zv + f_centerx;
			vPos.y = vPos.y * zv + f_centery;

			if(i >= r->nWaterVerts && camera.underwater) {
				vPos.x += vert_wibble_table[((wibble + (long)vPos.y) >> 3) & 0x1F];
				vPos.y += vert_wibble_table[((wibble + (long)vPos.x) >> 3) & 0x1F];
			}

			MyVertexBuffer[i].rhw = zv * f_moneopersp;

			if(vPos.x < clip_left) {
				clipFlag++;
			} else if(vPos.x > clip_right) {
				clipFlag += 2;
			}

			if(vPos.y < clip_top) {
				clipFlag += 4;
			} else if(vPos.y > clip_bottom) {
				clipFlag += 8;
			}
		}

		*clip++ = clipFlag;
		MyVertexBuffer[i].sx = vPos.x;
		MyVertexBuffer[i].sy = vPos.y;
		MyVertexBuffer[i].sz = vPos.z;

		if(i >= r->nShoreVerts && camera.underwater) {
			cR = CLRR(r->prelightwater[i]);
			cG = CLRG(r->prelightwater[i]);
			cB = CLRB(r->prelightwater[i]);
		} else {
			cR = CLRR(r->prelight[i]);
			cG = CLRG(r->prelight[i]);
			cB = CLRB(r->prelight[i]);
		}

		sA = 0xFF;
		sR = 0;
		sG = 0;
		sB = 0;
		fR = 0;
		fG = 0;
		fB = 0;

		for(int j = 0; j < nRoomDynamics; j++) {
			l = &RoomDynamics[j];

			lPos.x = vtx.x - r->posx - l->x;
			lPos.y = vtx.y - r->posy - l->y;
			lPos.z = vtx.z - r->posz - l->z;
			val = SQUARE(lPos.x) + SQUARE(lPos.y) + SQUARE(lPos.z);

			if(val < l->sqr_falloff) {
				val = (float)sqrt(val);
				val2 = l->inv_falloff * (l->falloff - val);
				lPos.x = (n.x * D3DMView._11 + n.y * D3DMView._21 + n.z * D3DMView._31) * (1.0F / val * lPos.x);
				lPos.y = (n.x * D3DMView._12 + n.y * D3DMView._22 + n.z * D3DMView._32) * (1.0F / val * lPos.y);
				lPos.z = (n.x * D3DMView._13 + n.y * D3DMView._23 + n.z * D3DMView._33) * (1.0F / val * lPos.z);
				val = val2 * (1.0F - (lPos.x + lPos.y + lPos.z));
				fR += l->r * val;
				fG += l->g * val;
				fB += l->b * val;
			}
		}

		cR += (long)(fR * 128.0F);
		cG += (long)(fG * 128.0F);
		cB += (long)(fB * 128.0F);

		if(i < r->nWaterVerts + r->nShoreVerts) {
			rndoff = (long)((vtx.x / 64.0F) + (vtx.y / 64.0F) + (vtx.z / 128.0F)) & 0xFC;

			rnd = WaterTable[r->MeshEffect][rndoff & 0x3C].random;
			shimmer = WaterTable[r->MeshEffect][((wibble >> 2) + rnd) & 0x3F].shimmer;
			abs = WaterTable[r->MeshEffect][((wibble >> 2) + rnd) & 0x3F].abs;
			col = (shimmer + abs) << 3;
			cR += col;
			cG += col;
			cB += col;
		}

		if(vPos.z > DistanceFogStart) {
			val = (vPos.z - DistanceFogStart) * num;

			if(gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6) {
				val = (vPos.z - DistanceFogStart) / 512.0F;
				sA -= (long)(val * (255.0F / 8.0F));

				if(sA < 0) {
					sA = 0;
				}
			} else {
				cR -= (long)val;
				cG -= (long)val;
				cB -= (long)val;
			}
		}

		if(cR - 128 <= 0) {
			cR <<= 1;
		} else {
			sR = (cR - 128) >> 1;
			cR = 255;
		}

		if(cG - 128 <= 0) {
			cG <<= 1;
		} else {
			sG = (cG - 128) >> 1;
			cG = 255;
		}

		if(cB - 128 <= 0) {
			cB <<= 1;
		} else {
			sB = (cB - 128) >> 1;
			cB = 255;
		}

		if(sR > 255) {
			sR = 255;
		} else if(sR < 0) {
			sR = 0;
		}
		if(sG > 255) {
			sG = 255;
		} else if(sG < 0) {
			sG = 0;
		}
		if(sB > 255) {
			sB = 255;
		} else if(sB < 0) {
			sB = 0;
		}
		if(cR > 255) {
			cR = 255;
		} else if(cR < 0) {
			cR = 0;
		}
		if(cG > 255) {
			cG = 255;
		} else if(cG < 0) {
			cG = 0;
		}
		if(cB > 255) {
			cB = 255;
		} else if(cB < 0) {
			cB = 0;
		}

		MyVertexBuffer[i].color = RGBA(cR, cG, cB, 0xFF);
		MyVertexBuffer[i].specular = RGBA(sR, sG, sB, sA);
	}
}

void CalcTriFaceNormal(_D3DVECTOR* p1, _D3DVECTOR* p2, _D3DVECTOR* p3, _D3DVECTOR* N) {
	FVECTOR u, v;

	u.x = p1->x - p2->x;
	u.y = p1->y - p2->y;
	u.z = p1->z - p2->z;
	v.x = p3->x - p2->x;
	v.y = p3->y - p2->y;
	v.z = p3->z - p2->z;
	N->x = v.z * u.y - v.y * u.z;
	N->y = v.x * u.z - v.z * u.x;
	N->z = v.y * u.x - v.x * u.y;
}

void CreateVertexNormals(ROOM_INFO* r, short* FaceData) {
	_D3DVECTOR p1;
	_D3DVECTOR p2;
	_D3DVECTOR p3;
	_D3DVECTOR n1;
	_D3DVECTOR n2;
	short* data;
	short nQuads;
	short nTris;

	data = FaceData;
	r->fnormals = (_D3DVECTOR*)calloc((r->gt3cnt + r->gt4cnt), sizeof(_D3DVECTOR));
	nQuads = *data++;

	for(int i = 0; i < nQuads; i++) {
		p1 = r->verts[data[0]];
		p2 = r->verts[data[1]];
		p3 = r->verts[data[2]];
		CalcTriFaceNormal(&p1, &p2, &p3, &n1);

		p1 = r->verts[data[0]];
		p2 = r->verts[data[2]];
		p3 = r->verts[data[3]];
		CalcTriFaceNormal(&p1, &p2, &p3, &n2);

		n1.x += n2.x;
		n1.y += n2.y;
		n1.z += n2.z;
		D3DNormalise(&n1);
		r->fnormals[i] = n1;
		data += 5;
	}

	nTris = *data++;

	for(int i = 0; i < nTris; i++) {
		p1 = r->verts[data[0]];
		p2 = r->verts[data[1]];
		p3 = r->verts[data[2]];
		CalcTriFaceNormal(&p1, &p2, &p3, &n1);
		D3DNormalise(&n1);
		r->fnormals[nQuads + i] = n1;
		data += 4;
	}

	r->vnormals = (_D3DVECTOR*)calloc(r->nVerts, sizeof(_D3DVECTOR));

	data = FaceData;
	nQuads = *data++;

	data += nQuads * 5;
	nTris = *data;

	for(int i = 0; i < r->nVerts; i++) {
		n1.x = 0;
		n1.y = 0;
		n1.z = 0;

		data = FaceData + 1;

		for(int j = 0; j < nQuads; j++) {
			if(data[0] == i || data[1] == i || data[2] == i || data[3] == i) {
				n1.x += r->fnormals[j].x;
				n1.y += r->fnormals[j].y;
				n1.z += r->fnormals[j].z;
			}

			data += 5;
		}

		data++;

		for(int j = 0; j < nTris; j++) {
			if(data[0] == i || data[1] == i || data[2] == i) {
				n1.x += r->fnormals[nQuads + j].x;
				n1.y += r->fnormals[nQuads + j].y;
				n1.z += r->fnormals[nQuads + j].z;
			}

			data += 4;
		}

		D3DNormalise(&n1);
		r->vnormals[i] = n1;
	}
}

void ProcessRoomData(ROOM_INFO* r, short* data) {
	D3DVERTEX* vptr;
	LIGHTINFO* light;
	PCLIGHT_INFO* pclight;
	FOGBULB_STRUCT* bulb;
	D3DVERTEXBUFFERDESC vb;
	short* data_ptr;
	short* faces;
	short* FaceData;
	short* prelight;
	float intensity;
	long nWaterVerts, nShoreVerts, nRestOfVerts, nLights, nBulbs;
	unsigned short cR, cG, cB;

	data_ptr = data;
	r->nVerts = *data_ptr++;

	if(!r->nVerts) {
		r->num_lights = 0;
		r->SourceVB = 0;
		return;
	}

	data_ptr += r->nVerts * 6;
	FaceData = data_ptr;
	r->gt4cnt = *data_ptr++;
	r->quads = (POLYFACE4*)calloc(r->gt4cnt, sizeof(POLYFACE4));
	data_ptr += r->gt4cnt * 5;
	r->gt3cnt = *data_ptr;
	r->tris = (POLYFACE3*)calloc(r->gt3cnt, sizeof(POLYFACE3));
	r->verts = (_D3DVECTOR*)calloc(r->nVerts, sizeof(_D3DVECTOR));
	faces = (short*)calloc(r->nVerts, sizeof(short));
	prelight = (short*)calloc(r->nVerts, sizeof(short));
	data_ptr = data + 1; // go to vert data
	nWaterVerts = 0;

	for(int i = 0; i < r->nVerts; i++) // get water verts
	{
		if(data_ptr[4] & 0x2000) {
			r->verts[nWaterVerts].x = (float)data_ptr[0];
			r->verts[nWaterVerts].y = (float)data_ptr[1];
			r->verts[nWaterVerts].z = (float)data_ptr[2];
			prelight[nWaterVerts] = data_ptr[5];
			faces[i] = (short)(nWaterVerts | 0x8000);
			nWaterVerts++;
		}

		data_ptr += 6;
	}

	data_ptr = data + 1;
	nShoreVerts = 0;

	for(int i = 0; i < r->nVerts; i++) // again for shore verts
	{
		if(data_ptr[4] & 0x4000 && !(data_ptr[4] & 0x2000)) {
			r->verts[nShoreVerts + nWaterVerts].x = (float)data_ptr[0];
			r->verts[nShoreVerts + nWaterVerts].y = (float)data_ptr[1];
			r->verts[nShoreVerts + nWaterVerts].z = (float)data_ptr[2];
			prelight[nShoreVerts + nWaterVerts] = data_ptr[5];
			faces[i] = (short)(nShoreVerts + nWaterVerts);
			nShoreVerts++;
		}

		data_ptr += 6;
	}

	data_ptr = data + 1;
	nRestOfVerts = 0;

	for(int i = 0; i < r->nVerts; i++) // one more for everything else
	{
		if(!(data_ptr[4] & 0x4000) && !(data_ptr[4] & 0x2000)) {
			r->verts[nRestOfVerts + nShoreVerts + nWaterVerts].x = (float)data_ptr[0];
			r->verts[nRestOfVerts + nShoreVerts + nWaterVerts].y = (float)data_ptr[1];
			r->verts[nRestOfVerts + nShoreVerts + nWaterVerts].z = (float)data_ptr[2];
			prelight[nRestOfVerts + nShoreVerts + nWaterVerts] = data_ptr[5];
			faces[i] = (short)(nRestOfVerts + nShoreVerts + nWaterVerts);
			nRestOfVerts++;
		}

		data_ptr += 6;
	}

	data_ptr = FaceData + 1;
	r->nWaterVerts = nWaterVerts;
	r->nShoreVerts = nShoreVerts;

	for(int i = 0; i < r->gt4cnt; data_ptr += 5, i++) // get quad data
	{
		if(faces[data_ptr[0]] & 0x8000 || faces[data_ptr[1]] & 0x8000 || faces[data_ptr[2]] & 0x8000 || faces[data_ptr[3]] & 0x8000) {
			r->quads->textInfo |= 0x4000;
		}

		r->quads[i].vertices[0] = faces[data_ptr[0]] & 0x7FFF;
		r->quads[i].vertices[1] = faces[data_ptr[1]] & 0x7FFF;
		r->quads[i].vertices[2] = faces[data_ptr[2]] & 0x7FFF;
		r->quads[i].vertices[3] = faces[data_ptr[3]] & 0x7FFF;
		r->quads[i].textInfo |= data_ptr[4];
	}

	data_ptr++; // skip over tri count

	for(int i = 0; i < r->gt3cnt; data_ptr+=4, i++) // tris
	{
		r->tris[i].vertices[0] = faces[data_ptr[0]] & 0x7FFF;
		r->tris[i].vertices[1] = faces[data_ptr[1]] & 0x7FFF;
		r->tris[i].vertices[2] = faces[data_ptr[2]] & 0x7FFF;
		r->tris[i].textInfo = data_ptr[3];
	}

	free(faces);
	CreateVertexNormals(r, FaceData);

	r->prelight = (long*)calloc(r->nVerts, sizeof(long));
	r->prelightwater = (long*)calloc(r->nVerts, sizeof(long));
	r->watercalc = 0;
	vb.dwNumVertices = r->nVerts;
	vb.dwSize = sizeof(D3DVERTEXBUFFERDESC);
	vb.dwCaps = 0;
	vb.dwFVF = D3DFVF_VERTEX;
	DXAttempt(IDirect3D3_CreateVertexBuffer(App.dx.lpD3D, &vb, &r->SourceVB, D3DDP_DONOTCLIP, 0));
	IDirect3DVertexBuffer_Lock(r->SourceVB, DDLOCK_WRITEONLY, (void**)&vptr, 0);
	r->posx = (float)r->x;
	r->posy = (float)r->y;
	r->posz = (float)r->z;
	data_ptr = data + 1;

	for(int i = 0; i < r->nVerts; i++) {
		vptr->x = r->verts[i].x + (float)r->x;
		vptr->y = r->verts[i].y + (float)r->y;
		vptr->z = r->verts[i].z + (float)r->z;
		vptr->nx = r->vnormals[i].x;
		vptr->ny = r->vnormals[i].y;
		vptr->nz = r->vnormals[i].z;
		cR = ((prelight[i] & 0x7C00) >> 10) << 3;
		cG = ((prelight[i] & 0x3E0) >> 5) << 3;
		cB = (prelight[i] & 0x1F) << 3;
		r->prelight[i] = RGBA(cR, cG, cB, 0xFF);
		cR = (unsigned short)((cR * water_color_R) >> 8);
		cG = (unsigned short)((cG * water_color_G) >> 8);
		cB = (unsigned short)((cB * water_color_B) >> 8);
		r->prelightwater[i] = RGBA(cR, cG, cB, 0xFF);
		vptr++;
		data_ptr += 6;
	}

	IDirect3DVertexBuffer_Unlock(r->SourceVB);
	free(prelight);
	r->pclight = NULL;

	if(r->num_lights) {
		r->pclight = (PCLIGHT_INFO*)calloc(r->num_lights, sizeof(PCLIGHT_INFO));
		nLights = 0;
		nBulbs = NumLevelFogBulbs;

		for(int i = 0; i < r->num_lights; i++) {
			light = &r->light[i];

			if(light->Type == LIGHT_FOG) {
				if(NumLevelFogBulbs >= 20) {
					Log(__func__, "Fog Bulb Discarded - More Than %d", 20);
					continue;
				}

				bulb = &FogBulbs[nBulbs];
				bulb->inRange = 0;
				bulb->density = light->r;
				bulb->WorldPos.x = (float)light->x;
				bulb->WorldPos.y = (float)light->y;
				bulb->WorldPos.z = (float)light->z;
				bulb->rad = light->Outer;
				bulb->sqrad = SQUARE(bulb->rad);
				bulb->inv_sqrad = 1 / bulb->sqrad;
				nBulbs++;
				NumLevelFogBulbs = nBulbs;
			} else {
				if(!light->r && !light->g && !light->b && light->Type == LIGHT_SPOT) {
					continue;
				}

				pclight = &r->pclight[nLights];
				pclight->r = light->r * (1.0F / 255.0F);
				pclight->g = light->g * (1.0F / 255.0F);
				pclight->b = light->b * (1.0F / 255.0F);
				intensity = r->light[nLights].Intensity;

				if(intensity < 0) {
					intensity = -intensity;
				}

				intensity *= 1.0F / 8191.0F;
				pclight->r *= intensity;
				pclight->g *= intensity;
				pclight->b *= intensity;

				if(r->light[nLights].Type) {
					pclight->shadow = (long)(intensity * 255);
				}

				pclight->x = (float)light->x;
				pclight->y = (float)light->y;
				pclight->z = (float)light->z;

				pclight->nx = -light->nx;
				pclight->ny = -light->ny;
				pclight->nz = -light->nz;

				pclight->Inner = light->Inner;
				pclight->Outer = light->Outer;
				pclight->InnerAngle = 2 * (float)acos(light->Inner);
				pclight->OuterAngle = 2 * (float)acos(light->Outer);

				if(r->light[nLights].Type == LIGHT_SPOT && pclight->OuterAngle > 3.1415927F) {
					Log(__func__, "SpotLight Corrected");
					pclight->OuterAngle = 3.1415927F;
				}

				pclight->Cutoff = light->Cutoff;
				pclight->Type = light->Type;
				nLights++;
			}
		}
	}

	IDirect3DVertexBuffer_Optimize(r->SourceVB, App.dx._lpD3DDevice, 0);
}

void S_InsertRoom(ROOM_INFO* r) {
	TEXTURESTRUCT* pTex;
	long doublesided;

	clip_left = r->left;
	clip_right = r->right;
	clip_bottom = r->bottom;
	clip_top = r->top;
	SetD3DViewMatrix();

	if(r->nVerts) {
		ProcessRoomDynamics(r);
		ProcessRoomVertices(r);

		for(int i = 0; i < r->gt4cnt; i++) {
			pTex = GetTextInfo(currentLevel, r->quads[i].textInfo & 0x3FFF);
			doublesided = (r->quads[i].textInfo >> 15) & 1;

			if(!pTex->drawtype) {
				AddQuadZBuffer(MyVertexBuffer, r->quads[i].vertices[0], r->quads[i].vertices[1], r->quads[i].vertices[2], r->quads[i].vertices[3], pTex, doublesided);
			} else if(pTex->drawtype <= 2) {
				AddQuadSorted(MyVertexBuffer, r->quads[i].vertices[0], r->quads[i].vertices[1], r->quads[i].vertices[2], r->quads[i].vertices[3], pTex, doublesided);
			}
		}


		for(int i = 0; i < r->gt3cnt; i++) {
			pTex = GetTextInfo(currentLevel, r->tris[i].textInfo & 0x3FFF);
			doublesided = (r->tris[i].textInfo >> 15) & 1;

			if(!pTex->drawtype) {
				AddTriZBuffer(MyVertexBuffer, r->tris[i].vertices[0], r->tris[i].vertices[1], r->tris[i].vertices[2], pTex, doublesided);
			} else if(pTex->drawtype <= 2) {
				AddTriSorted(MyVertexBuffer, r->tris[i].vertices[0], r->tris[i].vertices[1], r->tris[i].vertices[2], pTex, doublesided);
			}
		}
	}
}


void InitBuckets() {
	long oldBuckets = nBuckets;
	if(oldBuckets != GetNumTextures(currentLevel)) {
		nBuckets = GetNumTextures(currentLevel);
		Bucket = (TEXTUREBUCKET*)calloc(nBuckets, sizeof(TEXTUREBUCKET));
	}
	TEXTUREBUCKET* bucket;

	for(int i = 0; i < nBuckets; i++) {
		bucket = &Bucket[i];
		bucket->tpage = -1;
		bucket->nVtx = 0;
	}
}

void DrawBucket(TEXTUREBUCKET* bucket) {

	if(!bucket->nVtx) {
		return;
	}

	if(HasRendererBumpTexture(currentLevel, bucket->tpage) && App.BumpMapping) {
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGENABLE, 0);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, GetRendererBumpTexture(currentLevel, bucket->tpage)->dxTex));
		IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGENABLE, 1);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 1);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		DrawPrimitiveCnt++;
	}

	DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, GetRendererTexture(currentLevel, bucket->tpage)->dxTex));
	IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);

	if(App.BumpMapping) {
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
	}

	bucket->nVtx = 0;
	bucket->tpage = -1;
	DrawPrimitiveCnt++;
}

void FindBucket(long tpage, D3DTLBUMPVERTEX** Vpp, long** nVtxpp) {
	TEXTUREBUCKET* bucket;
	long nVtx, biggest;

	for(int i = 0; i < nBuckets; i++) {
		bucket = &Bucket[i];

		if(bucket->tpage == tpage && bucket->nVtx <= 2000) {
			*Vpp = &bucket->vtx[bucket->nVtx];
			*nVtxpp = &bucket->nVtx;
			return;
		}

		if(bucket->nVtx > 2000) {
			DrawBucket(bucket);
			bucket->tpage = tpage;
			bucket->nVtx = 0;
			*Vpp = bucket->vtx;
			*nVtxpp = &bucket->nVtx;
			return;
		}
	}

	nVtx = 0;
	biggest = 0;

	for(int i = 0; i < nBuckets; i++) {
		bucket = &Bucket[i];

		if(bucket->tpage == -1) {
			bucket->tpage = tpage;
			*Vpp = bucket->vtx;
			*nVtxpp = &bucket->nVtx;
			return;
		}

		if(bucket->nVtx > nVtx) {
			nVtx = bucket->nVtx;
			biggest = i;
		}
	}

	bucket = &Bucket[biggest];
	DrawBucket(bucket);
	bucket->tpage = tpage;
	bucket->nVtx = 0;
	*Vpp = bucket->vtx;
	*nVtxpp = &bucket->nVtx;
}

void DrawBuckets() {
	TEXTUREBUCKET* bucket;

	if(App.BumpMapping) {
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGENABLE, 0);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

		for(int i = 0; i < nBuckets; i++) {
			bucket = &Bucket[i];

			if(HasRendererBumpTexture(currentLevel, bucket->tpage) && bucket->nVtx) {
				DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, GetRendererBumpTexture(currentLevel, bucket->tpage)->dxTex));
				IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP);
				DrawPrimitiveCnt++;
			}
		}

		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGENABLE, 1);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 1);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		for(int i = 0; i < nBuckets; i++) {
			bucket = &Bucket[i];

			if(HasRendererBumpTexture(currentLevel, bucket->tpage) && bucket->nVtx) {
				DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, GetRendererTexture(currentLevel, bucket->tpage)->dxTex));
				IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
				bucket->nVtx = 0;
				bucket->tpage = -1;
				DrawPrimitiveCnt++;
			}
		}

		IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);

		for(int i = 0; i < nBuckets; i++) {
			bucket = &Bucket[i];

			if(!HasRendererBumpTexture(currentLevel, bucket->tpage) && bucket->nVtx) {
				DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, GetRendererTexture(currentLevel, bucket->tpage)->dxTex));
				IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
				bucket->nVtx = 0;
				bucket->tpage = -1;
				DrawPrimitiveCnt++;
			}
		}
	} else {
		for(int i = 0; i < nBuckets; i++) {
			bucket = &Bucket[i];
			DrawBucket(bucket);
		}
	}
}


void ProcessMesh(LEVEL_INFO* lvl, short* mesh_ptr, short* last_mesh_ptr, long i) {
	MESH_DATA* mesh;
	D3DVERTEXBUFFERDESC buf;
	D3DVERTEX* vtx;
	long lp;
	char c;
	if(mesh_ptr == last_mesh_ptr) {

		mesh = (MESH_DATA*)mesh_ptr;
		mesh_vtxbuf[i] = mesh;
	} else {
		last_mesh_ptr = mesh_ptr;
		mesh = (MESH_DATA*)calloc(1, sizeof(MESH_DATA));
		*GetMeshPointer(currentLevel, i) = (short*)mesh;
		mesh_vtxbuf[i] = mesh;
		mesh->x = mesh_ptr[0];
		mesh->y = mesh_ptr[1];
		mesh->z = mesh_ptr[2];
		mesh->r = mesh_ptr[3];
		mesh->flags = mesh_ptr[4];
		mesh->nVerts = mesh_ptr[5] & 0xFF;
		lp = 0;

		if(!mesh->nVerts) {
			lp = mesh_ptr[5] >> 8;
		}

		mesh_ptr += 6;

		if(mesh->nVerts) {
			buf.dwNumVertices = mesh->nVerts;
			buf.dwSize = sizeof(D3DVERTEXBUFFERDESC);
			buf.dwCaps = 0;
			buf.dwFVF = D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ;
			DXAttempt(IDirect3D3_CreateVertexBuffer(App.dx.lpD3D, &buf, &mesh->SourceVB, 0, 0));
			DXAttempt(IDirect3DVertexBuffer7_Lock(mesh->SourceVB, DDLOCK_WRITEONLY, (LPVOID*)&vtx, 0));
			IDirect3DVertexBuffer_Lock(mesh->SourceVB, DDLOCK_WRITEONLY, (LPVOID*)&vtx, 0);

			for(int j = 0; j < mesh->nVerts; j++) {
				vtx[j].x = mesh_ptr[0];
				vtx[j].y = mesh_ptr[1];
				vtx[j].z = mesh_ptr[2];
				mesh_ptr += 3;
			}

			mesh->nNorms = mesh_ptr[0];
			mesh_ptr++;

			if(!mesh->nNorms) {
				mesh->nNorms = mesh->nVerts;
			}

			if(mesh->nNorms > 0) {
				mesh->Normals = (_D3DVECTOR*)calloc(mesh->nNorms, sizeof(_D3DVECTOR));

				for(int j = 0; j < mesh->nVerts; j++) {
					vtx[j].nx = mesh_ptr[0];
					vtx[j].ny = mesh_ptr[1];
					vtx[j].nz = mesh_ptr[2];
					mesh_ptr += 3;
					D3DNormalise((_D3DVECTOR*)&vtx[j].nx);
					mesh->Normals[j].x = vtx[j].nx;
					mesh->Normals[j].y = vtx[j].ny;
					mesh->Normals[j].z = vtx[j].nz;
				}

				mesh->prelight = 0;
			} else {
				mesh->Normals = 0;
				mesh->prelight = (long*)calloc(mesh->nVerts, sizeof(long));

				for(int j = 0; j < mesh->nVerts; j++) {
					c = 255 - (mesh_ptr[0] >> 5);
					mesh->prelight[j] = RGBONLY(c, c, c);
					mesh_ptr++;
				}
			}

			IDirect3DVertexBuffer_Unlock(mesh->SourceVB);
		} else {
			mesh_ptr += 6 * lp + 1;
		}

		mesh->ngt4 = mesh_ptr[0];
		mesh_ptr++;

		if(mesh->ngt4) {
			mesh->gt4 = (POLYFACE4*)calloc(mesh->ngt4, sizeof(POLYFACE4));
			lp = 6 * mesh->ngt4;
			memcpy(mesh->gt4, mesh_ptr, sizeof(POLYFACE4) * mesh->ngt4);
			mesh_ptr += lp;
		}

		mesh->ngt3 = mesh_ptr[0];
		mesh_ptr++;

		if(mesh->ngt3) {
			mesh->gt3 = (POLYFACE3*)calloc(mesh->ngt3, sizeof(POLYFACE3));
			lp = 5 * mesh->ngt3;
			memcpy(mesh->gt3, mesh_ptr, sizeof(POLYFACE3) * mesh->ngt3);
		}
	}
}

unsigned short GetRandom(WATERTAB* wt, long lp) {
	long loop;
	unsigned short ret;

	do {
		ret = rand() & 0xFC;

		for(loop = 0; loop < lp; loop++) {
			if(wt[loop].random == ret) {
				break;
			}
		}

	} while(loop != lp);

	return ret;
}
void S_InitRoomDraw() {
	float fSin;
	short sSin;

	srand(121197);

	for(int i = 0; i < 64; i++) {
		sSin = rcossin_tbl[i << 7];
		WaterTable[0][i].shimmer = (63 * sSin) >> 15;
		WaterTable[0][i].choppy = (16 * sSin) >> 12;
		WaterTable[0][i].random = (unsigned char)GetRandom(&WaterTable[0][0], i);
		WaterTable[0][i].abs = 0;

		WaterTable[1][i].shimmer = (32 * sSin) >> 15;
		WaterTable[1][i].choppy = 0;
		WaterTable[1][i].random = (unsigned char)GetRandom(&WaterTable[1][0], i);
		WaterTable[1][i].abs = -3;

		WaterTable[2][i].shimmer = (64 * sSin) >> 15;
		WaterTable[2][i].choppy = 0;
		WaterTable[2][i].random = (unsigned char)GetRandom(&WaterTable[2][0], i);
		WaterTable[2][i].abs = 0;

		WaterTable[3][i].shimmer = (96 * sSin) >> 15;
		WaterTable[3][i].choppy = 0;
		WaterTable[3][i].random = (unsigned char)GetRandom(&WaterTable[3][0], i);
		WaterTable[3][i].abs = 4;

		WaterTable[4][i].shimmer = (127 * sSin) >> 15;
		WaterTable[4][i].choppy = 0;
		WaterTable[4][i].random = (unsigned char)GetRandom(&WaterTable[4][0], i);
		WaterTable[4][i].abs = 8;

		for(int j = 0, k = 5; j < 4; j++, k += 4) {
			for(int m = 0; m < 4; m++) {
				WaterTable[k + m][i].shimmer = -((sSin * water_shimmer[m]) >> 15);
				WaterTable[k + m][i].choppy = sSin * water_choppy[j] >> 12;
				WaterTable[k + m][i].random = (unsigned char)GetRandom(&WaterTable[k + m][0], i);
				WaterTable[k + m][i].abs = water_abs[m];
			}
		}
	}

	for(int i = 0; i < 32; i++) {
		fSin = sinf((float)(i * (M_PI / 16.0F)));
		vert_wibble_table[i] = fSin + fSin;
	}
}
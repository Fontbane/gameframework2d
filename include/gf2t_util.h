#ifndef __GF2T_UTIL_H__
#define __GF2T_UTIL_H__

#include <SDL.h>
#include "gfc_types.h"
#include "gfc_vector.h"

#define DIR_NORTH	0
#define DIR_WEST	1
#define DIR_SOUTH	2
#define DIR_EAST	3

typedef struct pointu8_s {
	Uint8 x;
	Uint8 y;
}PointU8;

#define POINTU8_EQ(a, b) a.x==b.x && a.y==b.y

typedef struct points8_s {
	Sint8 x;
	Sint8 y;
}PointS8;

PointU8 v2d_to_pu8(Vector2D v);
Bool f_gte_u8(float f, Uint8 u);
Bool f_lte_u8(float f, Uint8 u);
Bool f_eq_u8(float f, Uint8 u);

Vector2D tilepos_to_surfacepos(PointU8 p);

void stash_init();
void stash_data(char* key, void* data);
void* stash_check(char* key);
void stash_remove(char* key);

#endif
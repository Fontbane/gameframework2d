#include "gf2t_util.h"
#include "gfc_hashmap.h"

HashMap* stash;

void stash_init() {
	stash = gfc_hashmap_new();
}

void stash_data(char* key, void* data) {
	gfc_hashmap_insert(stash, key, data);
}

void* stash_check(char* key) {
	return gfc_hashmap_get(stash, key);
}

void stash_remove(char* key) {
	gfc_hashmap_delete_by_key(stash, key);
}

PointU8 v2d_to_pu8(Vector2D v) {
	PointU8 p = {
		(Uint8)v.x,
		(Uint8)v.y
	};
	return p;
}
Vector2D tilepos_to_surfacepos(PointU8 p) {
	Vector2D v = {
		.x = p.x * 16,
		.y = p.y * 16
	};
	return v;
}
Bool f_gte_u8(float f, Uint8 u) {
	return f >= (float)u;
}
Bool f_lte_u8(float f, Uint8 u) {
	return f <= (float)u;
}
Bool f_eq_u8(float f, Uint8 u) {
	return f == (float)u;
}
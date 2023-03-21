#include "simple_json.h"
#include "simple_logger.h"
#include "gf2d_tile.h"


Tileset* tileset_load(char* internalName) {
	Tileset* t;
	char configPath[80];
	char spritePath[80];
	FILE* tilemeta;
	if (t=stash_check(internalName)) {
		t->refcount++;
		return t;
	}
	t = gfc_allocate_array(sizeof(Tileset), 1);
	if (!t) {
		return NULL;
	}
	t->name = internalName;
	strcat(configPath, internalName);
	strcat(configPath, ".btmeta");

	strcat(spritePath, internalName);
	strcat(spritePath, ".png");

	tilemeta = fopen(configPath, "rb");
	if (!tilemeta) {
		slog("Unable to open config file for tileset %s", internalName);
		return NULL;
	}
	fread_s(&t->cols, 1, 1, 1, tilemeta);
	fread_s(&t->rows, 1, 1, 1, tilemeta);
	t->tiles = gfc_allocate_array(sizeof(TileMetadata), t->cols * t->rows);
	if (!t->tiles) {
		return NULL;
	}
	fread(t->tiles, sizeof(TileMetadata), t->cols * t->rows, tilemeta);
	fclose(configPath);

	t->tilesheet = gf2d_sprite_load_all(spritePath, 16, 16, t->cols, true);

	return t;
}
void tileset_free(Tileset* tileset) {
	stash_remove(tileset->name);
	gf2d_sprite_free(tileset->tilesheet);
	free(tileset->tiles);
	free(tileset);
}
MetatileInstance* gf2t_metatile_instance_new(Uint8 metatile_id, PointU8 position, Uint8 frames) {
	MetatileInstance* m;
	m = gfc_allocate_array(sizeof(MetatileInstance), 1);
	if (!m) {
		slog("Unable to allocate metatile instance");
		return m;
	}
	m->metatile_id = metatile_id;
	m->position = position;
	m->frames = frames;

	return m;
}
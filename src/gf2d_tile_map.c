#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_tile_map.h"


typedef struct mapmanager_s {
	Map* currentMap;
	List* chunks;
	Map otherMaps[4];
	Uint8 loadedTilesetID[4];
}MapManager;

static MapManager map_manager;

void gf2t_map_manager_init() {
	map_manager=*(MapManager*)gfc_allocate_array(sizeof(MapManager), 1);
}
Map* gf2t_map_new() {
	int i;
	for (i = 0; i < 4; i++) {
		if (!map_manager.otherMaps[i].info._inUse) {
			map_manager.otherMaps[i].info._inUse = i + 1;
			return &map_manager.otherMaps[i];
		}
	}
	slog("FUCK! NO MORE MAPS!");
	return NULL;
}
void gf2t_map_load(Map* map, char* internalName, Sint8 offset) {
	SJson* json;
	FILE* layout;
	int found_fields = 0;
	char configPath[80];
	char layoutPath[80];

	strcat(configPath, internalName);
	strcat(configPath, ".json");

	strcat(layoutPath, internalName);
	strcat(layoutPath, ".btf");

	json = sj_load(configPath);
	if (!json) {
		slog("Was unable to load map with name %s", internalName);
	}
	gfc_word_cpy(map->name,
		sj_object_get_value_as_string(json, "name"));
	found_fields += sj_get_bool_value(sj_object_get_value(json, "mapID"), &map->info.mapID);
	found_fields += sj_get_bool_value(sj_object_get_value(json, "height"), &map->info.height);
	found_fields += sj_get_bool_value(sj_object_get_value(json, "width"), &map->info.width);

	map->tilemap = gfc_allocate_array(sizeof(TilemapTile), map->info.width * map->info.height);
	map->layer2 = gfc_allocate_array(sizeof(TilemapTile), map->info.width * map->info.height);

	if (!map->layer2) {
		slog("Insufficient memory to load tile layout");
	}

	layout = fopen(layoutPath, "rb");
	if (!layout) {
		slog("Unable to open tile layout for %s", internalName);
	}

	fread(map->tilemap, sizeof(TilemapTile), map->info.height * map->info.width, layout);
	fread(map->layer2, sizeof(TilemapTile), map->info.height * map->info.width, layout);
	fclose(layoutPath);

	map->tileset=tileset_load(sj_object_get_value_as_string(json, "tileset"));

	map->info.indoors = sj_object_get_value(json, "indoors") != NULL;
	map->info.showName = sj_object_get_value(json, "showName") != NULL;

	map->drawbuff = gf2d_graphics_create_surface(map->info.width * 16, map->info.height * 16);

}

void gf2t_map_free(Map* map) {
	tileset_free(map->tileset);
	free(map->tilemap);
	free(map->layer2);
	SDL_FreeSurface(map->drawbuff);
	free(map);
}

Chunk* gf2t_chunk_new(Map* map, PointU8 boundsMin, PointU8 boundsMax) {
	int x, y, i;
	Chunk* chunk = gfc_allocate_array(sizeof(Chunk), 1);
	chunk->map = map;
	chunk->boundsMax = boundsMax;
	chunk->id = boundsMin.y / 16 * map->info.width / 16 + boundsMin.x / 16;
	chunk->ents = gfc_list_new_size(CHUNK_MAX_ENTS);
	for (i= y = 0; y < boundsMax.y-boundsMin.y; y++) {
		for (x = 0; x < boundsMax.x-boundsMin.x; x++, i++) {
			chunk->tiles[i].configData = &map->tilemap[(boundsMin.y + y)*map->info.width+ boundsMin.x + x];
			chunk->tiles[i].metatile = 
				gf2t_metatile_instance_new(chunk->tiles[i].configData->metatile_id,
					(PointU8){ boundsMin.x + x, boundsMin.y + y }, map->tileset->tiles[chunk->tiles[i].configData->tileID].frames);
		}
	}
	gfc_list_append(map_manager.chunks, chunk);
	return chunk;
}
MetatileInstance* metatile_instance_new(Uint8 metatile_id, PointU8 position) {
	MetatileInstance* m = gfc_allocate_array(sizeof(MetatileInstance), 1);
	if (!m) {
		slog("OH GOD OH FUCK COULDN'T INSTANTIATE METATILE WITH ID %d", metatile_id);
		return NULL;
	}
	m->metatile_id = metatile_id;
	m->position = position;
	return m;
}

void gf2t_chunk_free(Chunk* chunk) {
	gf2t_clear_chunk_entities(chunk);
	gfc_list_delete_data(map_manager.chunks, chunk);
	free(chunk);
}
void gf2t_chunk_add_ent(Chunk* chunk, Entity* ent) {
	gfc_list_append(chunk->ents, ent);
}
void gf2t_clear_chunk_entities(Chunk* chunk) {
	gfc_list_foreach(chunk->ents, ent_free);
}

TilemapTileActive* gf2t_get_tile_at(Chunk* chunk, PointU8 pos) {
	return &chunk->tiles[
		(chunk->boundsMax.y - pos.y) * 16 + (chunk->boundsMax.x - pos.x)
	];
}

void gf2t_map_draw_to_buffer(Map* map) {
	int i;
	for (i = 0; i < map->info.width * map->info.height; i++) {
		gf2d_sprite_draw_to_surface(
			map->tileset->tilesheet, tilepos_to_surfacepos((PointU8) { i% map->info.width, i / map->info.width }),
			NULL, NULL, map->tilemap[i].tileID, map->drawbuff
		);
	}
}

void gf2t_chunk_draw_all(Chunk* chunk) {
	int i;
	Uint8 x, y;
	TilemapTileActive currTile;
	//Draw bottom layer of tiles (only the updated ones)
	for (i = 0; i < 256; i++) {
		currTile = chunk->tiles[i];
		if (currTile.metatile->frames>0 && 
			currTile.metatile->frame==floorf(currTile.metatile->frame)) {
			gf2d_sprite_draw_to_surface(
				chunk->map->tileset->tilesheet, tilepos_to_surfacepos(currTile.metatile->position),
				NULL, NULL, currTile.configData->tileID + (Uint8)currTile.metatile->state, chunk->map->drawbuff
			);
		}
	}
	//Draw entity layer, from bottom-right position to top-left
	ent_draw_all_in_chunk(chunk);

	//Draw layer of tiles that goes above entities
	for (i = y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++, i++) {
			if (chunk->layer2[i] > 0) {
				gf2d_sprite_draw(chunk->map->tileset->tilesheet, tilepos_to_surfacepos((PointU8) { x, y }),
					NULL, NULL, NULL, NULL, NULL, 0);
			}
		}
	}
}
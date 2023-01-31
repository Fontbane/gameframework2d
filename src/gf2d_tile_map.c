#include "gf2d_tile_map.h"


typedef struct mapmanager_s {
	Map* currentMap;
	List* chunks;
}MapManager;



void gf2t_map_manager_init();
Map* gf2t_map_new();
void gf2t_map_load(Map* map, Uint16 mapID, Sint8 offset);
void gf2t_map_free(Map* map);

Chunk* gf2t_chunk_new(Map* map, PointU8 boundsMin, PointU8 boundsMax);
void gf2t_chunk_free(Chunk* chunk);
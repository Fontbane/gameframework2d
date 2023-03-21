#ifndef __GF2D_TILE_MAP_H__
#define __GF2D_TILE_MAP_H__

#include "gf2d_tile.h"

/*Defines a connection to another map*/
typedef struct mapconnection_s {
	Uint16 mapID;		/*Map connected to*/
	Uint8 direction : 2;	/*Which direction the connected map is in relation to the original map*/
	Uint8 padding : 6;
	Sint8 offset;		/*How much to offset the connected map from the edge of the original*/
}MapConnection;

#define MAX_MAP_CONNECTIONS 6

typedef struct mapdata_s {
	Uint16 mapID;

	//Flags
	Uint16 flags : 13;
	Uint16 showName : 1; //Whether name should be displayed upon entering map
	Uint16 indoors : 1;
	Uint8 _inUse:1;		//this shouldn't be here but it is lol

	Uint8 width;
	Uint8 height;

	MapConnection connections[MAX_MAP_CONNECTIONS];
}MapConfigData;

typedef struct map_s {
	MapConfigData info;
	TextWord name;
	TextWord internalName;
	Tileset* tileset;
	TilemapTile* tilemap;	/*Pointer to first tile in tilemap*/
	Uint16* layer2;
	SDL_Surface* drawbuff;
}Map;

typedef struct chunk_s {
	Map* map;
	TilemapTileActive tiles[256];
	Uint16 layer2[256];
	Uint8 entMatrix[256];
	PointU8 boundsMax;	/*Position of top left and bottom right tiles within the map*/
	Uint16 firstEnt;
	Uint8 id;
	List* ents;
}Chunk;

#define MAX_MAP_SIZE	SDL_MAX_UINT8
#define MAX_MAPS	SDL_MAX_UINT16

#define CHUNK_MAX_ENTS	64

#include "gf2t_entity.h"

void gf2t_map_manager_init();
Map* gf2t_map_new();
void gf2t_map_load(Map* map, char* internalName, Sint8 offset);
void gf2t_map_free(Map* map);

void gf2t_map_draw_to_buffer(Map* map);
void gf2t_map_update(Map* map);

Chunk* gf2t_chunk_new(Map* map, PointU8 boundsMin, PointU8 boundsMax);
void gf2t_chunk_add_ent(Chunk* chunk, Entity* ent);
void gf2t_clear_chunk_entities(Chunk* chunk);
void gf2t_chunk_free(Chunk* chunk);

TilemapTileActive* gf2t_get_tile_at(Chunk* chunk, PointU8 pos);

void gf2t_chunk_draw_all(Chunk* chunk);

#endif

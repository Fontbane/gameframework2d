#ifndef __GF2D_TILE_MAP_H__
#define __GF2D_TILE_MAP_H__

#include "gf2d_tile.h"

#define DIR_NORTH	=	0
#define DIR_WEST	=	1
#define DIR_SOUTH	=	2
#define DIR_EAST	=	3

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
	Uint8 width;
	Uint8 height;
	Uint32 flags;
	MapConnection connections[MAX_MAP_CONNECTIONS];
}MapConfigData;

typedef struct mapstrings_s {
	TextWord name;			//Name shown to player
	TextWord internalName;	//Name used for file path
}MapStrings;

typedef struct map_s {
	MapConfigData info;
	MapStrings* str;
	TilemapTile* tilemap;	/*Pointer to first tile in tilemap*/
	List* chunks;
}Map;

typedef struct chunk_s {
	Map* map;
	TilemapTileActive* tiles;
	PointU8 boundsMin, boundsMax;	/*Position of top left and bottom right tiles within the map*/
	List* ents;
}Chunk;

#define MAX_MAPS	SDL_UINT16_MAX

void gf2t_map_manager_init();
Map* gf2t_map_new();
void gf2t_map_load(Map* map, Uint16 mapID, Sint8 offset);
void gf2t_map_free(Map* map);

Chunk* gf2t_chunk_new(Map* map, PointU8 boundsMin, PointU8 boundsMax);
void gf2t_clear_chunk_entities(Chunk* chunk);
void gf2t_chunk_free(Chunk* chunk);

#endif

#ifndef __GF2D_TILE_H__
#define __GF2D_TILE_H__

#include "gf2d_draw.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "gf2t_util.h"

/*!!Do not increase the following values!!*/
#define MAX_TILES_IN_SHEET	SDL_MAX_UINT16
#define MAX_TILES_IN_ROW	SDL_MAX_UINT8
#define MAX_METATILES		SDL_MAX_UINT8
/*!!Do not increase the above values!!*/

#define TILE_SIZE 16

enum {
	MT_NORMAL,
	MT_ICE
};

/*The data stored for each tile in a tileset.*/
typedef struct tile_s {
	Uint16 tileID;		/*Frame of the tileset containing the tile*/
	Uint8 metatile_id;	/*The id of the metatile with its behavior. 0 is normal.*/
	Uint8 frames : 4;		/*Number of extra frames of animation this tile has. 0 for none.*/
	Uint8 framenum : 4;		/*The frame of animation this tile is*/
}TileMetadata;

/*An instance of a metatile. Can store a state and which frame of animation it's in.*/
typedef struct metatile_instance_s {
	PointU8 position;
	Uint8 metatile_id;
	Uint8 frames;

	Uint32 occupied : 1;
	Uint32 state:31;/*timers etc*/
	float frame;/*for animated tiles*/
}MetatileInstance;

/*Config data for each tile in a tilemap. When you draw a map in an editor, each tile is this.*/
typedef struct tilemap_tile_s {
	Uint16 tileID;

	Uint8 collisionType : 4;	/*User can have up to 16 elevations or collision types*/
	Uint8 pass_from_west : 1;
	Uint8 pass_from_east : 1;
	Uint8 pass_from_north : 1;
	Uint8 pass_from_south : 1;
	Uint8 metatile_id;
}TilemapTile;

#define CHUNK_SIZE	16	//The height and width of a chunk of tiles.

/*An active tilemap tile. Up to 4 chunks of these can be stored, the rest are flushed.*/
/*A TilemapTileActive will NOT be flushed if its metatile has had its state changed.*/
/*Metatiles will only stay if their state has changed*/
typedef struct active_tilemap_tile_s {
	TilemapTile* configData;
	MetatileInstance* metatile;
}TilemapTileActive;

typedef struct {
	char* name;
	Uint8 rows;
	Uint8 cols;
	Uint8 refcount;
	Sprite* tilesheet;
	TileMetadata* tiles;
} Tileset;

Tileset* tileset_load(char* internalName);
void tileset_free(Tileset* tileset);
MetatileInstance* gf2t_metatile_instance_new(Uint8 metatile_id, PointU8 position, Uint8 frames);

#endif

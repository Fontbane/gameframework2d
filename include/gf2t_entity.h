#ifndef __GF2T_ENTITY_H__
#define __GF2T_ENTITY_H__

#include "gfc_types.h"
#include "gf2d_sprite.h"
#include "gf2d_tile_map.h"

typedef struct move_q {
	MoveQ* next;		//Next movement in the queue
	MoveQ* prev;
	PointS8 movement;	//Current movement instructions
	Uint8 turn;			//Change where the entity is facing, to one of four DIR directions
}MoveQ;

typedef struct entconfig_s{
	Uint16 id;
	Uint16 script_id;			//Script to execute
	Uint32 script_parameters;

	PointU8 origin;
	Uint8 speed; //1: slow, 2: normal, 4: fast
	Uint8 facing : 2; //Use the DIR definitions from tile map.h
	Uint8 player : 1; //Entities with this flag can load new maps and chunks
	Uint8 mobile : 1; //Whether it can move
	Uint8 ignoreCollision : 1;
	Uint8 destroyOnChunkUnload : 1; //If set, clear all but config data
	Uint8 unused_flags : 2;

	Uint16 sprite_id;
	Uint8 movement_type;
	Uint8 entity_type;

}EntityConfig;

typedef struct edict_s{
	Uint16 id;
	PointU8 tilePosition;

	Uint8 _inuse: 1;
	Uint8 facing : 2;
	Uint8 unused_flags : 5;
	Uint8 collisionLayer;
	PointU8 targetPosition;

	Sprite* sprite;

	EntityConfig* config;

	MoveQ* moveq;

	void (*think)(edict_s* self);
	void (*update)(edict_s* self);
	void (*draw)(edict_s* self);
	void (*onCollision)(edict_s* self, edict_s* other);
}Entity;

void ent_manager_init(Uint16 max);
Entity* ent_new();
void ent_pull_from_config(Entity* ent, char* name);
void ent_clear(Entity* ent);
void ent_free(Entity* ent);
void ent_think(Entity* self);
void ent_update(Entity* self);
void ent_draw(Entity* self);
void ent_think_all();
void ent_update_all();
void ent_draw_all();
void ent_manager_close();

Bool ent_move(Entity* ent);
Bool ent_stop(Entity* ent);
void lockall();
void unlockall();

void move_up(Entity* ent);
void move_down(Entity* ent);
void move_left(Entity* ent);
void move_right(Entity* ent);
PointS8 moveq_pop(MoveQ* move);
MoveQ* moveq_get_last(MoveQ* move);
MoveQ* moveq_next(Entity* ent);
MoveQ* moveq_prev(Entity* ent);
MoveQ* moveq_push(MoveQ* move);

#endif
#ifndef __GF2T_ENTITY_H__
#define __GF2T_ENTITY_H__

#include "gfc_types.h"
#include "gf2d_sprite.h"
#include "gf2d_tile_map.h"

typedef struct move_q {
	struct move_q* next;		//Next movement in the queue
	struct move_q* prev;
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
	Uint8 loop_movement : 1;
	Uint8 unused : 1;

	Uint16 sprite_id;
	Uint8 movement_type;
	Uint8 entity_type;

}EntityConfig;

typedef struct edict_s{
	Uint16 id;
	PointU8 tilePosition;

	Uint8 _inuse: 1;
	Uint8 facing : 2;
	Uint8 moveDir : 2;
	Uint8 moving : 1;
	Uint8 sliding : 1;
	Uint8 waiting : 1;
	Uint8 collisionLayer;
	PointU8 targetPosition;

	Vector2D worldPosition;
	Vector2D drawPosition;
	float frame;

	Sprite* sprite;

	EntityConfig* config;

	Chunk* currChunk;

	MoveQ* moveq;
	MoveQ* moveq_tail;

	Uint32 timer;

	void (*think)(struct edict_s* self);
	void (*update)(struct edict_s* self);
	void (*draw)(struct edict_s* self);
	void (*onCollision)(struct edict_s* self, struct edict_s* other);
}Entity;

#define SPRITE_MOVE_INTERVAL 0.03125f
#define TURN_90 DIR_EAST+1
#define TURN_180 DIR_EAST+2
#define TURN_270 DIR_EAST+3
#define NO_TURN 255

void ent_manager_init(Uint32 max);
Entity* ent_new();
void ent_pull_from_config(Entity* ent, char* name);
void ent_free(Entity* ent);
void ent_think(Entity* self);
void ent_update(Entity* self);
void ent_draw(Entity* self);
void ent_think_all();
void ent_update_all();
void ent_draw_all();
void ent_draw_all_in_chunk(Chunk* chunk);
void ent_manager_close();

Bool ent_start_movement(Entity* ent);
Bool ent_move(Entity* ent);
Bool ent_stop(Entity* ent);
Bool ent_check_collision_at(Entity* ent, PointU8 position);
void lockall();
void unlockall();

void move_up(Entity* ent);
void move_down(Entity* ent);
void move_left(Entity* ent);
void move_right(Entity* ent);
void move_wait(Entity* ent);
void move_turn_90(Entity* ent);
void move_turn_180(Entity* ent);
void move_turn_270(Entity* ent);
void move_turn_north(Entity* ent);
void move_turn_west(Entity* ent);
void move_turn_south(Entity* ent);
void move_turn_east(Entity* ent);

void move_up_now(Entity* ent);
void move_down_now(Entity* ent);
void move_left_now(Entity* ent);
void move_right_now(Entity* ent);
void move_wait_now(Entity* ent);
void move_turn_90_now(Entity* ent);
void move_turn_180_now(Entity* ent);
void move_turn_270_now(Entity* ent);
void move_turn_north_now(Entity* ent);
void move_turn_west_now(Entity* ent);
void move_turn_south_now(Entity* ent);
void move_turn_east_now(Entity* ent);

int moveq_remove(Entity* ent);
void moveq_free(Entity* ent);

#endif
#include "simple_logger.h"

#include "gf2d_tile_map.h"
#include "gf2t_entity.h"

typedef struct {
	Uint32 maxEnts;
	Uint8 _lockall;
	Entity* entity_list;
}EntityManager;

EntityManager ent_manager;

const Uint8 gWalkAnimFrames[] = { 0, 4, 8, 12, 16 };
const Uint8 gTurn90[] = { DIR_EAST, DIR_NORTH, DIR_WEST, DIR_SOUTH };
const Uint8 gTurn180[] = { DIR_SOUTH, DIR_EAST, DIR_NORTH, DIR_WEST };
const Uint8 gTurn270[] = { DIR_WEST, DIR_SOUTH, DIR_EAST, DIR_NORTH };

void ent_manager_init(Uint32 max) {
	if (max > SDL_MAX_UINT16) max = SDL_MAX_UINT16;
	ent_manager.maxEnts = max;
	ent_manager.entity_list = gfc_allocate_array(sizeof(Entity), max);
	if (!ent_manager.entity_list) {
		slog("Unable to initialize entity system!");
	}
	else {
		slog("Entity system initialized");
	}
}

Entity* ent_new() {
	int i;
	for (i = 0; i < ent_manager.maxEnts; i++) {
		if (!ent_manager.entity_list[i]._inuse) {
			ent_free(&ent_manager.entity_list[i]);
			ent_manager.entity_list[i].think = ent_think;
			ent_manager.entity_list[i].update = ent_update;
			ent_manager.entity_list[i].draw = ent_draw;
			return &ent_manager.entity_list[i];
		}
	}
	slog("Entity list is FULL!");
	return NULL;
}

void ent_pull_from_config(Entity* ent, char* name) {
	//TODO
}

void ent_draw_all_in_chunk(Chunk* chunk) {
	int i;
	for (i = 0; i < 256; i++) {
		if (chunk->entMatrix[i] > 0) {
			ent_draw(&ent_manager.entity_list[chunk->firstEnt + i]);
		}
	}
}

void ent_free(Entity* ent) {
	moveq_free(ent);
	memset(ent, 0, sizeof(Entity*));
}

void ent_think(Entity* self) {
	if (self->think) {
		self->think(self);
	}
}
void ent_update(Entity* self) {
	if (self->update) {
		self->update(self);
	}
	if (!ent_manager._lockall && self->moveq && !self->moving && (!self->waiting || self->waiting && SDL_GetTicks()>=self->timer)) {
		self->moving=ent_start_movement(self);
	}
	else if (!ent_manager._lockall && self->moving) {
		ent_move(self);
	}
}
void ent_draw(Entity* self) {
	if (self->draw) {
		self->draw(self);
	}
	else {
		gf2d_sprite_draw(self->sprite, self->drawPosition, NULL, NULL, NULL, NULL, NULL, self->frame);
	}
}
void ent_think_all() {
	int i;
	for (i = 0; i < ent_manager.maxEnts; i++) {
		if (ent_manager.entity_list[i]._inuse) {
			ent_think(&ent_manager.entity_list[i]);
		}
	}
}
void ent_update_all() {
	int i;
	for (i = 0; i < ent_manager.maxEnts; i++) {
		if (ent_manager.entity_list[i]._inuse) {
			ent_update(&ent_manager.entity_list[i]);
		}
	}
}
void ent_draw_all() {
	int i;
	for (i = 0; i < ent_manager.maxEnts; i++) {
		if (ent_manager.entity_list[i]._inuse) {
			ent_draw(&ent_manager.entity_list[i]);
		}
	}
}
void ent_manager_close() {
	int i;
	for (i = 0; i < ent_manager.maxEnts; i++) {
		if (ent_manager.entity_list[i]._inuse) {
			ent_free(&ent_manager.entity_list[i]);
		}
	}
	slog("Entity system closed");
}

Bool ent_start_movement(Entity* ent) {
	TilemapTileActive *startTile, *targetTile;
	if (!ent->config) {
		return 0;
	}
	else {
		switch (ent->moveq->turn) {
		case TURN_270:
			ent->facing = gTurn270[ent->facing];
			break;
		case TURN_180:
			ent->facing = gTurn180[ent->facing];
			break;
		case TURN_90:
			ent->facing = gTurn90[ent->facing];
			break;
		case NO_TURN:
			break;
		default:
			ent->facing = ent->moveq->turn;
		}
		if (ent->moveq->movement.x == ent->moveq->movement.y && ent->moveq->movement.x== 0) {
			ent->moving = ent->sliding = 0;
			ent->timer = SDL_GetTicks() + 1000;
			return ent->waiting = 1;
		}
		startTile = gf2t_get_tile_at(ent->currChunk, ent->tilePosition);
		PointU8 possibleTarget = {
			.x = ent->targetPosition.x + ent->moveq->movement.x,
			.y = ent->targetPosition.y + ent->moveq->movement.y
		};
		targetTile = gf2t_get_tile_at(ent->currChunk, possibleTarget);
		if (startTile->metatile->metatile_id == MT_ICE || targetTile->metatile->metatile_id == MT_ICE) {
			ent->sliding = 1;
		}
		if (possibleTarget.x < MAX_MAP_SIZE && possibleTarget.y < MAX_MAP_SIZE && ent_check_collision_at(ent,possibleTarget)) {
			moveq_remove(ent);
			ent->targetPosition = possibleTarget;
			return 1;
		}
		else {
			return ent->moving = ent->sliding = 0;
		}
	}
}
Bool ent_move(Entity* ent) {
	Bool stop = 0;
	float moveInterval;
	if (!ent->config || POINTU8_EQ(ent->tilePosition, ent->targetPosition)) {
		return ent->moving=0;
	}
	if (ent->sprite->frames_per_line * 2.0f == 0) {
		moveInterval = 0;
	}
	else {
		moveInterval = ent->config->speed / (ent->sprite->frames_per_line * 2.0f);
	}
	switch (ent->moveDir) {
	case DIR_NORTH:
		ent->worldPosition.y -= moveInterval;
		if (f_lte_u8(ent->worldPosition.y,ent->targetPosition.y)) {
			ent->tilePosition = ent->targetPosition;
			ent->moving = 0;
			if (ent->sliding) move_up_now(ent);
		}
		break;
	case DIR_WEST:
		ent->worldPosition.x -= moveInterval;
		if (f_lte_u8(ent->worldPosition.x, ent->targetPosition.x)) {
			ent->tilePosition = ent->targetPosition;
			ent->moving = 0;
			if (ent->sliding) move_left_now(ent);
		}
		break;
	case DIR_SOUTH:
		ent->worldPosition.y += moveInterval;
		if (f_gte_u8(ent->worldPosition.y, ent->targetPosition.y)) {
			ent->tilePosition = ent->targetPosition;
			ent->moving = 0;
			if (ent->sliding) move_down_now(ent);
		}
		break;
	case DIR_EAST:
		ent->worldPosition.x += moveInterval;
		if (f_gte_u8(ent->worldPosition.x, ent->targetPosition.x)) {
			ent->tilePosition = ent->targetPosition;
			ent->moving = 0;
			if (ent->sliding) move_right_now(ent);
		}
		break;
	}
	if (ent->facing > DIR_EAST) {
		ent->facing = DIR_NORTH;
	}
	if (!ent->sliding) {
		if (ent->frame + moveInterval < gWalkAnimFrames[ent->facing + 1]) {
			ent->frame = gWalkAnimFrames[ent->facing];
		}
		else {
			ent->frame += moveInterval;
		}
	}
	return ent->moving;
}
Bool ent_stop(Entity* ent) {
	ent->moving = 0;
	return 1;
}
Bool ent_check_collision_at(Entity* ent, PointU8 position) {

}
void lockall() {
	ent_manager._lockall = 1;
}
void unlockall() {
	ent_manager._lockall = 0;
}
void move_append(Entity* ent, Sint8 x, Sint8 y, Uint8 dir) {
	MoveQ q = {
		.movement = {x, y},
		.next = NULL,
		.prev = ent->moveq_tail,
		.turn = dir
	};
	ent->moveq_tail = &q;
	ent->moveq_tail->prev->next = ent->moveq_tail;
}
void move_up(Entity* ent) {
	move_append(ent, 0, -1, DIR_NORTH);
}
void move_down(Entity* ent) {
	move_append(ent, 0, 1, DIR_SOUTH);
}
void move_left(Entity* ent) {
	move_append(ent, -1, 0, DIR_WEST);
}
void move_right(Entity* ent) {
	move_append(ent, 1, 0, DIR_EAST);
}
void move_wait(Entity* ent) {
	move_append(ent, 0, 0, NO_TURN);
}
void move_turn_90(Entity* ent){
	move_append(ent, 0, 0, TURN_90);
}
void move_turn_180(Entity* ent) {
	move_append(ent, 0, 0, TURN_180);
}
void move_turn_270(Entity* ent) {
	move_append(ent, 0, 0, TURN_270);
}
void move_turn_north(Entity* ent) {
	move_append(ent, 0, 0, DIR_NORTH);
}
void move_turn_west(Entity* ent) {
	move_append(ent, 0, 0, DIR_WEST);
}
void move_turn_south(Entity* ent) {
	move_append(ent, 0, 0, DIR_SOUTH);
}
void move_turn_east(Entity* ent) {
	move_append(ent, 0, 0, DIR_EAST);
}

void move_now(Entity* ent, Sint8 x, Sint8 y, Uint8 dir) {
	MoveQ q = {
		.movement = {x, y},
		.next = ent->moveq,
		.prev = NULL,
		.turn = dir
	};
	ent->moveq = &q;
}
void move_up_now(Entity* ent) {
	move_now(ent, 0, -1, DIR_NORTH);
}
void move_down_now(Entity* ent) {
	move_now(ent, 0, 1, DIR_SOUTH);
}
void move_left_now(Entity* ent){
	move_now(ent, -1, 0, DIR_WEST);
}
void move_right_now(Entity* ent){
	move_now(ent, 1, 0, DIR_EAST);
}
void move_wait_now(Entity* ent){
	move_now(ent, 0, 0, NO_TURN);
}
void move_turn_90_now(Entity* ent) {
	move_now(ent, 0, 0, TURN_90);
}
void move_turn_180_now(Entity* ent) {
	move_now(ent, 0, 0, TURN_180);
}
void move_turn_270_now(Entity* ent) {
	move_now(ent, 0, 0, TURN_270);
}
void move_turn_north_now(Entity* ent) {
	move_now(ent, 0, 0, DIR_NORTH);
}
void move_turn_west_now(Entity* ent) {
	move_now(ent, 0, 0, DIR_WEST);
}
void move_turn_south_now(Entity* ent) {
	move_now(ent, 0, 0, DIR_SOUTH);
}
void move_turn_east_now(Entity* ent) {
	move_now(ent, 0, 0, DIR_EAST);
}
int moveq_remove(Entity* ent) {
	MoveQ* old = ent->moveq;
	if (!ent->moveq) {
		return 0;
	}
	if (ent->config->loop_movement && ent->moveq->prev && ent->moveq->next) {
		ent->moveq->prev->next = ent->moveq->next;
		ent->moveq->next->prev = ent->moveq->prev;
	}
	ent->moveq = ent->moveq->next;
	free(old);
	return 1;
}
void moveq_free(Entity* ent) {
	while (moveq_remove(ent));
}
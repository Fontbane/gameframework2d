#ifndef __GF2T_ASSET_MANAGER_H__
#define __GF2T_ASSET_MANAGER_H__

#include "gf2t_util.h"
#include "gfc_text.h"

#define NUM_ITEMS			256
#define NUM_MAPS			256
#define NUM_IMAGES			512
#define NUM_SOUNDS			128
#define NUM_GENERICS		128
#define NUM_CFG				256
#define NUM_TEXTS			512

#define CFGS_CFG			0
#define CFGS_MAPS			(CFGS_CFG+NUM_CFG)
#define CFGS_IMAGES			(CFGS_MAPS+NUM_MAPS)
#define CFGS_SOUNDS			(CFGS_IMAGES+NUM_IMAGES)
#define CFGS_GENERICS		(CFGS_SOUNDS+NUM_SOUNDS)
#define CFGS_ITEMS			(CFGS_GENERICS+NUM_GENERICS)
#define CFGS_TEXTS			(CFGS_ITEMS+NUM_ITEMS)

#define MAX_CONFIG_STRINGS 2048

TextLine gConfigStrings[MAX_CONFIG_STRINGS];

typedef enum {
	ASSTYPE_GENERIC,
	ASSTYPE_TEXT,
	ASSTYPE_GFX,
	ASSTYPE_SFX,
	ASSTYPE_BGM,
	ASSTYPE_CFG,
	ASSTYPE_MAP,
	ASSTYPE_ASSCFG,
	ASSTYPE_MAX
}AssetType;

typedef struct asset_s {
	TextLine path;
	Uint16 id;
	Uint16 refcount;

	AssetType whatkindofthingisthis;
	void* thingusedwhere;
}Asset;

void read_in_configstrings();

#endif

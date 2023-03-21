# GF2D tile (GF2T)
An extension for GF2D that adds tile based maps and movement, as well as a simple entity system interacting with it.

# Features

GF2T offers a few features to make it easier to make tile based RPGs
 - Tile behaviors which can trigger scripts (up to 256 metatile behaviors)
 - Animated tiles
 - Entities with tile based movement and a scripting system for movement
 - Tile based maps loaded from binary files
 - Robust collision system supporting multiple elevations

# Creating a new Tileset
A tileset in this framework consists of two files: the tilesheet (png), and the meta file (btmeta). The meta file is simply a binary file with 4 bytes for each tile in the tilesheet, in order from left to right and top to bottom. An editor is planned but any hex editor can be used for now.
The structure of a tile in a btmeta file is as follows:
- 2 bytes for tile id
- 1 byte for metatile id
- 4 bits for the number of frames in the tile's animation, if applicable
- 4 bits for the frame number of a frame in an animated tile, if applicable
Animated tiles are composed of up to 16 consecutive tiles which act as its frames. The first "frame" in this set should have its "frames" field set to how many frames there are in the animation.

# Creating a new map
A map consists of a json config file for the map's information, and a btf file for the tilemap layout. A btf file is hard to make without an editor, so I should make one. It's essentially a binary file containing raw data representing each tile in a tile map, in order from left to right and top to bottom. The structure for this data is as follows:
- 2 bytes for the tile's id in the tileset (tileset is specified in json config)
- 4 bits for collision type or elevation
- 4 bits signifying if the tile can be passed from each cardinal direction WENS
- 1 byte for the metatile id of that tile

# Entity scripting
The entities in this framework can be scripted for movement relatively simply. there are two types of commands: move\_x and move\_x\_now. "now" commands queue up that movement to be done next, regardless of what else is in the entity's movement queue. The other command pushes that movement to the end of the movement queue. The commands are as follows (each has a "now" variant as well):
- move\_up
- move\_down
- move\_left
- move\_right
- move\_wait (waits for 1 second)
- move\_turn\_90 (Turns 90 degrees)
- move\_turn\_180
- move\_turn\_270
- move\_turn\_north
- move\_turn\_west
- move\_turn\_south
- move\_turn\_east
Entities are planned to have more fields that can be loaded from config. These will be configured in their map files. Also planned is cross-map entities (including the player).

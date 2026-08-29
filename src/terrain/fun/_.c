#include "settings.c"
#include "chunk.c"
#include "block_vox.c"
#include "debug.c"
#include "terrain.c"
#include "terminal.c"
#include "set.c"
#include "game.c"

entity get_terrain_id() {
    return zox_id(Terrain);
}
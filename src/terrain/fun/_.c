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

int get_chunk_index(int i, int j, int rows) {
    return (i + rows + 1) * (rows + rows + 1) + (j + rows + 1);
}
// vertical_rows + vertical_rows +  - rows + rows + 1
int calculate_terrain_chunks_count(int rows, int slabs) {
    return (slabs + slabs + 1) * (rows + rows + 1) * (rows + rows + 1);
}

int get_chunk_index_2(
    int i,
    int j,
    int k,
    int rows,
    int vertical)
{
    i += rows;
    j += vertical;
    k += rows;
    return i * (rows + rows + 1) + j * (rows + rows + 1) * (rows + rows + 1) + k;
}

int get_chunk_index_3(
    int3 position,
    int rows,
    int vertical)
{
    position.x += rows;
    position.y += vertical;
    position.z += rows;
    return position.x * (rows + rows + 1) + position.y  * (rows + rows + 1) * (rows + rows + 1) + position.z;
}

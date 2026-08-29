#define directory_voxes "voxes"
const byte max_vox_file_lods = 5;
int files_voxes_count = 0;
entity *files_voxes;
string_hashmap *files_hashmap_voxes;
const byte is_generate_vox_airs = 1;
const float fracture_dark_multiplier = 0.6f; // 0.13f
const float grass_blend_dark_multiplier = 0.8f; // 0.13f
byte disable_block_voxes = 0;
byte disable_block_vox_generation = 0;

static inline bool in_bounds(byte3 p, byte size) {
    return p.x < size && p.y < size && p.z < size;
}

// TODO: Move to octree macros
static inline void set_voxel_safe(VoxelNode *tree, byte depth, byte3 p, byte v) {
    if (!in_bounds(p, octree_size(depth))) {
        // zox_logw("Position [%ix%ix%i] out of B [%i]", p.x, p.y, p.z, depth);
        return;
    }
    set_VoxelNode(tree, depth, p, v);
}

// Shifts our node to the new blueprint size
static inline void scale_node_transform(
    byte3 bounds,
    byte3* position,
    byte3* size)
{
    uint blueprint_length = octree_size(nodegraph_max_depth);
    position->x = (position->x * bounds.x) / blueprint_length;
    position->y = (position->y * bounds.y) / blueprint_length;
    position->z = (position->z * bounds.z) / blueprint_length;
    size->x = (size->x * bounds.x) / blueprint_length;
    size->y = (size->y * bounds.y) / blueprint_length;
    size->z = (size->z * bounds.z) / blueprint_length;
    if (!size->x) size->x = 1;
    if (!size->y) size->y = 1;
    if (!size->z) size->z = 1;
}

// Scales a node transform from the fixed nodegraph depth to the target model depth.
// Since octree sizes are powers of two, this is an exact bit shift.
static inline void shift_node_transform(byte model_depth, byte3* position, byte3* size) {
    byte node_depth = nodegraph_max_depth;
    int shift = (int)model_depth - (int)nodegraph_max_depth;
    if (shift > 0) {
        position->x <<= shift;
        position->y <<= shift;
        position->z <<= shift;
        size->x <<= shift;
        size->y <<= shift;
        size->z <<= shift;
    } else if (shift < 0) {
        byte shift = node_depth - model_depth;
        position->x >>= shift;
        position->y >>= shift;
        position->z >>= shift;
        size->x >>= shift;
        size->y >>= shift;
        size->z >>= shift;
    }
    if (!size->x) size->x = 1;
    if (!size->y) size->y = 1;
    if (!size->z) size->z = 1;
}

// Models
#include "vox.c"
#include "outlines.c"
#include "io.c"
#include "voronoi3.c"
// graphs
#include "nodegraph.c"
// nodes
#include "paint.c"
#include "cube.c"
#include "sphere.c"
#include "ellipsoid.c"
#include "cylinder.c"


void process_arguments_voxes(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        /*if (strcmp(args[i], "--disableoutlines") == 0) {
            zox_block_outlines = 0;
        } else if (strcmp(args[i], "--enableoutlines") == 0) {
            zox_log("Enabling Block Outlines");
            zox_block_outlines = 1;
        } else */
        if (strcmp(args[i], "--noblockvoxes") == 0 || strcmp(args[i], "--nominivoxes") == 0) {
            zox_log("Disabling Block Voxes");
            disable_block_voxes = 1;
        }
    }
}
// A chunk's lights and voxels at a mesh's depth with a voxel of border from its 26 neighbours, flattened once a
//  mesh (octree_fill_grid): a face then indexes its 9 lights and 4 solids, it walked a tree and took a lock for
//  each (13 a face, thousands of faces a chunk), doc/tech/octrees.md
//  - smooth lighting (sys/build_smooth.c) and gpu meshing (fun/gpu.c, the shader reads the same grid) use it
typedef struct {
    byte* lights;
    byte* voxels;
    int length;         // the chunk's voxels a side at depth, + 2
    byte present[27];   // which of the 27 chunks have lights, a missing one reads as past the world
    byte voxels_present[27];    // which have voxels, a missing one is air (gpu meshing culls faces into it)
} smooth_grid;

#define smooth_grid_depth_max 5

static inline byte smooth_grid_fill(
    smooth_grid* grid,
    const LightNode** lights,
    spinlock** lights_locks,
    const VoxelNode** voxels,
    spinlock** voxels_locks,
    byte depth)
{
    if (depth > smooth_grid_depth_max) {
        return 0;
    }
    const int size = octree_size(depth);
    grid->length = size + 2;
    const size_t cells = (size_t) grid->length * grid->length * grid->length;
    // a pad after each (calloc): gpu meshing uploads whole uints (gpu_mesh_grid_bytes, depth 0's 27 is 28)
    const size_t padded = (cells + 3) & ~(size_t) 3;
    grid->lights = calloc(padded * 2, 1);
    if (!grid->lights) {
        return 0;
    }
    grid->voxels = grid->lights + padded;
    // missing chunks are air
    memset(grid->voxels, 0, cells);
    for (sbyte z = -1; z <= 1; z++)
    for (sbyte y = -1; y <= 1; y++)
    for (sbyte x = -1; x <= 1; x++) {
        const byte i = OCTREE_INDEX(x, y, z);
        const int3 origin = { x * size + 1, y * size + 1, z * size + 1 };
        grid->present[i] = lights[i] && lights_locks[i];
        if (grid->present[i]) {
            spin_lock(lights_locks[i]);
            octree_fill_grid(lights[i], sizeof(LightNode), octree_type_offset_LightNode, 0, depth, int3_zero, origin,
                grid->lights, grid->length, 1);
            spin_unlock(lights_locks[i]);
        }
        grid->voxels_present[i] = voxels[i] && voxels_locks[i];
        if (grid->voxels_present[i]) {
            spin_lock(voxels_locks[i]);
            octree_fill_grid(voxels[i], sizeof(VoxelNode), octree_type_offset_VoxelNode, 0, depth, int3_zero, origin,
                grid->voxels, grid->length, 0);
            spin_unlock(voxels_locks[i]);
        }
    }
    return 1;
}


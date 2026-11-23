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
    byte voxel_depths[27];      // set before a fill: the depth each draws at, its voxels no deeper (0: the grid's)
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
            const byte stop = grid->voxel_depths[i] && grid->voxel_depths[i] < depth ? grid->voxel_depths[i] : depth;
            octree_fill_grid_stop(voxels[i], sizeof(VoxelNode), octree_type_offset_VoxelNode, 0, depth, int3_zero, origin,
                grid->voxels, grid->length, 0, stop);
            spin_unlock(voxels_locks[i]);
        }
    }
    return 1;
}

// a mesh's borders with the 6 beside it, as cpu sides meet them (build_voxel_sides), after smooth_grid_fill (only its
//  voxels change, lights stay):
//  - drawn finer than the mesh (voxel_depths deeper than the grid's): a border cell holds the neighbour's node at the
//    mesh's depth, its most common solid, so it hid the mesh's face where the neighbour draws air along the border
//    (holes where lods met, a far chunk's wall missing beside a near one, tst stream_seams); the cell is air when any
//    voxel the neighbour draws against it doesn't hide the mesh's faces
//  - open (chunk_side_open, unsettled): its cells are air, every face into it shows
//  - hides: a voxel's, 1 if it hides the mesh's faces (block_material_hides), air's 0
static inline void smooth_grid_borders(
    smooth_grid* grid,
    const VoxelNode** voxels,
    spinlock** voxels_locks,
    byte depth,
    const byte* hides)
{
    const int size = octree_size(depth);
    const int length = grid->length;
    for (byte direction = 0; direction < 6; direction++) {
        const byte axis = direction / 2, positive = direction % 2;
        const sbyte s = positive ? 1 : -1;
        const byte j = OCTREE_INDEX(axis == 0 ? s : 0, axis == 1 ? s : 0, axis == 2 ? s : 0);
        const byte drawn = grid->voxel_depths[j];
        if (!grid->voxels_present[j] || drawn <= depth) {
            continue;
        }
        const byte open = drawn == chunk_side_open;
        const int k = open ? 1 : 1 << (drawn - depth);     // its voxels a cell, along each axis
        const byte a = axis == 0 ? 1 : 0, b = axis == 2 ? 1 : 2;
        if (!open) {
            spin_lock(voxels_locks[j]);
        }
        for (int u = 0; u < size; u++) {
            for (int v = 0; v < size; v++) {
                int cell[3];
                cell[axis] = positive ? size : -1;
                cell[a] = u;
                cell[b] = v;
                byte* value = &grid->voxels[((cell[0] + 1) * length + cell[1] + 1) * length + cell[2] + 1];
                if (!*value) {
                    continue;
                }
                byte air = open;
                for (int fu = 0; fu < k && !air; fu++) {
                    for (int fv = 0; fv < k && !air; fv++) {
                        int fine[3];
                        fine[axis] = positive ? 0 : size * k - 1;
                        fine[a] = u * k + fu;
                        fine[b] = v * k + fv;
                        air = !hides[getv_VoxelNode(voxels[j], drawn, (byte3) { fine[0], fine[1], fine[2] })];
                    }
                }
                if (air) {
                    *value = 0;
                }
            }
        }
        if (!open) {
            spin_unlock(voxels_locks[j]);
        }
    }
}


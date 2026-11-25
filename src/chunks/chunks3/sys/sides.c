// a neighbour unsettled: every face into it shows (chunk_side_depth)
#define chunk_side_open 255

// a chunk's faces: which show is worked out from its voxels and its neighbours' (sys/build_grid.c, the meshes' rule),
//  these are the pieces of it shared: what hides a face, the depths chunks draw at, locking their trees together

// a node's block material, every block solid without a table
static inline byte node_material(
    const byte* materials,
    const VoxelNode* node)
{
    byte value = node->value;
    if (!value) {
        return zox_block_material_none;
    }
    return materials ?
        materials[value] :
        zox_block_material_solid;
}

// delves down a voxel node, but only one one side
// Returns 0 if any of it doesn't hide a face of the material (air, a see through other material)
static inline byte get_node_sides_all_hide(
    const byte* materials,
    byte material,
    const VoxelNode* node,
    byte axis,
    byte side,
    byte distance)
{
    if (!node) {
        return 0;
    }
    // at end of node tree, return if it hides
    if (!distance || !has_children_VoxelNode(node)) {
        return block_material_hides(node_material(materials, node), material);
    }
    VoxelNode* kids = get_children_VoxelNode(node);
    if (!kids) {
        zox_loge("get_node_sides_all_hide: null children.");
        return 0;
    }
    distance--;
    const byte* indices = side_child_indices[axis][side];
    for (byte i = 0; i < 4; i++) {
        if (!get_node_sides_all_hide(
            materials,
            material,
            &kids[indices[i]],
            axis,
            side,
            distance
        )) {
            return 0;
        }
    }
    return 1;
}

// TODO: Only generate sides at target depth
static inline byte zox_lock_voxel_trees(
    spinlock* root_lock,
    spinlock** neighbor_locks,
    spinlock** locked_locks)
{
    byte count = 0;
    if (root_lock) {
        locked_locks[count++] = root_lock;
    }
    for (byte i = 0; i < 6; i++) {
        spinlock* lock = neighbor_locks[i];
        if (!lock) {
            continue;
        }
        byte duplicate = 0;
        for (byte j = 0; j < count; j++) {
            if (locked_locks[j] == lock) {
                duplicate = 1;
                break;
            }
        }
        if (!duplicate) {
            locked_locks[count++] = lock;
        }
    }
    // Deterministic ordering.
    for (byte i = 1; i < count; i++) {
        spinlock* lock = locked_locks[i];
        uintptr_t key = (uintptr_t) lock;
        byte j = i;
        while (j > 0 &&
            (uintptr_t) locked_locks[j - 1] > key)
        {
            locked_locks[j] = locked_locks[j - 1];
            j--;
        }
        locked_locks[j] = lock;
    }
    for (byte i = 0; i < count; i++) {
        spin_lock(locked_locks[i]);
    }
    return count;
}

static inline void zox_unlock_voxel_trees(
    spinlock** locked_locks,
    byte count)
{
    for (byte i = count; i > 0; i--) {
        spin_unlock(locked_locks[i - 1]);
    }
}

// the depth a chunk draws at: its RenderDepth, no deeper than its voxels (NodeDepth only grows, a chunk the stream
//  left keeps its detail and draws coarser)
static inline byte chunk_drawn_depth(ecs* world, entity chunk) {
    const byte node_depth = zox_getv(chunk, NodeDepth);
    if (!zox_has(chunk, RenderDepth)) {
        return node_depth;
    }
    const byte render_depth = zox_getv(chunk, RenderDepth);
    return render_depth < node_depth ? render_depth : node_depth;
}

// what a chunk's neighbours work out their faces against: open while it's unsettled (its lod changing, ChunkLodDirty,
//  or generating), else the depth it draws (chunk_drawn_depth)
//  - unsettled it'll draw something else soon (a swap, voxels generated finer, its first mesh): built against what it
//    drew, a face it hid showed a hole when it changed first; open, faces it covers are behind its voxels, unseen, and
//    they're rebuilt against it once it settles (chunk_mesh_transition_system) (tst chunk_mesh_flicker)
static inline byte chunk_side_depth(ecs* world, entity chunk) {
    if (zox_has(chunk, ChunkLodDirty) || (zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk))) {
        return chunk_side_open;
    }
    return chunk_drawn_depth(world, chunk);
}

// its neighbours rebuild their faces against it (chunk_side_depth): it's unsettled now, or settled at another depth (a
//  face it hid at its old depth stayed hidden: holes where lods met as you walked about)
static inline void chunk_neighbors_rebuild_sides(ecs* world, entity chunk) {
    const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
    for (byte n = 0; n < chunk_neighbors_length; n++) {
        const entity neighbor = neighbors->value[n];
        if (zox_valid(neighbor) && zox_has(neighbor, Chunk3)) {
            zox_add(neighbor, BuildChunkSides);
        }
    }
}

// a chunk's meshes rebuild (BuildMesh): its voxels or a neighbour changed (BuildChunkSides, from the triggers and
//  chunk_neighbors_rebuild_sides); their faces come from the voxels then (sys/build_grid.c)
//  - added even when it seems built already: a mesh building this frame had its BuildMesh's removal deferred, has()
//    still saw it, and it kept the faces from before (holes as lods changed)
void chunk_sides_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity meshes[zox_chunk_meshes_max];
        const uint meshes_count = get_chunk_meshes(world, e, meshes);
        for (uint j = 0; j < meshes_count; j++) {
            zox_add(meshes[j], BuildMesh);
        }
        zox_remove(e, BuildChunkSides);
        zox_sys_increment();
    }
    zox_sys_on_end();
} zoxd_system(chunk_sides_system);

// TODO: Calculate only sides needed

byte zox_sides_only_target_depth = 0;
#define zox_chunk_sides_budget_ms 3.0

// Sides System will generate our chunk sides before rendering, per block material (blocks/fun/solids.c)
// TODO: Let Colored use this too
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

static inline byte get_adjacent_depth(
    byte depth,
    const byte* neighbor_depths,
    int3 position,
    byte direction)
{
    if (is_on_edge_octree(depth, position, direction)) {
        return neighbor_depths[direction];
    } else {
        return depth;
    }
}

// the node beside position, from a neighbour chunk past the edge: at neighbor_depth there when that's coarser (it
//  draws that, a deeper tree than it draws gave faces against voxels it doesn't show: holes where lods met)
static inline const VoxelNode* get_adjacentn_VoxelNode2(
    const VoxelNode** neighbors,
    const VoxelNode* vnode,
    int3 position,
    byte depth,
    byte direction,
    byte neighbor_depth)
{
    if (!vnode) {
        return NULL;
    }
    short length = octree_size(depth);
    position = move_position(position, direction);
    if (position.x < 0 || position.x >= length ||
        position.y < 0 || position.y >= length ||
        position.z < 0 || position.z >= length)
    {
        position = reverse_position(position, direction, length);
        vnode = neighbors[direction];
        if (!vnode) {
            return NULL;
        }
        if (neighbor_depth < depth) {
            const byte shift = depth - neighbor_depth;
            return get_VoxelNode(vnode, neighbor_depth,
                (byte3) { position.x >> shift, position.y >> shift, position.z >> shift });
        }
    }
    const VoxelNode* result =
        get_VoxelNode(
            vnode,
            depth,
            int3_to_byte3(position));
    return result;
}

// this function accounts for size of drawing voxels
// NOTE: Returns 1 to build the side
static inline byte build_voxel_sides(
    const byte* materials,
    byte material,
    const VoxelNode* root,
    const VoxelNode** neighbor_voxels,
    const byte* neighbor_depths,
    const VoxelNode* voxels,
    SidesOctree* sides,
    byte depth,
    byte3 position,
    byte direction)
{
    int3 positioni = byte3_to_int3(position);
    // Accounts for Dig vs Render Difference
    byte adjacent_depth = get_adjacent_depth(
        depth,
        neighbor_depths,
        positioni,
        direction);
    const VoxelNode* adjacent_node = get_adjacentn_VoxelNode2(
        neighbor_voxels,
        root,
        positioni,
        depth,
        direction,
        adjacent_depth);
    // NOTE: Edge of World Voxels
    if (!adjacent_node) {
        // NOTE: Adds rendered facefor top of world
        if (direction == direction_up) {
            return 1;
        }
        return 0;
    }
    if (zox_dbg_render_all_sides) {
        //if (nearby_lock != NULL) spin_unlock(nearby_lock);
        return 1;
    }
    if (adjacent_depth > depth) {
        byte dist = adjacent_depth - depth;
        SideInfo side = side_infos[direction];
        return !get_node_sides_all_hide(
            materials,
            material,
            adjacent_node,
            side.axis,
            side.side,
            dist);
    }
    return !block_material_hides(node_material(materials, adjacent_node), material);
}

// builds one material's sides: a bit per face that draws, at every depth down to the target
static inline byte build_sides_dig(
    const byte* materials,
    byte material,
    const VoxelNode* root,
    const VoxelNode** neighbor_voxels,
    const byte* neighbor_depths,
    const VoxelNode* voxels,
    SidesOctree* sides,
    byte target_depth,
    byte depth,
    byte3 position,
    byte dbg_log)
{
    // NOTE: if air we stop here at any branch node
    // NOTE: Assumes voxel octree has optimized for solid non air values first
    byte value = voxels->value;
    if (!value) {
        // collapse sub node and set to 0
        sides->value = 0;
        if (has_children_SidesOctree(sides)) {
            collapse_SidesOctree(sides);
        }
        return 0;
    }
    // We should keep digging even when it's closed
    // NOTE: Dig down voxel + sides octree
    byte has_vkids = has_children_VoxelNode(voxels);
    byte is_material = node_material(materials, voxels) == material;
    // all another material's: no faces in here, nothing to open
    if (!has_vkids && !is_material) {
        sides->value = 0;
        if (has_children_SidesOctree(sides)) {
            collapse_SidesOctree(sides);
        }
        return 0;
    }
    if (depth < target_depth &&
        (zox_split_textured_quads ||
            (!zox_split_textured_quads &&
            has_vkids)))
    {
        if (is_closed_SidesOctree(sides)) {
            open_one_SidesOctree(sides);
            // NOTE: If fails malloc
            if (is_closed_SidesOctree(sides)) {
                zox_loge("Sides Malloc Failed");
                return 0;
            }
        }
        byte3 child_position = {
            position.x << 1,
            position.y << 1,
            position.z << 1
        };
        SidesOctree* sides_kids = get_children_SidesOctree(sides);
        const VoxelNode* kids =
            has_vkids ?
                get_children_VoxelNode(voxels):
                NULL;
        // byte did_build = 0;
        byte child_depth = depth + 1;
        for (byte i = 0; i < 8; i++) {
            SidesOctree* sides_kid = &sides_kids[i];
            const VoxelNode* child_voxel =
                has_vkids ?
                    &kids[i] :
                    voxels;
            byte3 nposition = byte3_add(
                child_position,
                octree_positions[i]);
            build_sides_dig(
                materials,
                material,
                root,
                neighbor_voxels,
                neighbor_depths,
                child_voxel,
                sides_kid,
                target_depth,
                child_depth,
                nposition,
                dbg_log);
        }
        if (zox_sides_only_target_depth && depth < target_depth) {
            return 0;
        }
    }
    // Set our sides here at depth
    sides->value = 0;
    // NOTE: if the material's, check all sides, Set side 1 if it draws
    if (is_material) {
        for (byte direction = 0; direction < 6; direction++) {
            if (build_voxel_sides(
                materials,
                material,
                root,
                neighbor_voxels,
                neighbor_depths,
                voxels,
                sides,
                depth,
                position,
                direction))
            {
                sides->value |= (1 << (direction + 1));
            }
        }
    }
    if (dbg_log && sides->value) {
        zox_log("Sides was added at Depth [%i] Pos [%ix%ix%i] -> [%i]",
            depth,
            position.x,
            position.y,
            position.z,
            sides->value);
    }
    return sides->value;
}

// TODO: The issue is now we generate sides per level instead of just whats needed! Perhaps a bitmask of which sides are generated is better! .... but when swapping its good to have ready for build! - we can just keep a depth of sides and when updating render depth, it can build higher if needed, this way its not building high on lower depth chunks

// TODO: Create a Sides entity so each BlockMaterial can have its own meshes underneath (Water would need seperate sides)

// TODO: Add a BlockSolidityCache to BlockManager

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
//  left keeps its detail and draws coarser); its neighbours' faces are worked out against this
static inline byte chunk_drawn_depth(ecs* world, entity chunk) {
    const byte node_depth = zox_getv(chunk, NodeDepth);
    if (!zox_has(chunk, RenderDepth)) {
        return node_depth;
    }
    const byte render_depth = zox_getv(chunk, RenderDepth);
    return render_depth && render_depth < node_depth ? render_depth : node_depth;
}

// its neighbours rebuild their faces against it: it draws at another depth now (chunk_drawn_depth), a face it hid at
//  its old depth stayed hidden (holes where lods met as you walked about)
static inline void chunk_neighbors_rebuild_sides(ecs* world, entity chunk) {
    const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
    for (byte n = 0; n < chunk_neighbors_length; n++) {
        const entity neighbor = neighbors->value[n];
        if (zox_valid(neighbor) && zox_has(neighbor, Chunk3)) {
            zox_add(neighbor, BuildChunkSides);
        }
    }
}

// NOTE: Keeps updated when voxels octree changes
// NOTE: Calculates the sides of a voxel octree per material, into the chunk's materials (fun/materials.c)
void chunk_sides_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    init_side_child_indices();
    // a few ms a worker a frame, the rest next frame (whole chunks at once were 290ms lag spikes)
    zox_sys_budget(sides, zox_disable_process_skips ? 1000000 : zox_chunk_sides_budget_ms);
    byte materials[256];
    entity manager = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(VoxelNode);
    zox_sys_out(VoxelNodeLock);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, voxels);
        zox_sys_o(VoxelNodeLock, voxels_lock);
        if (zox_sys_budget_spent(sides)) {
            continue;
        }
        zox_sys_budget_start(sides);
        const entity chunk_manager = zox_get_link(world, e, BlockManagerLink);
        if (!manager || manager != chunk_manager) {
            manager = chunk_manager;
            get_block_materials(world, manager, materials);
        }
        const VoxelNode* neighbor_voxels[6];
        spinlock* neighbor_locks[6];
        byte neighbor_depths[6];
        for (byte j = 0; j < 6; j++) {
            entity e = neighbors->value[j];
            // no voxels, no lock: a lock left unset here was a segfault in zox_lock_voxel_trees
            neighbor_depths[j] = 0;
            neighbor_voxels[j] = NULL;
            neighbor_locks[j] = NULL;
            if (!zox_valid(e)) {
                continue;
            }
#ifdef zox_safety_checks
            if (!zox_has(e, NodeDepth) || !zox_has(e, VoxelNode)) {
                // a deleted chunk's id was an npc's stat, alive (flecs revived it, patched: doc/flecs_patches.md,
                //  instance child ids); should it show again, what it is and what the terrain's map holds say more
                char* type = zox_type_string(e);
                const int3 at = int3_add(zox_getv(it->entities[i], ChunkPosition), get_direction_int3(j));
                const entity mapped = int3_hashmap_get(zox_get(zox_get_parent(world, it->entities[i]), ChunkLinks)->value, at);
                zox_loge("Neighbor [%i] of [%s] isn't a chunk: id [%u] gen [%u], the map holds [%u] gen [%u], type [%.160s]",
                    j, zox_sys_e_name, (uint) e, (uint) (e >> 32), (uint) mapped, (uint) (mapped >> 32), type ? type : "none");
                free(type);
                continue;
            }
#endif
            neighbor_depths[j] = chunk_drawn_depth(world, e);
            neighbor_voxels[j] = zox_get(e, VoxelNode);
            neighbor_locks[j] = &(zox_mut(e, VoxelNodeLock)->value);
        }
        entity chunk_materials[zox_block_materials];
        const uint materials_count = get_chunk_materials(world, e, chunk_materials);
        // its octree and neighbors' locked while read (thread_safety.md)
        spinlock* locked_voxel_locks[7];
        const byte locked_voxel_count = zox_lock_voxel_trees(
            &voxels_lock->value,
            neighbor_locks,
            locked_voxel_locks);
        for (uint j = 0; j < materials_count; j++) {
            const entity material = chunk_materials[j];
            // a gpu meshed material has no cpu sides (its mesh reads the voxels), its meshes are still triggered below
            if (chunk_material_gpu(zox_getv(material, BlockMaterial))) {
                continue;
            }
            SidesOctree* sides = zox_mut(material, SidesOctree);
            spinlock* sides_lock = &(zox_mut(material, SidesOctreeLock)->value);
            spin_lock(sides_lock);
            build_sides_dig(
                materials,
                zox_getv(material, BlockMaterial),
                voxels,
                neighbor_voxels,
                neighbor_depths,
                voxels,
                sides,
                depth->value,
                0,
                byte3_zero,
                dbg_log);
            spin_unlock(sides_lock);
        }
        zox_unlock_voxel_trees(
            locked_voxel_locks,
            locked_voxel_count);
        if (dbg_log) {
            zox_log("Built Sides [%s]", zox_sys_e_name);
        }
        // Triggers meshes to build now: added even when it seems built already, a mesh building this frame had its
        //  BuildMesh's removal deferred, has() still saw it, and it kept the faces from before (holes as lods changed)
        entity meshes[zox_chunk_meshes_max];
        const uint meshes_count = get_chunk_meshes(world, e, meshes);
        for (uint j = 0; j < meshes_count; j++) {
            zox_add(meshes[j], BuildMesh);
        }
        zox_remove(e, BuildChunkSides);
        zox_sys_budget_end(sides);
        zox_sys_increment();
    }
    zox_sys_on_end();
} zoxd_system(chunk_sides_system);

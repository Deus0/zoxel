// Spawns a chunk's materials and their meshes (fun/materials.c)
//  - a material for each block material in the chunk's voxels, when they change
//  - a mesh per material at the chunk's depth, if it has none there

// does the material show a mesh (one not Disabled)
static inline byte chunk_material_shown(ecs* world, entity material) {
    entity meshes[zox_chunk_meshes_max];
    const uint count = zox_get_children_by_id(world, material, meshes, zox_chunk_meshes_max, zox_id(ChunkMesh));
    for (uint i = 0; i < count; i++) {
        if (!zox_has(meshes[i], Disabled)) {
            return 1;
        }
    }
    return 0;
}

// a material's mesh at the depth, spawned if it has none: 0 if it had one
static inline entity spawn_chunk_material_mesh(
    ecs* world,
    entity material,
    entity tilemap,
    byte depth,
    float4x4 matrix,
    byte render_disabled)
{
    entity active_mesh = zox_get_link(world, material, ActiveMesh);
    if (zox_valid(active_mesh) && zox_getv(active_mesh, RenderDepth) == depth) {
        return 0;
    }
    if (chunk_mesh_at_depth(world, material, depth)) {
        return 0;
    }
    // Make sure old one isnt building
    if (zox_valid(active_mesh) && !zox_has(active_mesh, BuildDisabled)) {
        zox_add(active_mesh, BuildDisabled);
    }
    return spawn_chunk_mesh_textured(
        world,
        material,
        matrix,
        depth,
        render_disabled,
        tilemap);
}

// NOTE: When Depth changes or Voxels Generate, the materials and meshes that don't exist are spawned
// returns the new meshes, a new material's shown as it builds (it shows nothing else)
static inline byte spawn_chunk_meshes(
    ecs* world,
    entity e,
    const VoxelNode* voxels,
    spinlock* lock,
    const byte* block_materials,
    byte depth,
    float4x4 matrix,
    byte render_disabled,
    byte* swaps)
{
    spin_lock(lock);
    const byte mask = get_node_materials(voxels, block_materials);
    spin_unlock(lock);
    // air and vox models only: no faces
    if (!(mask >> 1)) {
        return 0;
    }
    entity tilemap = zox_get_link(world, e, Tilemap);
    if (!zox_valid(tilemap)) {
        zox_loge("Tilemap Material is Invalid");
        return 0;
    }
    byte spawned = 0;
    for (byte i = 1; i < zox_block_materials; i++) {
        if (!(mask & (1 << i))) {
            continue;
        }
        entity material = get_chunk_material(world, e, i);
        // a material spawned now isn't made until this system ends: its links and children can't be read yet
        const byte is_new = !material;
        if (is_new) {
            material = spawn_chunk_material(world, e, i, tilemap);
        }
        const byte shown = !is_new && chunk_material_shown(world, material);
        const entity mesh = is_new ?
            spawn_chunk_mesh_textured(world, material, matrix, depth, render_disabled, tilemap) :
            spawn_chunk_material_mesh(world, material, tilemap, depth, matrix, render_disabled);
        if (!mesh) {
            continue;
        }
        spawned++;
        // a new chunk shows it as it builds; one showing another mesh swaps to it once it's ready (enabled now it
        //  drew over the old one, dark until lit: tst lod_swap)
        if (!shown) {
            zox_remove(mesh, Disabled);
        } else {
            *swaps = 1;
        }
    }
    return spawned;
}

void chunk_mesh_spawn_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    byte block_materials[256];
    entity manager = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    zox_sys_in(RenderDepth);
    zox_sys_in(VoxelNode);
    zox_sys_out(VoxelNodeLock);
    zox_sys_out(ChunkMeshTimer);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(VoxelNode, voxels);
        zox_sys_o(VoxelNodeLock, lock);
        zox_sys_o(ChunkMeshTimer, timer);
        const entity chunk_manager = zox_get_link(world, e, BlockManagerLink);
        if (!manager || manager != chunk_manager) {
            manager = chunk_manager;
            get_block_materials(world, manager, block_materials);
        }
        byte swaps = 0;
        if (spawn_chunk_meshes(
            world,
            e,
            voxels,
            &lock->value,
            block_materials,
            depth->value,
            matrix->value,
            render_disabled->value,
            &swaps))
        {
            timer->value = zox_current_time;
            if (swaps && !zox_has(e, ChunkLodDirty)) {
                zox_setv(e, ChunkLodDirty, zox_chunk_lod_dirty_toggle);
            }
        }
        if (dbg_log) {
            zox_log("Spawned Chunk Mesh on [%s] Depth [%i] Disabled [%i]",
                zox_sys_e_name,
                depth->value,
                render_disabled->value);
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_mesh_spawn_system);

void chunk_mesh_spawn2_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    byte block_materials[256];
    entity manager = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    zox_sys_in(RenderDepth);
    zox_sys_in(VoxelNode);
    zox_sys_out(VoxelNodeLock);
    zox_sys_out(ChunkLodDirty);
    zox_sys_out(ChunkMeshTimer);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(VoxelNode, voxels);
        zox_sys_o(VoxelNodeLock, lock);
        zox_sys_o(ChunkLodDirty, dirty);
        zox_sys_o(ChunkMeshTimer, timer);
        if (dirty->value != zox_chunk_lod_dirty_spawn) {
            continue;
        }
        const entity chunk_manager = zox_get_link(world, e, BlockManagerLink);
        if (!manager || manager != chunk_manager) {
            manager = chunk_manager;
            get_block_materials(world, manager, block_materials);
        }
        byte swaps = 0;
        if (spawn_chunk_meshes(
            world,
            e,
            voxels,
            &lock->value,
            block_materials,
            depth->value,
            matrix->value,
            render_disabled->value,
            &swaps))
        {
            timer->value = zox_current_time;
        }
        dirty->value = zox_chunk_lod_dirty_toggle;
        if (dbg_log) {
            zox_log("[LodDirty] + Chunk Mesh on [%s] Depth [%i]",
                zox_sys_e_name,
                depth->value);
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_mesh_spawn2_system);


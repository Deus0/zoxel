// #define zox_debug_chunk_link_system
static inline byte can_have_characters(ecs*, entity);

void zox_log_chunk_removed(
    ecs *world,
    entity e,
    entity e2)
{
    int3 position = zox_getv(e, ChunkPosition);
    zox_log("- chunk [%s] removed e [%s] [%ix%ix%i]",
        zox_get_name(e),
        zox_get_name(e2),
        position.x,
        position.y,
        position.z);
}

void zox_log_chunk_added(
    ecs *world,
    entity e,
    entity e2)
{
    int3 position = zox_getv(e, ChunkPosition);
    zox_log("+ chunk [%s] added e [%s] [%ix%ix%i]",
        zox_get_name(e),
        zox_get_name(e2),
        position.x,
        position.y,
        position.z);
}

byte set_entity_chunk(
    ecs* world,
    entity e,
    ChunkLink* link,
    entity new_chunk,
    byte dbg_log)
{
    entity old_chunk = link->value;
    if (!zox_valid(new_chunk) || !zox_has(new_chunk, ChunkEntities) || old_chunk == new_chunk) {
        return 0;
    }
    // remove entity from old chunk
    if (zox_valid(old_chunk)) {
        zox_muter(old_chunk, ChunkEntities, entites);
        remove_from_ChunkEntities(entites, e);
        if (dbg_log) {
            zox_log_chunk_removed(world, old_chunk, e);
        }
    }
    // add entity to new chunk
    zox_mut_begin(new_chunk, ChunkEntities, entites);
    if (add_to_ChunkEntities(entites, e)) {
        zox_mut_end(new_chunk, ChunkEntities)
        if (dbg_log) {
            zox_log_chunk_added(world, new_chunk, e);
        }
    } else {
        zox_log_error("failed to add npc to chunk");
    }
    link->value = new_chunk;
    // now render distabled
    byte chunk_render_disabled = zox_getv(new_chunk, RenderDisabled);
    zox_geter_value(new_chunk, RenderDistance, byte, chunk_render_distance);
    zox_geter_value(e, RenderDisabled, byte, character_render_disabled);
    // NOTE: Special case for air chunks
    // TODO: Ahhh think of how to fix this for flying npcs
    const VoxelNode* new_voxel_octree = zox_get(new_chunk, VoxelNode);
    byte is_air_chunk = !new_voxel_octree->value && !new_voxel_octree->ptr;
    if (!is_air_chunk && character_render_disabled != chunk_render_disabled) {
        zox_setv(e, RenderDisabled, chunk_render_disabled);
    }
    // now lod
    // calculate_lods
    // zox_geter_value(e, NodeDepth, byte, node_depth)
    // todo: this should be used by system itself
    if (zox_has(e, RenderDepth) && zox_has(e, MaxRenderDepth)) {
        zox_geter_value(e, RenderDepth, byte, old);
        zox_geter_value(e, MaxRenderDepth, byte, max_depth);
        byte render_depth = camera_distance_to_npc_render_depth(
            chunk_render_distance,
            max_depth);
        if (old != render_depth) {
            zox_setv(e, RenderDepth, render_depth);
            zox_setv(e, RenderDepthDirty, zox_dirty_trigger );
        }
    }
    return 1;
}

// NOTE: Links characters to terrain chunks
zox_sys2(ChunkLinkSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(Position3D);
    zox_sys_out(ChunkPosition);
    zox_sys_out(ChunkLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_o(ChunkPosition, chunk_position);
        zox_sys_o(ChunkLink, link);
        entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain)) {
            if (dbg_log) {
                zox_loge("Character [%s] has no Terrain linked",
                    zox_get_name(e));
            }
            continue; // these shouldn't be here
        }
        float terrain_scale = zox_getv(terrain, BlockScale);
        byte node_depth = zox_getv(terrain, NodeDepth);
        short length = octree_size(node_depth);
        int3 new_chunk_position = real_position_to_chunk_position(position->value, length, terrain_scale);
        // If already set and position has not changed
        if (zox_valid(link->value) && int3_equals(new_chunk_position, chunk_position->value)) {
            continue;
        }
        chunk_position->value = new_chunk_position;
        zox_geter(terrain, ChunkLinks, chunks);
        entity chunk = int3_hashmap_get(chunks->value, new_chunk_position);
        if (dbg_log) {
            zox_log("Character [%s] Linking Chunk [%ix%ix%i]:[%s]", zox_get_name(e), new_chunk_position.x, new_chunk_position.y, new_chunk_position.z, zox_get_name(chunk));
        }
        // NOTE: Disables if not set
        set_entity_chunk(
            world,
            e,
            link,
            chunk,
            dbg_log);
        if (zox_valid(link->value) && zox_has(e, DisableMovement)) {
            zox_remove(e, DisableMovement);
        } else if (!zox_valid(link->value) && !zox_has(e, DisableMovement)) {
            // check if in space here
            if (new_chunk_position.y <= render_distance_y) {
                zox_add(e, DisableMovement);
            }
        }
    }
} zox_sys_end(ChunkLinkSystem);

// chunk_lod_state_characters_update :: when terrain lod updates, update character lods

// ChunkDistanceDirty => ChunkEntities -> Update RenderDepths
void ChunkEntitiesLodSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(RenderDistance);
    zox_sys_in(ChunkEntities);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDistanceDirty, renderDistanceDirty);
        zox_sys_i(RenderDistance, renderDistance);
        zox_sys_i(ChunkEntities, entityLinks);
        if (renderDistanceDirty->value != zox_dirty_active) {
            continue;
        }
        for (int j = 0; j < entityLinks->length; j++) {
            const entity e2 = entityLinks->value[j];
            if (!(zox_valid(e2) && zox_has(e2, RenderDepth) && zox_has(e2, MaxRenderDepth))) {
                continue;
            }
            zox_geter_value(e2, MaxRenderDepth, byte, max_render_depth);
            const byte render_depth = camera_distance_to_npc_render_depth(renderDistance->value, max_render_depth);
            zox_geter_value(e2, RenderDepth, byte, old);
            if (old != render_depth) {
                zox_set(e2, RenderDepth, { render_depth });
                zox_set(e2, RenderDepthDirty, { zox_dirty_trigger });
                // zox_log("[%s] is now dirty [%i]", zox_get_name(e2), new_lod);
            }
        }
    }
} zoxd_system(ChunkEntitiesLodSystem)

// chunk_lod_state_characters_update :: when terrain lod updates, update character lods
// ChunkDistanceDirty => ChunkEntities -> Update RenderDepths
zox_sys2(ChunkEntitiesLodSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(RenderDistance);
    zox_sys_in(ChunkEntities);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDistanceDirty, state);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(ChunkEntities, entities);
        if (state->value != zox_dirty_active) {
            continue;
        }
        for (int j = 0; j < entities->length; j++) {
            entity e2 = entities->value[j];
            if (!(zox_valid(e2) && zox_has(e2, RenderDepth) && zox_has(e2, MaxRenderDepth))) {
                continue;
            }
            byte max_depth = zox_getv(e2, MaxRenderDepth);
            byte new_render_depth = camera_distance_to_npc_render_depth(distance->value, max_depth);
            byte old_render_depth = zox_getv(e2, RenderDepth);
            if (old_render_depth != new_render_depth) {
                zox_setv(e2, RenderDepth, new_render_depth );
                zox_setv(e2, RenderDepthDirty, zox_dirty_trigger);
            }
        }
    }
} zox_sys_end(ChunkEntitiesLodSystem);

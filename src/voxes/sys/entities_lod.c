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
            zox_geter_value(e2, MaxRenderDepth, byte, mdepth);
            byte rdepth = camera_distance_to_npc_render_depth(distance->value, mdepth);
            zox_geter_value(e2, RenderDepth, byte, old);
            if (old != rdepth) {
                zox_set(e2, RenderDepth, { rdepth });
                zox_set(e2, RenderDepthDirty, { zox_dirty_trigger });
            }
        }
    }
} zox_sys_end(ChunkEntitiesLodSystem);

// If Streamer2 is Dirty:
//  - Update Render Distances
//  - Update Render Depths
zox_sys2(TunkLodSystem) {
    byte dbg_log = 0;
    if (zox_cameras_disable_streaming) {
        return;
    }
    zox_sys_query();
    zox_sys_world();
    byte any_dirty = 0;
    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        if (any_dirty) {
            continue;
        }
        zox_sys_begin_2();
        zox_sys_in_2(StreamDirty2);
        for (int j = 0; j < it2.count; j++) {
            zox_sys_i_2(StreamDirty2, sdirty);
            if (sdirty->value == zox_dirty_active) {
                any_dirty = 1;
                break;
            }
        }
    }
    zox_sys_query_end();
    if (!any_dirty) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(TunkPosition);
    zox_sys_in(Chunk3Stack);
    zox_sys_out(RenderDistance);
    zox_sys_out(TunkLod);
    zox_sys_out(GenerateTunk);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkPosition, position);
        zox_sys_i(Chunk3Stack, stack);
        zox_sys_o(RenderDistance, distance);
        zox_sys_o(TunkLod, lod);
        zox_sys_o(GenerateTunk, generate);
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        int2 closest_point;
        byte had_streamer = 0;
        float closest_distance = 9999;
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2_at(1);
            zox_sys_in_2(StreamerLevel);
            zox_sys_in_2(StreamLink);
            zox_sys_in_2(StreamPosition2);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_i_2(StreamerLevel, level);
                zox_sys_i_2(StreamLink, stream_terrain);
                zox_sys_i_2(StreamPosition2, stream_point);
                if (level->value < 1) {
                    continue;
                }
                if (stream_terrain->value != terrain) {
                    continue;
                }
                float streamer_distance = int2_distance(stream_point->value, position->value);
                if (streamer_distance < closest_distance) {
                    had_streamer = 1;
                    closest_distance = streamer_distance;
                    closest_point = stream_point->value;
                }
            }
        }
        zox_sys_query_end();
        if (!had_streamer) {
            continue;
        }
        byte new_distance = get_camera_chunk2_distance(closest_point, position->value);
        if (distance->value == new_distance) {
            continue;
        }
        distance->value = new_distance;
        zox_setv(e, RenderDistanceDirty, 1);
        byte tunk_render_depth = camera_distance_to_terrain_render_depth(new_distance);
        if (tunk_render_depth > lod->value) {
            lod->value = tunk_render_depth;
            generate->value = zox_generate_tunk_start;
        }
        byte stack_i = 0;
        for (short y = - render_distance_y; y <= render_distance_y; y++, stack_i++) {
            entity chunk = stack->value[stack_i];
#ifdef zox_safety_checks
            if (!zox_valid(chunk)) {
                zox_logw("Chunk missing at [%ix%ix%i]", position->value.x, y, position->value.y);
                continue;
            }
#endif
            byte old_distance3 = zox_getv(chunk, RenderDistance);
            if (old_distance3 == new_distance) {
                continue;
            }
            zox_setv(chunk, RenderDistance, new_distance);
            zox_setv(chunk, RenderDistanceDirty, zox_dirty_trigger);
            // If same depth, continue
            byte old_depth = zox_getv(chunk, RenderDepth);
            if (old_depth == tunk_render_depth) {
                continue;
            }
            zox_setv(chunk, RenderDepth, tunk_render_depth);
            zox_setv(chunk, ChunkLodDirty, zox_chunk_lod_dirty_start);
            // Stop the active building
            entity active_mesh = zox_getv(chunk, ActiveMesh);
            // entity active_mesh = zox_get_link(world, chunk, ActiveMesh);
            if (zox_valid(active_mesh) &&
                !zox_has(active_mesh, BuildDisabled))
            {
                zox_add(active_mesh, BuildDisabled);
            }
            if (dbg_log) {
                zox_log("Chunk Depth Updated [%s]:[%i]", zox_getn(chunk), tunk_render_depth);
            }
            // NOTE: Clears the light if depth is set to increase
            if (disable_lights) {
                continue;
            }
            byte node_depth = zox_getv(chunk, NodeDepth);
            if (tunk_render_depth > node_depth) {
                // NOTE: Causes flicker but needed for fixing lighting in older chunks
                zox_muter(chunk, LightNode, lights);
                lights->value = darklight;
                collapse_LightNode(lights);
                if (y == render_distance_y) {
                    zox_add(e, GenerateSunlight);
                }
            }
        }
    }
} zox_sys_end(TunkLodSystem);


// NOTE: Theres an issue when we optimize, the light nodes can be set by the bigger nodes, so it was hard to debug

// a sunbeam down from pos at light: lights each voxel to the first solid, dimmed in each liquid (light_dim), queues a
//  flood from each at its light, and carries on into the chunk below at the light it left with
byte sunbeam(
    spinlock* lightlock,
    LightQueue* floodlight_queue,
    SunlightQueue* chunk_below_queue,
    LightNode* root_lights,
    const VoxelNode* root_vnode,
    byte depth, byte3 pos,
    byte light,
    byte min_light,
    const byte* solidity,
    const byte* liquids)
{
    byte dirty = 0;
    const int length = octree_size(depth);
    if (pos.y >= length) {
        zox_logw("position too high [%i]", pos.y);
        return dirty;
    }
    byte beam_stopped = 0;
    for (int y = pos.y; y >= 0; y--) {
        pos.y = y;
        const byte voxel = getv_VoxelNode(root_vnode, depth, pos);
        if (voxel && solidity[voxel - 1]) {
            beam_stopped = 1;
            break;
        }
        if (voxel && liquids[voxel - 1]) {
            light = light_dim(light, min_light);
        }
        spin_lock(lightlock);
        set_LightNode(root_lights, depth, pos, light);
        spin_unlock(lightlock);
        dirty = 1;
        zox_logv("+ SunLight [%i] Set at [%ix%ix%i]", light, pos.x, pos.y, pos.z);
        if (locks_enabled) spin_lock(&floodlight_queue->lock);
        a_LightQueue(floodlight_queue, (LightUpdate) {
            .light = light,
            .pos = pos,
            .depth = depth,
            .distance = light_propogation_distance
        });
        if (locks_enabled) spin_unlock(&floodlight_queue->lock);
    }
    if (dirty && chunk_below_queue && !beam_stopped && light > min_light) {
        if (locks_enabled) {
            spin_lock(&chunk_below_queue->lock);
        }
        a_SunlightQueue(chunk_below_queue, (SunlightUpdate) {
            .pos = (byte3) { pos.x, length - 1, pos.z },
            .light = light,
            .depth = depth
        });
        if (locks_enabled) {
            spin_unlock(&chunk_below_queue->lock);
        }
    }
    return dirty;
}

// Queued side updates for propogation
void light_beam_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    const uint max_beams = 256;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(LightLock);
    zox_sys_out(SunlightQueue);
    zox_sys_out(LightQueue);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    entity realm = 0;
    byte solidity[zox_light_solidity_length];
    memset(solidity, 1, zox_light_solidity_length);
    entity liquids_realm = 0;
    byte liquids[zox_light_solidity_length];
    memset(liquids, 0, zox_light_solidity_length);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(VoxelNode, root_vnode);
        zox_sys_o(LightLock, lightlock);
        zox_sys_o(SunlightQueue, sunlight_queue);
        zox_sys_o(LightQueue, floodlight_queue);
        zox_sys_o(LightNode, root_lights);
        zox_sys_o(LightNodeDirty, dirty);
        if (!sunlight_queue->count) {
            continue;
        }
        // NOTE: We wait for it to generate (its ground and water in), a beam through it before lit it as air
        if (zox_has(e, GenerateChunk) && zox_getv(e, GenerateChunk) &&
            zox_getv(e, GenerateChunk) < zox_generate_terrain_sunlight)
        {
            continue;
        }
        entity chunkd = neighbors->value[direction_down];
        if (!zox_has(e, BottomChunk)) {
            if (!zox_valid(chunkd)) {
                // NOTE: We wait for below chunk to load
                if (dbg_log) {
                    zox_log("Delaying Chunk [%s] as below chunk not valid", zox_get_name(e));
                }
                continue;
            }
        }
        // NOTE: Check Blocks Caches
        update_light_solidity(
            world,
            zox_get_link(world, e, BlockManagerLink),
            &realm,
            solidity);
        update_light_liquids(world, zox_get_link(world, e, BlockManagerLink), &liquids_realm, liquids);
        SunlightQueue* chunk_below_sunlight_queue =
            zox_valid(chunkd) ?
                zox_gett_mut(chunkd, SunlightQueue) :
                NULL;
        if (dbg_log) {
            zox_log("[%s] Extended Sunbeams l[%i]", zox_get_name(e), sunlight);
        }
        uint beams = 0;
        // sunlight_queue->count &&
        while (beams < max_beams) {
            spin_lock(&sunlight_queue->lock);
            if (!sunlight_queue->count) {
                spin_unlock(&sunlight_queue->lock);
                break;
            }
            SunlightUpdate update = remove_SunlightQueue(sunlight_queue);
            spin_unlock(&sunlight_queue->lock);
            // NOTE: For bottom chunk we just remove queue for beaming to bottom of earth
            byte3 pos = update.pos;
            short length = octree_size(update.depth);
            if (pos.x >= length || pos.z >= length || pos.y >= length) {
                //  zox_loge("[r_LightQueue] position oob [%ix%ix%i]", pos.x, pos.y, pos.z);
                continue;
            }
            if (dbg_log >= 2) {
                zox_log(" - Beaming [%ix%ix%i] l[%i] q [%i]", update.pos.x, update.pos.y, update.pos.z, update.light, sunlight_queue->count);
            }
            if (sunbeam(
                &lightlock->value,
                floodlight_queue,
                chunk_below_sunlight_queue,
                root_lights,
                root_vnode,
                update.depth,
                update.pos,
                update.light,
                darklight,
                solidity,
                liquids))
            {
                if (!dirty->value) {
                    if (dbg_log) {
                        zox_log("[%s] ChunkLights Updated at [%ix%ix%i] l[%i] q [%i] Depth [%i]", zox_get_name(e) , update.pos.x, update.pos.y, update.pos.z, update.light, sunlight_queue->count, update.depth);
                    }
                }
                dirty->value = zox_dirty_trigger;
            }
            beams++;
        }
    }
    zox_sys_on_end();
} zoxd_system(light_beam_system);

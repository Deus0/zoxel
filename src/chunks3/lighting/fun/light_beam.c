// NOTE: Theres an issue when we optimize, the light nodes can be set by the bigger nodes, so it was hard to debug

byte sunbeam(
    LightQueue* queued,
    LightNode* root_lnode,
    const VoxelNode* root_vnode,
    const byte depth,
    byte3 pos,
    const byte light,
    const VoxelNode* n_root_vnodes[6],   // (READ-ONLY)
    const LightNode* n_root_lnodes[6],   // (READ-ONLY)
    LightQueue* n_light_queues[6],         // (WRITE)
    byte min_light,
    byte air_decay,
    byte* solidity
) {
    byte length = powers_of_two[depth];
    if (pos.y >= length) {
        zox_logw("position too high [%i]", pos.y);
        return 0;
    }

    byte max_y = pos.y;
    byte flood_start = 0;
    byte flood_end = 0;
    byte beam_stopped = 0;

    for (byte y = 0; y <= max_y; y++) {
        pos.y = max_y - y;

        byte voxel = get_value_VoxelNode(root_vnode, depth, pos, 0);
        if (voxel && solidity[voxel - 1]) {
            // zox_log("sunbeam stopped v at [%ix%ix%i] v[%i]",  pos.x, pos.y, pos.z, voxel);
            beam_stopped = 1;
            break;
        }

        // set light in LightNode
        zox_log_lighting_light("+ SunLight [%i] Set at [%ix%ix%i]", light, pos.x, pos.y, pos.z);
        set_LightNode(root_lnode, depth, pos, light, 0);

        if (y == 0) {
            flood_end = pos.y;
        }
        flood_start = pos.y;
    }

    zox_log_lighting_light(" * light beam y: [%i] to [%i]", flood_start, (flood_end));

    for (byte y = flood_start; y <= flood_end; y++) {
        pos.y = y;

        zox_log_lighting_light(" - Light Beam Spreads [%ix%ix%i]", pos.x, pos.y, pos.z);

        // TODO: when we change light, we can save light to array, and reuse here

        flood_light(
            root_vnode,
            root_lnode,
            n_root_vnodes,
            n_root_lnodes,
            n_light_queues,
            depth,
            pos,
            light,
            light_propogation_distance,
            darklight,
            light_air_decay,
            solidity
        );
    }

    if (queued && !beam_stopped) {
        spin_lock(&queued->lock);
        a_LightQueue(queued, (LightUpdate) {
            .type = zox_light_type_beam,
            .pos = (byte3) {
                pos.x,
                length - 1,
                pos.z
            },
            .light = light,
            .depth = depth
        });
        spin_unlock(&queued->lock);
        return 1;
    }

    return 0;
}
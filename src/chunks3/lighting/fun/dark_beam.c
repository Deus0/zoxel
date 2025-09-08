byte dark_sunbeam(
    DarkQueue* queued,
    const VoxelNode* root_vnode,
    LightNode* root_lnode,
    const VoxelNode* n_root_vnodes[6],
    const LightNode* n_root_lnodes[6],
    LightQueue* n_light_queues[6],
    LightQueue* light_queue,
    DarkQueue* n_dark_queues[6],
    DarkQueue* dark_queue,
    byte depth,
    byte3 pos,
    byte sunlight,
    byte min_light,
    byte air_decay,
    byte type,
    byte* solidity
) {
    byte dirty = 0;

    byte length = powers_of_two[depth];
    if (pos.y > length) {
        return dirty;
    }

    byte max_y = pos.y;
    byte flood_start = 0;
    byte flood_end = 0;
    byte beam_stopped = 0;

    for (byte y = 0; y < max_y; y++) {
        pos.y = max_y - y;

        if (type == zox_light_type_beam_start && y == 0) {
            flood_end = pos.y;
            continue;
        }

        byte voxel = get_value_VoxelNode(root_vnode, depth, pos, 0);
        if (voxel && solidity[voxel - 1]) {
            // zox_log("sunbeam stopped v at [%ix%ix%i] v[%i]",  pos.x, pos.y, pos.z, voxel);
            beam_stopped = 1;
            break;
        }

        // extinguish sunlight here
        byte current_light = get_value_LightNode(root_lnode, depth, pos, 0);
        if (current_light != sunlight) {
            // zox_log("sunbeam stopped l at %i", pos.y);
            beam_stopped = 1;
            break;
        }

        zox_log_lighting_dark("     - Light Banished at [%ix%ix%i] l[%i]", pos.x, pos.y, pos.z, current_light);

        set_LightNode(root_lnode, depth, pos, min_light, 0);
        dirty = 1;

        if (y == 0) {
            flood_end = pos.y;
        }
        flood_start = pos.y;

    }

    // flood after, otherwise is refills with sunlight
    // QUESTION: Dark DarkFlood use neighbor.. Probably?!?!
    //      can we delay these until its gone? test sunlight between chunks, sunlight comes through chunk + check it dissapears

    zox_log_lighting_dark(" * dark beam y: [%i] to [%i]", flood_start, flood_end);

    for (byte y = flood_start; y <= flood_end; y++) {
        pos.y = y;

        zox_log_lighting_dark(" - Dark Beam Spreads [%ix%ix%i]", pos.x,  pos.y, pos.z);

        if (dark_flood_light(
            root_vnode,
            root_lnode,
            n_root_vnodes,
            n_root_lnodes,
            n_light_queues,
            light_queue,
            n_dark_queues,
            dark_queue,
            depth,
            pos,
            sunlight - air_decay + 1,
            darklight_propogation_distance,
            min_light,
            air_decay,
            solidity
        )) {
            dirty = 1;
        }
    }

    // pass downward into chunk below since we survived until the end
    if (!beam_stopped && queued) {

        zox_log_lighting_dark(" - Dark Beam Continues [%ix%ix%i]", pos.x, length, pos.z);

        spin_lock(&queued->lock);
        a_DarkQueue(
            queued,
            (DarkUpdate) {
                .type = zox_light_type_beam,
                .pos = (byte3) {
                    pos.x,
                    length,   // y = bottom edge
                    pos.z
                },
                .depth = depth,
                .light = sunlight
        });
        spin_unlock(&queued->lock);
    }

    return dirty;
}

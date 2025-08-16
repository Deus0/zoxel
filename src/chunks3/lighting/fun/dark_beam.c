byte dark_sunbeam(
    SunlightQueue* queued,                // 👈 like sunbeam, for chunk below
    const VoxelNode* root_vnode,          // READ
    LightNode* root_lnode,                // WRITE
    const VoxelNode* n_root_vnodes[6],    // READ
    const LightNode* n_root_lnodes[6],    // READ
    PropogateQueue* n_queues[6],          // WRITE
    PropogateQueue* propogation_queue,
    byte depth,
    byte3 pos,
    byte sunlight,                       // sunlight level we’re extinguishing (usually 255)
    byte min_light,
    byte air_decay
) {
    byte length = powers_of_two[depth];
    if (pos.y > length) return 0;

    byte y_max = pos.y;
    byte flood_start = 0;
    byte flood_end = 0;

    byte beam_stopped = 0;
    for (byte y = 0; y < y_max; y++) {
        pos.y = y_max - 1 - y;

        byte voxel = get_value_VoxelNode(root_vnode, depth, pos, 0);
        if (voxel) {
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

        // zox_log("dark beam setting at [%ix%ix%i] l [%i]", pos.x, pos.y, pos.z, min_light);
        // for sun, we extinquish and dark flood it
        set_LightNode(root_lnode, depth, pos, min_light, 0);
        if (y == 0) {
            flood_end = pos.y;
        }
        flood_start = pos.y;

    }

    // flood after, otherwise is refills with sunlight
    // QUESTION: Dark DarkFlood use neighbor.. Probably?!?!
    //      can we delay these until its gone? test sunlight between chunks, sunlight comes through chunk + check it dissapears

    // zox_log("dark beam -> dark flooding: [%i] to [%i]", flood_start, flood_end);

    for (byte y = flood_start; y <= flood_end; y++) {
        pos.y = y;
        dark_flood_light(
            root_vnode,
            root_lnode,
            n_root_vnodes,
            n_root_lnodes,
            n_queues,
            propogation_queue,
            depth,
            pos,
            sunlight - air_decay + 1,
            darklight_propogation_distance,
            min_light,
            air_decay
        );
    }

    // pass downward into chunk below since we survived until the end
    if (!beam_stopped && queued) {
        spin_lock(&queued->lock);
        a_SunlightQueue(queued, (SunlightUpdate) {
            .type = 1,
            .pos = (byte3) {
                pos.x,
                length,   // y = bottom edge
                pos.z
            },
            .light = sunlight
        });
        spin_unlock(&queued->lock);
        return 1;
    }

    return 0;
}

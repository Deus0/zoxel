// NOTE: Theres an issue when we optimize, the light nodes can be set by the bigger nodes, so it was hard to debug

byte sunbeam(
    SunlightQueue* queued,
    LightNode* lnode,
    const VoxelNode* root_vnode,
    const byte depth,
    byte3 pos,
    byte beamlight,
    byte type
) {
    byte length = powers_of_two[depth];
    if (pos.y > length) {
        zox_logw("position too high [%i]", pos.y);
        return 0;
    }

    byte hit_solid = 0;
    byte light = beamlight;
    byte length2 = pos.y;
    byte flood_start = 0;
    byte flood_end = 0;
    byte beam_stopped = 0;

    for (byte y = 0; y < length2; y++) {
        pos.y = length2 - 1 - y;

        byte voxel = get_value_VoxelNode(root_vnode, depth, pos, 0);
        if (voxel) {
            // zox_log("sunbeam stopped v at [%ix%ix%i] v[%i]",  pos.x, pos.y, pos.z, voxel);
            beam_stopped = 1;
            break;
        }

        // check if not hit
        /*if (!hit_solid) {

            const VoxelNode* check_node = get_VoxelNode(
                vnode,
                depth,
                pos,
                0);

            if (check_node) {
                byte voxel = check_node->value;
                if (voxel) {
                    if (type == 2) {
                        break;
                    }
                    hit_solid = 1;
                    light = darklight;
                    // zox_log("+ Light Hit Block at [%ix%ix%i]",  pos.x, pos.y, pos.z);
                }
            } else {
                hit_solid = 1;
                light = darklight; // sunlight blocked
            }
        }*/

        // set light in LightNode
        zox_log_lighting_light("+ SunLight [%i] Set at [%ix%ix%i]", light, pos.x, pos.y, pos.z);
        set_LightNode(lnode, depth, pos, light, 0);

        if (y == 0) {
            flood_end = pos.y;
        }
        flood_start = pos.y;
    }


    zox_log_lighting_light(" * light beam y: [%i] to [%i]", flood_start, (flood_end));

    for (byte y = flood_start; y <= flood_end; y++) {
        pos.y = y;

        zox_log_lighting_light(" - Light Beam Spreads [%ix%ix%i]", pos.x,  pos.y, pos.z);

        /*flood_light(
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
            air_decay
        );*/
    }

    // only need check for non batch
    //  NOTE: Batch Sunlight will set darklight for all belows
    if (queued && !beam_stopped) {
        // ((type == 2 && !hit_solid) || type == 0)) {
        // for now we assume chunk below has same depth
        a_SunlightQueue(queued, (SunlightUpdate) {
            .type = type,
            .pos = (byte3) {
                pos.x,
                length,
                pos.z
            },
            .light = light
        });
        return 1;
    }

    return 0;
}

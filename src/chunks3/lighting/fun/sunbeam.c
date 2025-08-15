// NOTE: Theres an issue when we optimize, the light nodes can be set by the bigger nodes, so it was hard to debug

byte sunbeam(
    SunlightQueue* queued,
    LightNode* lnode,
    const VoxelNode* vnode,
    const byte depth,
    byte3 pos,
    byte beamlight
) {
    byte length = powers_of_two[depth];
    if (pos.y > length) {
        zox_logw("position too high [%i]", pos.y);
        return 0;
    }/* else if (!pos.y) {
        pos.y = length; // hmm
    }*/
    byte hit_solid = 0;
    byte light = beamlight;
    byte length2 = pos.y;
    for (byte y = 0; y < length2; y++) {
        pos.y = length2 - 1 - y;

        // check if not hit
        if (!hit_solid) {

            const VoxelNode* check_node = get_VoxelNode_ex(
                vnode,
                depth,
                pos,
                0);

            if (check_node) {
                byte voxel = check_node->value;
                if (voxel) {
                    hit_solid = 1;
                    light = darklight;
                    // zox_log("+ Light Hit Block at [%ix%ix%i]",  pos.x, pos.y, pos.z);
                }
            } else {
                hit_solid = 1;
                light = darklight; // sunlight blocked
            }
        }

        // set light in LightNode
        // zox_log("+ Light [%i] Set at [%ix%ix%i]", light, pos.x, pos.y, pos.z);
        set_LightNode_ex(
            lnode,
            depth,
            pos,
            light,
            0);
    }
    if (queued) { // !hit_solid &&
        // for now we assume chunk below has same depth
        a_SunlightQueue(queued, (SunlightUpdate) {
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

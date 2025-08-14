// Progress ray in a sunlight direction, stops when solid, decreases when liquid
// TODO: Optimize LightNode System - group same values
// WORKS: TODO: Fix Updates tho

void SunlightSystem(iter *it) {
    byte sunlight = 255; // full sunlight
    byte darklight = 64; // full sunlight
    byte midlight = 225; // full sunlight

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(VoxelNode);
    zox_sys_out(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_i(RenderDepth, depthr);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_o(LightNode, lnode);
        zox_sys_o(LightNodeDepth, depthl);
        zox_sys_o(LightNodeDirty, updated);

        // sunlight uses RenderDepthDirty
        if (dirty->value != zox_dirty_active) {
            // continue;
        }
        // we skip if already at right depth
        if (depthl->value >= depthr->value) {
            // zox_log("Skip Updating lights");
            // continue;
        }
        depthl->value = depthr->value;
        // lnode->value = sunlight;

        // now for all XZ places we go through
        byte length = powers_of_two[depthl->value];
        for (byte x = 0; x < length; x++) {
            for (byte z = 0; z < length; z++) {

                // now we progress down
                byte light = sunlight; // full sunlight
                byte last = 0;
                for (byte y = 0; y < length; y++) {
                    byte3 positionl = (byte3) { x, length - 1 - y, z };

                    if (last) {   // debugging atm with last_voxel
                        light = darklight; // sunlight blocked
                    }
                    if (light != darklight) {

                        const VoxelNode* check_node = get_VoxelNode_ex(
                            vnode,
                            depthl->value,
                            positionl,
                            0);

                        if (check_node) {
                            byte voxel = check_node->value;
                            if (voxel) {
                                light = midlight;
                                last = voxel;
                            }
                        } else {
                            light = darklight; // sunlight blocked
                            zox_log_error("null return at [%ix%ix%i]", positionl.x, positionl.y, positionl.z);
                        }
                    }

                    // set light in LightNode
                    set_LightNode_ex(lnode, depthl->value, positionl, light, 0);
                }
            }
        }

        // zox_log("Updating lights to Depth [%i]:", depthl->value);
        // print_octree_layer(lnode, depthl->value, LightNode);

        updated->value = zox_dirty_trigger;
    }
} zoxd_system2(SunlightSystem);
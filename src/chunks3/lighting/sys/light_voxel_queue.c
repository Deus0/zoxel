//  Using the VoxelUpdateQueue, we add to the sunbeams and propogation
// for now: check above voxel, if sun, create beam

void VoxelLightSystem(ecs_iter_t *it) {

    zox_sys_world();
    zox_sys_begin();

    zox_sys_in(VoxelNodeQueue);
    zox_sys_in(RenderDepth);
    zox_sys_in(LightNode);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkNeighbors);

    zox_sys_out(SunlightQueue);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNodeQueue, input_queue);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(LightNode, root_lnode);
        zox_sys_i(VoxelNode, vnode);
        zox_sys_i(ChunkNeighbors, neighbors);

        zox_sys_o(SunlightQueue, sun_queue);

        if (!input_queue->count) {
            continue;
        }

        const LightNode* nnodesl[6];
        fetch_neightbor_light_nodes(
            world,
            neighbors,
            nnodesl);

        for (size_t i = 0; i < input_queue->count; i++) {
            VoxelNodeUpdate update = g_VoxelNodeQueue(input_queue, i);

            if (update.value == 0) {

                const LightNode* above = get_LightNode_neighbor(
                    root_lnode,
                    nnodesl,
                    direction_up,
                    update.positionl,
                    depth->value
                );
                const byte light = above ? above->value : 0;

                if (light == sunlight) {
                    // zox_log("Removed block with sun above");
                    // if y, we do y + 1
                    a_SunlightQueue(sun_queue, (SunlightUpdate) {
                        .pos = (byte3) {
                            update.positionl.x,
                            update.positionl.y + 1,
                            update.positionl.z
                        },
                        .light = sunlight
                    });
                }
            }

            // if filling hole
            if (update.value) {

                const LightNode* above = get_LightNode_neighbor(
                    root_lnode,
                    nnodesl,
                    direction_up,
                    update.positionl,
                    depth->value
                );
                const byte light = above ? above->value : 0;

                if (light == sunlight) {
                    // zox_log("Removed block with sun above");
                    // if y, we do y + 1
                    a_SunlightQueue(sun_queue, (SunlightUpdate) {
                        .pos = (byte3) {
                            update.positionl.x,
                            update.positionl.y,
                            update.positionl.z
                        },
                        .light = darklight
                    });
                }

            }
        }
    }
} zoxd_system2(VoxelLightSystem);
//  Using the VoxelUpdateQueue, we add to the sunbeams and propogation
// for now: check above voxel, if sun, create beam
// TODO: Convert local position to terrain position (positionv)

void VoxelLightSystem(ecs_iter_t *it) {

    zox_sys_world();
    zox_sys_begin();

    zox_sys_in(VoxelNodeQueue);
    zox_sys_in(RenderDepth);
    zox_sys_in(LightNode);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(SunlightQueue);
    zox_sys_out(PropogateQueue);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNodeQueue, input_queue);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(LightNode, root_lnode);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(SunlightQueue, sun_queue);
        zox_sys_o(PropogateQueue, propogation_queue);

        if (!input_queue->count) {
            continue;
        }

        const LightNode* nnodesl[6];
        fetch_neightbor_light_nodes(
            world,
            neighbors,
            nnodesl);

        for (size_t i = 0; i < input_queue->count; i++) {

            VoxelNodeUpdate update = input_queue->ptr[i];

            if (update.value == 0) {

                const LightNode* above = get_neighbor_LightNode(
                    root_lnode,
                    nnodesl,
                    direction_up,
                    update.positionl,
                    depth->value
                );
                const byte light_above = above ? above->value : 0;

                if (light_above == sunlight) {
                    // zox_log("Removed block with sun above");
                    // if y, we do y + 1
                    a_SunlightQueue(sun_queue, (SunlightUpdate) {
                        .pos = (byte3) {
                            update.positionl.x,
                            update.positionl.y + 1,
                            update.positionl.z
                        },
                        .light = light_above
                    });
                } else {

                    byte max_nlight = light_above;
                    for (byte dir = 0; dir < 6; dir++) {
                        if (dir == direction_up) continue;
                        const LightNode* lnnode = get_neighbor_LightNode(
                            root_lnode,
                            nnodesl,
                            dir,
                            update.positionl,
                            depth->value
                        );
                        const byte nlight = lnnode ? lnnode->value : 0;
                        if (nlight > max_nlight) max_nlight = nlight;
                    }

                    byte new_light = (max_nlight > light_air_decay) ? (byte) (max_nlight - light_air_decay) : darklight;

                    a_PropogateQueue(
                        propogation_queue,
                        (PropogateUpdate) {
                            .type = 0,
                            .pos = update.positionl,
                            .depth = depth->value,
                            .light = new_light,
                            .distance = light_propogation_distance
                    });
                }
            }

            // if filling hole
            else if (update.value) {

                // TODO: We can add a dark beam here instead
                //          - It needs to call dark floodfill on all points along


                const LightNode* above = get_neighbor_LightNode(
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

                // Reverse Flood
                const LightNode* removed_lnode = get_LightNode_ex(
                    root_lnode,
                    depth->value,
                    update.positionl,
                    0
                );
                byte removed_light = removed_lnode ? removed_lnode->value : 0;
                if (removed_light > darklight) {
                    // removed_light -= light_air_decay;
                    // zox_log("Reverse Light Flood +Queue [%i]", removed_light);
                    a_PropogateQueue(
                        propogation_queue,
                        (PropogateUpdate) {
                            .type = 1,
                            .pos = update.positionl,
                            .depth = depth->value,
                            .light = removed_light,
                            .distance = darklight_propogation_distance
                    });
                }
            }
        }
    }
} zoxd_system2(VoxelLightSystem);
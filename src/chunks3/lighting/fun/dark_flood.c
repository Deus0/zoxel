static inline void dark_flood_light(
    const VoxelNode* root_vnode,          // (READ)
    LightNode* root_lnode,                // (WRITE)
    const VoxelNode* n_root_vnodes[6],    // (READ-ONLY)
    const LightNode* n_root_lnodes[6],    // (READ-ONLY)
    LightQueue* n_light_queues[6],
    LightQueue* light_queue,
    DarkQueue* n_dark_queues[6],
    DarkQueue* dark_queue,
    byte depth,
    byte3 positionl,
    byte old_light,                       // the light we’re extinguishing
    byte distance,
    byte min_light,
    byte air_decay,
    byte* solidity
) {
    if (!root_vnode || !root_lnode || distance == 0 || old_light <= min_light) {
        return;
    }

    const byte SIZE = (byte)((1u << depth) - 1u);

    for (byte dir = 0; dir < 6; dir++) {
        byte3 pos = positionl;
        byte oob = 0;

        // move with wrap
        switch (dir) {
            case 0: if (pos.x > 0) pos.x--; else { pos.x = SIZE; oob = 1; } break;
            case 1: if (pos.x < SIZE) pos.x++; else { pos.x = 0;    oob = 1; } break;
            case 2: if (pos.y > 0) pos.y--; else { pos.y = SIZE; oob = 1; } break;
            case 3: if (pos.y < SIZE) pos.y++; else { pos.y = 0;    oob = 1; } break;
            case 4: if (pos.z > 0) pos.z--; else { pos.z = SIZE; oob = 1; } break;
            case 5: if (pos.z < SIZE) pos.z++; else { pos.z = 0;    oob = 1; } break;
        }

        if (oob) {
            // --- neighbor chunk ---
            const VoxelNode* n_root_vnode   = n_root_vnodes[dir];
            const LightNode* n_root_lnode = n_root_lnodes[dir];
            if (!n_root_lnode) {
                continue;
            }

            if (n_root_vnode) {
                byte voxel = get_value_VoxelNode(n_root_vnode, depth, pos, 0);
                if (voxel && solidity[voxel - 1]) {
                    continue; // solid wall
                }
            }

            byte ncurrent_light = get_value_LightNode(n_root_lnode, depth, pos, 0);
            if (ncurrent_light <= min_light) {
                continue;   // omg this wasn't here
            }

            // wait we never set light for dark flood on neighbors

            // enqueue only
            byte is_darkness = (ncurrent_light < old_light);

            // zox_log("dark flood at border [%ix%ix%i] dist [%i] d?[%i]", pos.x, pos.y, pos.z, distance, is_darkness);

            if (is_darkness) {

                DarkQueue* nqueue = n_dark_queues[dir];
                if (nqueue) {
                    spin_lock(&nqueue->lock);
                    a_DarkQueue(nqueue, (DarkUpdate) {
                        .type  = 0,
                        .light = old_light,
                        .distance = distance - 1,
                        .pos   = pos,
                        .depth = depth
                    });
                    spin_unlock(&nqueue->lock);
                }

            } else {

                LightQueue* nqueue = n_light_queues[dir];
                if (nqueue) {
                    spin_lock(&nqueue->lock);
                    a_LightQueue(nqueue, (LightUpdate) {
                        .type  = zox_light_type_flood,
                        .light = ncurrent_light,
                        .distance = light_propogation_distance,
                        .pos   = pos,
                        .depth = depth
                    });
                    spin_unlock(&nqueue->lock);
                }

            }

            continue;
        }

        // --- in-chunk ---
        byte voxel = get_value_VoxelNode(root_vnode, depth, pos, 0);
        if (voxel) {
            continue; // solid → stop
        }

        byte current_light = get_value_LightNode(root_lnode, depth, pos, 0);
        if (current_light <= min_light) {
            continue;   // omg this wasn't here
        }

        if (current_light < old_light) {

            zox_log_lighting_dark("     - Light Banished at [%ix%ix%i] l[%i] dist[%i]", pos.x, pos.y, pos.z, old_light, distance);

            // extinguish here and continue removing
            set_LightNode(root_lnode, depth, pos, min_light, 0);

            dark_flood_light(
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
                old_light,
                distance - 1,
                min_light,
                air_decay,
                solidity
            );

        } else {

            if (light_queue) {
                byte decayed_light = (current_light > light_air_decay) ? (byte) (current_light - light_air_decay) : darklight;

                zox_log_lighting_dark("     + Dark Flood Light Source [%ix%ix%i] new [%i] old [%i] decayed [%i]", pos.x, pos.y, pos.z, current_light, old_light, decayed_light);

                spin_lock(&light_queue->lock);
                a_LightQueue(light_queue, (LightUpdate) {
                    .type  = zox_light_type_flood,
                    .light = decayed_light,
                    .distance = light_propogation_distance,
                    .pos   = pos,
                    .depth = depth
                });
                spin_unlock(&light_queue->lock);
            }
        }
    }
}
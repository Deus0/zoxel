// -> for each neighbor, we have 6
//      -> If out of bounds:
//          ->wrap position, add to queue, we can get the light and voxel though to check if meets conditions
//      -> otherwise
//          -> check if air
//              -> if air we check light
//                  -> if light less than current (decayed light) we set it and propogate there
//          -> if not air, i.e. solid, of course light doesnt go through solid you twat
// NOTE: Stop setting neighbor data, thats const, you are literally corrupting memory

static inline void flood_light(
    const VoxelNode* root_vnode,         // (READ)
    LightNode* root_lnode,               // (WRITE)
    const VoxelNode* n_root_vnodes[6],   // (READ-ONLY)
    const LightNode* n_root_lnodes[6],   // (READ-ONLY)
    LightQueue* n_queues[6],         // (WRITE)
    byte depth,
    byte3 positionl,
    byte light,
    byte distance,
    byte min_light,
    byte air_decay
) {
    if (!root_vnode || !root_lnode || distance == 0 || light <= min_light) {
        return;
    }

    const byte length = (byte)((1u << depth) - 1u);

    for (byte dir = 0; dir < 6; dir++) {
        byte3 pos = positionl;
        byte oob = 0;

        // move with wrap into neighbor-local coords
        switch (dir) {
            case 0: if (pos.x > 0) pos.x--; else { pos.x = length; oob = 1; } break;
            case 1: if (pos.x < length) pos.x++; else { pos.x = 0;    oob = 1; } break;
            case 2: if (pos.y > 0) pos.y--; else { pos.y = length; oob = 1; } break;
            case 3: if (pos.y < length) pos.y++; else { pos.y = 0;    oob = 1; } break;
            case 4: if (pos.z > 0) pos.z--; else { pos.z = length; oob = 1; } break;
            case 5: if (pos.z < length) pos.z++; else { pos.z = 0;    oob = 1; } break;
        }

        if (oob) {
            // --- Cross-chunk: READ neighbor if present, never write it. Queue only. ---
            const VoxelNode* nvox_root  = n_root_vnodes[dir];
            const LightNode* nlight_root= n_root_lnodes[dir];

            // solid → no propagation
            if (nvox_root) {
                byte nvoxel = get_value_VoxelNode(nvox_root, depth, pos, 0);
                if (nvoxel) {
                    continue; // solid: hard stop
                }
            }

            // air decay
            byte decayed_light = (light > air_decay) ? (byte)(light - air_decay) : 0;
            if (decayed_light <= min_light) {
                continue;
            }

            // only queue if it improves neighbor
            byte current_light = nlight_root ? get_value_LightNode(nlight_root, depth, pos, 0) : 0;
            if (decayed_light <= current_light) {
                continue;
            }

            LightQueue* nqueue = n_queues[dir];
            // Add to neighbor queue
            if (nqueue) {
                spin_lock(&nqueue->lock);
                a_LightQueue(nqueue,
                    (LightUpdate) {
                        .type = zox_light_type_flood,
                        .light = decayed_light,
                        .pos = pos,
                        .depth = depth,
                        .distance = distance - 1
                    });
                spin_unlock(&nqueue->lock);
            }

            continue;
        }

        // --- In-chunk: READ voxel, WRITE light in our own chunk only. ---
        byte voxel = get_value_VoxelNode(root_vnode, depth, pos, 0);
        if (voxel) {
            continue;
        }

        byte decayed_light = (light > air_decay) ? (byte) (light - air_decay) : min_light;

        byte current_light = get_value_LightNode(root_lnode, depth, pos, 0);
        if (decayed_light <= current_light) {
            continue;
        }

        zox_log_lighting_light("     + Light Flooded [%ix%ix%i] l[%i] dist[%i]", pos.x, pos.y, pos.z, decayed_light, distance);

        set_LightNode(root_lnode, depth, pos, decayed_light, 0);

        flood_light(
            root_vnode,
            root_lnode,
            n_root_vnodes,
            n_root_lnodes,
            n_queues,
            depth,
            pos,
            decayed_light,
            distance - 1,
            min_light,
            air_decay
        );
    }
}

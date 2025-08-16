static inline void flood_light_remove(
    const VoxelNode* root_vnode,          // (READ)
    LightNode* root_lnode,                // (WRITE)
    const VoxelNode* n_root_vnodes[6],    // (READ-ONLY)
    const LightNode* n_root_lnodes[6],    // (READ-ONLY)
    PropogateQueue* n_queues[6],          // (WRITE)
    byte depth,
    byte3 positionl,
    byte old_light,                       // the light we’re extinguishing
    byte distance,
    byte min_light,
    byte air_decay
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
            const VoxelNode* nvox_root   = n_root_vnodes[dir];
            const LightNode* nlight_root = n_root_lnodes[dir];
            if (!nlight_root) continue;

            if (nvox_root) {
                byte v = get_VoxelNode_value_ex(nvox_root, depth, pos, 0);
                if (v) continue; // solid wall
            }

            byte ncur = get_LightNode_value_ex(nlight_root, depth, pos, 0);

            PropogateQueue* nqueue = n_queues[dir];
            if (!nqueue) continue;

            // enqueue only
            spin_lock(&nqueue->lock);
            a_PropogateQueue(nqueue, (PropogateUpdate) {
                .pos   = pos,
                .depth = depth,
                .type  = (ncur < old_light) ? 1 : 0,
                .light = (ncur < old_light) ? old_light : ncur,
                .distance = (ncur <= old_light) ? distance - 1 : light_propogation_distance
            });
            spin_unlock(&nqueue->lock);

            continue;
        }

        // --- in-chunk ---
        byte v = get_VoxelNode_value_ex(root_vnode, depth, pos, 0);
        if (v) continue; // solid → stop

        byte cur = get_LightNode_value_ex(root_lnode, depth, pos, 0);
        if (cur == 0) continue;

        if (cur < old_light) {
            // extinguish here and continue removing
            set_LightNode_ex(root_lnode, depth, pos, min_light, 0);

            flood_light_remove(
                root_vnode,
                root_lnode,
                n_root_vnodes,
                n_root_lnodes,
                n_queues,
                depth,
                pos,
                old_light,
                distance - 1,
                min_light,
                air_decay
            );

        } else {
            // survivor → re-flood from this brighter cell
            flood_light(
                root_vnode,
                root_lnode,
                n_root_vnodes,
                n_root_lnodes,
                n_queues,
                depth,
                pos,
                cur,
                light_propogation_distance,
                min_light,
                air_decay
            );
        }
    }
}
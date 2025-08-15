static inline void flood_light_recursive(
    const VoxelNode* vnode,                 // our chunk's voxel root (READ)
    LightNode* lnode,                       // our chunk's light root (WRITE)
    const VoxelNode* voxel_neighbors[6],    // neighbor voxel roots (READ-ONLY)
    const LightNode* light_neighbors[6],    // neighbor light roots (READ-ONLY)
    byte depth,
    byte3 pos,                              // current position (in our chunk coords)
    byte light,                             // incoming light already applied at pos
    PropogateQueue* queue,                  // cross-chunk queue (WRITE)
    int max_depth
) {
    if (!vnode || !lnode || light == 0 || max_depth <= 0) return;

    const byte SIZE = (byte)((1u << depth) - 1u);
    const byte AIR_DECAY   = 32;
    // SOLID blocks block entirely; no partial bleed.

    for (byte dir = 0; dir < 6; dir++) {
        byte3 npos = pos;
        bool oob = false;

        // move with wrap into neighbor-local coords
        switch (dir) {
            case 0: if (npos.x > 0) npos.x--; else { npos.x = SIZE; oob = true; } break;
            case 1: if (npos.x < SIZE) npos.x++; else { npos.x = 0;    oob = true; } break;
            case 2: if (npos.y > 0) npos.y--; else { npos.y = SIZE; oob = true; } break;
            case 3: if (npos.y < SIZE) npos.y++; else { npos.y = 0;    oob = true; } break;
            case 4: if (npos.z > 0) npos.z--; else { npos.z = SIZE; oob = true; } break;
            case 5: if (npos.z < SIZE) npos.z++; else { npos.z = 0;    oob = true; } break;
        }

        if (oob) {
            // --- Cross-chunk: READ neighbor if present, never write it. Queue only. ---
            const VoxelNode* nvox_root  = voxel_neighbors[dir];
            const LightNode* nlight_root= light_neighbors[dir];

            // solid → no propagation
            if (nvox_root) {
                byte v = get_VoxelNode_value_ex(nvox_root, depth, npos, 0);
                if (v) continue; // solid: hard stop
            }

            // air decay
            byte new_light = (light > AIR_DECAY) ? (byte)(light - AIR_DECAY) : 0;
            if (new_light == 0) continue;

            // only queue if it improves neighbor
            byte ncur = nlight_root ? get_LightNode_value_ex(nlight_root, depth, npos, 0) : 0;
            if (new_light <= ncur) continue;

            if (queue) a_PropogateQueue(queue, (PropogateUpdate){ .value = new_light, .positionl = npos });
            continue;
        }

        // --- In-chunk: READ voxel, WRITE light in our own chunk only. ---
        byte v = get_VoxelNode_value_ex(vnode, depth, npos, 0);
        if (v) continue; // solid: no light through, you said it.

        byte new_light = (light > AIR_DECAY) ? (byte)(light - AIR_DECAY) : 0;
        if (new_light <= darklight) continue; // << added min-light check

        byte cur = get_LightNode_value_ex(lnode, depth, npos, 0);
        if (new_light <= cur) continue;

        set_LightNode_ex(lnode, depth, npos, new_light, 0); // write ONLY our chunk
        flood_light_recursive(
            vnode,                // still our voxel root
            lnode,                // still our light root
            voxel_neighbors,
            light_neighbors,
            depth,
            npos,
            new_light,
            queue,
            max_depth - 1
        );
    }
}

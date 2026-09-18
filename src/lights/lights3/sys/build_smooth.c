// hmmm issue seems to be about faces
// maybe we redo our mesh builder system

// if corner light AND two adjacents are solid voxels, return 0
#define zox_get_nearby_light(x, y, z) \
    getv_nearby_LightNode( \
        lights, \
        lights_locks, \
        position, \
        depth, \
        sbyte3_add( \
            neighbor_offsets[direction], \
            (sbyte3) { x, y, z } \
        ), \
        oob_value)

static inline void zox_apply_smooth_lights(
    spinlock** lights_locks,
    spinlock** voxels_locks,
    const LightNode** lights,
    const VoxelNode** voxels,
    const SidesOctree* sides,
    const MeshColorRGBs* colors,
    byte3 position,
    uint* ccount,
    byte target_depth,
    byte depth)
{
    const SidesOctree* sides_kids =
        (const SidesOctree*) sides->ptr;
    byte has_sides_kids = sides_kids != NULL;
    // NOTE: Voxels passed in for later adjacent checks
    // spinlock* root_lights_lock = lights_locks[OCTREE_INDEX(0, 0, 0)];
    // spinlock* root_voxels_lock = voxels_locks[OCTREE_INDEX(0, 0, 0)];
    // Dig Deeper
    if (depth < target_depth && has_sides_kids) {
        /*spin_lock(root_voxels_lock);
        byte has_vkids = !is_closed_VoxelNode(voxels);
        spin_unlock(root_voxels_lock);*/
        byte3_multiply_byte(&position, 2);
        depth++;
        for (byte i = 0; i < 8; i++) {
            /*spin_lock(root_voxels_lock);
            const VoxelNode* child_voxels = has_vkids ?
                &((const VoxelNode*) voxels->ptr)[i] :
                voxels;
            spin_unlock(root_voxels_lock);*/
            byte3 child_position = byte3_add(
                position,
                octree_positions[i]);
            zox_apply_smooth_lights(
                lights_locks,
                voxels_locks,
                lights,
                voxels, // child_voxels,
                &sides_kids[i],
                colors,
                child_position,
                ccount,
                target_depth,
                depth);
//#ifdef zox_safety_checks
            if (*ccount + voxel_face_vertices_length > colors->length) {
                break;
            }
// #endif
        }
        return;
    }
    // NOTE: If no sides were rendered
    byte sides_visible = sides->value;
    if (!sides_visible) {
        return;
    }
    /*spin_lock(root_lock);
    byte is_air = !voxels->value;
    spin_unlock(root_lock);
    if (!voxels->value) {
        return;
    }*/
    // TODO: Handle Corners Properly:
    //      - If two adjacent voxels solid, dont use corner light as it will be blocked
    // for each face that is visible according to node->sides
    for (byte direction = 0; direction < 6; direction++) {
        // skip hidden face
        byte side_visible = (sides->value & (1 << (direction + 1)));
        if (!side_visible) {
            continue;
        }
//#ifdef zox_safety_checks
        if (*ccount + voxel_face_vertices_length > colors->length) {
            zox_loge("Smooth Lights Error: Colors Past Limits [%i] - Face [%i] at Pos [%ix%ix%i] Depth [%i] of [%i]",
                *ccount,
                direction,
                position.x,
                position.y,
                position.z,
                depth,
                target_depth);
            return;
        }
//#endif
        const byte oob_values[] = {
            darklight,
            darklight,
            darklight,
            sunlight,
            darklight,
            darklight
        };
        byte oob_value = oob_values[direction];
        byte adjacent_light = zox_get_nearby_light(0, 0, 0);
        byte light_n1_0 = 0;
        byte light_1_0 = 0;
        byte light_0_n1 = 0;
        byte light_0_1 = 0;
        byte light_n1_n1 = 0;
        byte light_1_1 = 0;
        byte light_n1_1 = 0;
        byte light_1_n1 = 0;
        if (direction == direction_down ||
            direction == direction_up)
        {
            // Adjacents
            light_n1_0 = zox_get_nearby_light(-1, 0, 0);
            light_1_0 = zox_get_nearby_light(1, 0, 0);
            light_0_n1 = zox_get_nearby_light(0, 0, -1);
            light_0_1 = zox_get_nearby_light(0, 0, 1);
            light_n1_n1 = zox_get_nearby_light(-1, 0, -1);
            light_1_1 = zox_get_nearby_light(1, 0, 1);
            light_n1_1 = zox_get_nearby_light(-1, 0, 1);
            light_1_n1 = zox_get_nearby_light(1, 0, -1);
        } else if (direction == direction_back ||
            direction == direction_front)
        {
            // Adjacents
            light_n1_0 = zox_get_nearby_light(-1, 0, 0);
            light_1_0 = zox_get_nearby_light(1, 0, 0);
            light_0_n1 = zox_get_nearby_light(0, -1, 0);
            light_0_1 = zox_get_nearby_light(0, 1, 0);
            // Corners
            light_n1_n1 = zox_get_nearby_light(-1, -1, 0);
            light_1_1 = zox_get_nearby_light(1, 1, 0);
            light_n1_1 = zox_get_nearby_light(-1, 1, 0);
            light_1_n1 = zox_get_nearby_light(1, -1, 0);
        } else if (direction == direction_left ||
            direction == direction_right)
        {
            // Adjacents
            light_n1_0 = zox_get_nearby_light(0, -1, 0);
            light_1_0 = zox_get_nearby_light(0, 1, 0);
            light_0_n1 = zox_get_nearby_light(0, 0, -1);
            light_0_1 = zox_get_nearby_light(0, 0, 1);
            // Corners
            light_n1_n1 = zox_get_nearby_light(0, -1, -1);
            light_1_1 = zox_get_nearby_light(0, 1, 1);
            light_n1_1 = zox_get_nearby_light(0, -1, 1);
            light_1_n1 = zox_get_nearby_light(0, 1, -1);
        }
        for (byte v = 0; v < voxel_face_vertices_length; v++) {
            // Get other lights per vertex
            uint total_light = adjacent_light;
            if (v == 0) {
                // Negative Negative
                total_light += light_0_n1 + light_n1_0 + light_n1_n1;
            } else if (v == 1) {
                // Negative Positive
                total_light += light_0_1 + light_n1_0 + light_n1_1;
            } else if (v == 2) {
                // Positive Positive
                total_light += light_1_0 + light_0_1 + light_1_1;
            } else if (v == 3) {
                // Positive Negative
                total_light += light_0_n1 + light_1_0 + light_1_n1;
            }
            byte light = total_light / 4;
            color_rgb* c = &colors->value[*ccount];
            c->r = light;
            c->g = light;
            c->b = light;
            (*ccount)++;
        }
    }
}

// TODO: Add spinlock for SidesOctreeLock
// NOTE: Rebuilds Lights only when BuildMeshColors is dirty
void build_smooth_lights_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    byte max_process = !zox_disable_process_skips ? 1 : 0;
    if (!zox_smooth_lighting) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_in(MeshColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(MeshColorRGBs, colors);
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            break;
        }
        // Get chunk data
        entity chunk = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(chunk)) {
            zox_loge("Chunk Parent Invalid for [%s]", zox_getn(e));
            continue;
        }
#endif
        byte chunk_depth = zox_getv(chunk, NodeDepth);
        if (depth->value > chunk_depth) {
            if (dbg_log) {
                zox_log("Chunk Depth is wrong for Light Build [%s]",
                    zox_getn(chunk));
            }
            continue;
        }
        // If chunk building we wait
        if (zox_has(chunk, BuildChunkSides)) {
            if (dbg_log) {
                zox_log("Chunk [%s] is still Building Sides",
                    zox_getn(chunk));
            }
            continue;
        }
        const SidesOctree* sides = zox_get(chunk, SidesOctree);
        SidesOctreeLock* sides_lock = zox_mut(chunk, SidesOctreeLock);
        const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
        // const VoxelNode* root_voxels = zox_get(chunk, VoxelNode);
        entity nearby_chunks[27];
        const LightNode* lights[27];
        const VoxelNode* voxels[27];
        spinlock* lights_locks[27];
        spinlock* voxels_locks[27];
        fetch_nearby_chunks(
            world,
            chunk,
            neighbors->value,
            nearby_chunks);
        fetch_nearby_lights(
            world,
            nearby_chunks,
            lights,
            lights_locks);
        fetch_nearby_voxels(
            world,
            nearby_chunks,
            voxels,
            voxels_locks);
        uint ccount = 0;
        spin_lock(&sides_lock->value);
        zox_apply_smooth_lights(
            lights_locks,
            voxels_locks,
            lights,
            voxels,
            sides,
            colors,
            byte3_zero,
            &ccount,
            depth->value,
            0);
        spin_unlock(&sides_lock->value);
        zox_remove(e, BuildMeshColors);
        zox_add(e, MeshColorsDirty);
        /*if (ccount > colors->length) {
            zox_logw("Color Verts Missmatch: [%s] Found [%i] Colors [%i]", zox_get_name(e), ccount, colors->length);
        }*/
        zox_sys_increment();
    }
    zox_sys_on_end();
} zoxd_system(build_smooth_lights_system);

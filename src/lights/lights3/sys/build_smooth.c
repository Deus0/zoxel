// Smooth lighting: each vertex of a face is lit by the air around it, not the face's one light
//  - a vertex averages four lights: the face's (the voxel it looks into), its two edges' and its corner's
//  - a corner behind two solid edges is hidden, its light would leak round the corner, it counts as darklight
//  - solids hold darklight, so vertices in creases darken (ambient occlusion)
//  - walks the voxels (node) with the sides like the mesh builder (zox_terrain_building_dig), so both skip the
//    same faces (sides set on air), and colors line up with the mesh's vertices
//  - tst smooth_lights (the corner leak), terrain_render

// colors past limits errors, the mesh didn't match its chunk's sides (checked by tests)
uint zox_smooth_lights_errors = 0;

// a face's two axes along it, a vertex is one step along each (signed by smooth_vertex_signs)
static const sbyte3 smooth_face_axes[6][2] = {
    [direction_left] = { { 0, 1, 0 }, { 0, 0, 1 } },
    [direction_right] = { { 0, 1, 0 }, { 0, 0, 1 } },
    [direction_down] = { { 1, 0, 0 }, { 0, 0, 1 } },
    [direction_up] = { { 1, 0, 0 }, { 0, 0, 1 } },
    [direction_back] = { { 1, 0, 0 }, { 0, 1, 0 } },
    [direction_front] = { { 1, 0, 0 }, { 0, 1, 0 } },
};

// the mesh's vertex order on a face: --, -+, ++, +-
static const sbyte smooth_vertex_signs[4][2] = { { -1, -1 }, { -1, 1 }, { 1, 1 }, { 1, -1 } };

// a vertex's light: the face's, its edges' and its corner's averaged, a corner behind two solid edges is darklight
static inline byte smooth_vertex_light(byte face, byte edge_a, byte edge_b, byte corner, byte solid_a, byte solid_b) {
    if (solid_a && solid_b) {
        corner = darklight;
    }
    return (byte) ((face + edge_a + edge_b + corner) / 4);
}

// does the voxel at offset from position stop light (missing chunks are air)
static inline byte smooth_is_solid(
    const VoxelNode** voxels,
    spinlock** voxels_locks,
    const byte* solidity,
    byte3 position,
    byte depth,
    sbyte3 offset)
{
    const byte voxel = getv_nearby_VoxelNode(voxels, voxels_locks, position, depth, offset, 0);
    return voxel && solidity[voxel - 1];
}

static inline sbyte3 smooth_offset(sbyte3 face, sbyte3 a, sbyte sign_a, sbyte3 b, sbyte sign_b) {
    return (sbyte3) {
        face.x + a.x * sign_a + b.x * sign_b,
        face.y + a.y * sign_a + b.y * sign_b,
        face.z + a.z * sign_a + b.z * sign_b,
    };
}

// a face's four vertex lights, in the mesh's vertex order, for the voxel at position (27 nearby chunks)
void smooth_face_lights(
    const LightNode** lights,
    spinlock** lights_locks,
    const VoxelNode** voxels,
    spinlock** voxels_locks,
    const byte* solidity,
    byte3 position,
    byte depth,
    byte direction,
    byte* vertex_lights)
{
    // past the world: the sky above, dark elsewhere
    const byte oob = direction == direction_up ? sunlight : darklight;
    const sbyte3 face = neighbor_offsets[direction];
    const sbyte3 a = smooth_face_axes[direction][0];
    const sbyte3 b = smooth_face_axes[direction][1];
    const byte face_light = getv_nearby_LightNode(lights, lights_locks, position, depth, face, oob);
    for (byte v = 0; v < 4; v++) {
        const sbyte sa = smooth_vertex_signs[v][0], sb = smooth_vertex_signs[v][1];
        const sbyte3 edge_a = smooth_offset(face, a, sa, b, 0);
        const sbyte3 edge_b = smooth_offset(face, a, 0, b, sb);
        const sbyte3 corner = smooth_offset(face, a, sa, b, sb);
        vertex_lights[v] = smooth_vertex_light(
            face_light,
            getv_nearby_LightNode(lights, lights_locks, position, depth, edge_a, oob),
            getv_nearby_LightNode(lights, lights_locks, position, depth, edge_b, oob),
            getv_nearby_LightNode(lights, lights_locks, position, depth, corner, oob),
            smooth_is_solid(voxels, voxels_locks, solidity, position, depth, edge_a),
            smooth_is_solid(voxels, voxels_locks, solidity, position, depth, edge_b));
    }
}

static inline void zox_apply_smooth_lights(
    spinlock** lights_locks,
    spinlock** voxels_locks,
    const LightNode** lights,
    const VoxelNode** voxels,
    const byte* solidity,
    const VoxelNode* node,
    const SidesOctree* sides,
    const MeshColorRGBs* colors,
    byte3 position,
    uint* ccount,
    byte target_depth,
    byte depth)
{
    const SidesOctree* sides_kids =
        get_children_SidesOctree(sides);
    byte has_sides_kids = sides_kids != NULL;
    spinlock* node_lock = voxels_locks[OCTREE_INDEX(0, 0, 0)];
    // Dig Deeper
    if (depth < target_depth && has_sides_kids) {
        spin_lock(node_lock);
        const VoxelNode* node_kids = has_children_VoxelNode(node) ?
            get_children_VoxelNode(node) :
            NULL;
        spin_unlock(node_lock);
        byte3_multiply_byte(&position, 2);
        depth++;
        for (byte i = 0; i < 8; i++) {
            byte3 child_position = byte3_add(
                position,
                octree_positions[i]);
            zox_apply_smooth_lights(
                lights_locks,
                voxels_locks,
                lights,
                voxels,
                solidity,
                node_kids ? &node_kids[i] : node,
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
    // the mesh builder skips air with sides (stale sides), so we do too
    spin_lock(node_lock);
    const byte is_air = !node->value;
    spin_unlock(node_lock);
    if (is_air) {
        return;
    }
    // each face the sides say is visible
    for (byte direction = 0; direction < 6; direction++) {
        if (!(sides->value & (1 << (direction + 1)))) {
            continue;
        }
        if (*ccount + voxel_face_vertices_length > colors->length) {
            zox_smooth_lights_errors++;
            zox_loge("Smooth Lights Error: Colors Past Limits [%i] - Face [%i] at Pos [%ix%ix%i] Depth [%i] of [%i]",
                *ccount, direction, position.x, position.y, position.z, depth, target_depth);
            return;
        }
        byte vertex_lights[4];
        smooth_face_lights(lights, lights_locks, voxels, voxels_locks, solidity, position, depth, direction, vertex_lights);
        for (byte v = 0; v < voxel_face_vertices_length; v++) {
            colors->value[(*ccount)++] = (color_rgb) { vertex_lights[v], vertex_lights[v], vertex_lights[v] };
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
    // which blocks stop light, rebuilt when the block manager (realm) changes (fun/solidity.c)
    byte solidity[zox_light_solidity_length];
    memset(solidity, 1, zox_light_solidity_length);
    entity realm = 0;
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
            // can't light it, don't block its geometry upload
            zox_remove(e, BuildMeshColors);
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
        update_light_solidity(world, zox_get_link(world, chunk, BlockManagerLink), &realm, solidity);
        uint ccount = 0;
        spin_lock(&sides_lock->value);
        zox_apply_smooth_lights(
            lights_locks,
            voxels_locks,
            lights,
            voxels,
            solidity,
            voxels[OCTREE_INDEX(0, 0, 0)],
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

// NOTE: uses zox_set here for children setting
// NOTE: I may need to thread lock/unlock ChunkEntities when reading
// block spawn delve function
void set_chunk_block_spawns_render_disabled(
    ecs *world,
    const VoxelNode *node,
    byte state)
{
    if (is_closed_VoxelNode(node)) {
        return;
    } else if (is_linked_VoxelNode(node)) {
        entity e = get_entity_VoxelNode(node);
#ifdef zox_safety_checks
        if (!zox_valid(e)) {
            return;
        }
#endif
        // NOTE: Some Block Instances dont have this component
        if (zox_has(e, RenderDisabled)) {
            zox_setm(e, RenderDisabled, state);
        }
    } else if (has_children_VoxelNode(node)) {
        VoxelNode* kids = (VoxelNode*) node->ptr;
        for (int i = 0; i < octree_length; i++) {
            set_chunk_block_spawns_render_disabled(
                world,
                &kids[i],
                state);
        }
    }
}

void set_entity_render_disabled(
    ecs* world,
    entity e,
    byte disabled)
{
    if (!zox_valid(e)) {
        return;
    }
    if (zox_has(e, RenderDisabled)) {
        zox_setm(e, RenderDisabled, disabled);
    }
    // NOTE: for 3D UIs
    if (zox_has(e, ElementLinks)) {
        zox_geter(e, ElementLinks, elements);
        for (int k = 0; k < elements->length; k++) {
            entity e2 = elements->value[k];
#ifdef zox_safety_checks
            if (!zox_valid(e2)) {
                continue;
            }
#endif
            set_entity_render_disabled(world, e2, disabled);
        }
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
#ifdef zox_safety_checks
            if (!zox_valid(e2)) {
                continue;
            }
#endif
            set_entity_render_disabled(world, e2, disabled);
        }
    }

}

byte is_sphere_in_frustum(
    const plane* planes,
    float3 center,
    float radius)
{
    for (int i = 0; i < 6; i++) {
        float dist =
            float3_dot(
                planes[i].normal,
                center)
            - planes[i].distance;
        if (dist < -radius) {
            return 0; // sphere is completely outside
        }
    }
    return 1;
}

// Fast AABB cull using positive-vertex trick (drop-in replacement)
byte aabb_in_frustum_fast(
    const plane *planes,
    bounds b,
    float eps)
{
    for (int i = 0; i < 6; ++i) {
        const plane p = planes[i];
        // choose farthest vertex in direction of plane normal
        float3 pv = {
            b.center.x + (p.normal.x >= 0.0f ? b.extents.x : -b.extents.x),
            b.center.y + (p.normal.y >= 0.0f ? b.extents.y : -b.extents.y),
            b.center.z + (p.normal.z >= 0.0f ? b.extents.z : -b.extents.z)
        };
        if (float3_dot(p.normal, pv) - p.distance > eps) return 0; // outside
    }
    return 1; // intersects/inside
}

// TODO: Optimize this, perhaps with MegaChunks? Need to reduce the overall calls for >= 10k chunks
// this sets RenderDisabled for chunks and their children
void chunk_frustum_system(iter* it) {
    // TODO: We can cache the cameras here
    byte dbg_log = 0;
    zox_sys_on_begin();
    byte ignore_low_lods = 0;
    byte frustum_inwards = 1; // we just using this for safety
    // cache camera positions first
    float6_array_d* camera_bounds = create_float6_array_d(1);
    plane_array_d* camera_planes = create_plane_array_d(6);
    zox_sys_world();
    zox_sys_query();
    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        zox_sys_begin_2();
        zox_sys_in_2(Position3DBounds);
        zox_sys_in_2(CameraPlanes);
        for (int j = 0; j < it2.count; j++) {
            zox_sys_i_2(Position3DBounds, frustum_bounds);
            zox_sys_i_2(CameraPlanes, camera_planes2);
            float6_array_d_add(camera_bounds, frustum_bounds->value);
            for (int k = 0; k < zox_camera_planes; k++) {
                plane_array_d_add(
                    camera_planes,
                    camera_planes2->value[k]);
            }
        }
    }
    zox_sys_query_end();
    if (!camera_bounds->size) {
        zox_loge("No Cameras in [BillboardSystem]");
        dispose_float6_array_d(camera_bounds);
        dispose_plane_array_d(camera_planes);
        return;
    }
    // Sanity check
#ifdef zox_debug
    if (camera_planes->size != camera_bounds->size * zox_camera_planes) {
        zox_log("Camera Planes Size [%i] != Added [%i] [%ix%i]",
            camera_planes->size,
            camera_bounds->size * zox_camera_planes,
            camera_bounds->size,
            zox_camera_planes);
        return;
    }
#endif
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Bounds3D);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkEntities);
    zox_sys_out(RenderDisabled);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Bounds3D, bounds3);
        zox_sys_i(VoxelNode, voxels);
        zox_sys_i(ChunkEntities, entities);
        zox_sys_o(RenderDisabled, render_disabled);
        // NOTE: Some quick skips for largest voxels
        if (ignore_low_lods == 2 && !voxels->value && !voxels->ptr) {
            // render_disabled->value = 0;
            continue;
        }
        // our bounds3D isn't centred, terrain chunks corner offset!
        bounds chunk_bounds = {
            .center = float3_add(
                position->value,
                bounds3->value),
            .extents = bounds3->value
        };
        float3_scale_p(
            &chunk_bounds.extents,
            fudge_frustum_extents);
        byte is_viewed = disable_frustum_culling;
        if (ignore_low_lods && voxels->value && !voxels->ptr) {
            is_viewed = 1;
        }
        // For each Camera
        float radius = float3_length(chunk_bounds.extents);
        for (int j = 0; j < camera_bounds->size && !is_viewed; j++) {
            float6 frustum_bounds = camera_bounds->data[j];
            plane* planes = &camera_planes->data[j * zox_camera_planes];
            // our normals appear to be flipped
            byte inside_sphere = is_sphere_in_frustum(
                planes,
                chunk_bounds.center,
                radius);
            is_viewed =
                inside_sphere &&
                is_bounds_in_position_bounds(
                    frustum_bounds,
                    chunk_bounds) &&
                is_in_frustum(
                    planes,
                    chunk_bounds,
                    frustum_inwards);
            if (dbg_log >= 2) {
                zox_log("Checking for Camera [%s]",
                    zox_getn(it2.entities[j]));
            }
        }
        zox_sys_increment();
        if (render_disabled->value == !is_viewed) {
            continue;
        }
        render_disabled->value = !is_viewed;
        // -=- Chunk Meshes -=-
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e3 = it2.entities[j];
                if (zox_has(e3, ChunkMesh)) {
                    zox_setm(e3, RenderDisabled, render_disabled->value);
                }
            }
        }
        if (dbg_log >= 2) {
            zox_log("Chunk [%s] now Visible? [%s]",
                zox_getn(e),
                    is_viewed ?
                        "Visible" :
                        "Invisible");
        }
        // NOTE: Assuming this was toggled by children now??
        // Also set objects inside our terrain chunks!
        // -=- World Blocks -=-
        if (zox_has(e, BlocksSpawned)) {
            set_chunk_block_spawns_render_disabled(
                world,
                voxels,
                render_disabled->value);
        }
        // -=- -=- -=- -=- -=- -=-
        // NOTE: For characters
        for (int j = 0; j < entities->length; j++) {
            entity e2 = entities->value[j];
            set_entity_render_disabled(
                world,
                e2,
                render_disabled->value);
        }
        // -=- -=- -=- -=- -=- -=-
    }
    dispose_float6_array_d(camera_bounds);
    dispose_plane_array_d(camera_planes);
    zox_sys_on_end();
} zoxd_system(chunk_frustum_system);

// NOTE: uses zox_set here for children setting
// NOTE: I may need to thread lock/unlock ChunkEntities when reading
// block spawn delve function
void set_chunk_block_spawns_render_disabled(ecs *world, const VoxelNode *node, byte state) {
    if (is_closed_VoxelNode(node)) {
        return;
    } else if (is_linked_VoxelNode(node)) {
        entity e = get_entity_VoxelNode(node);
#ifdef zox_safety_checks
        if (!zox_valid(e)) {
            return;
        }
#endif
        zox_set(e, RenderDisabled, { state });
    } else if (has_children_VoxelNode(node)) {
        VoxelNode* kids = get_children_VoxelNode(node);
        for (int i = 0; i < octree_length; i++) {
            set_chunk_block_spawns_render_disabled(world, &kids[i], state);
        }
    }
}

void set_entity_render_disabled(ecs* world, entity e, byte disabled) {
    if (!zox_valid(e)) {
        return;
    }
    zox_set(e, RenderDisabled, { disabled });
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
            if (zox_has(e2, RenderDisabled)) {
                zox_set(e2, RenderDisabled, { disabled });
            }
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
            if (zox_has(e2, RenderDisabled)) {
                zox_set(e2, RenderDisabled, { disabled });
            }
            set_entity_render_disabled(world, e2, disabled);
        }
    }

}

byte is_sphere_in_frustum(const plane* planes, float3 center, float radius) {
    for (int i = 0; i < 6; i++) {
        float dist = float3_dot(planes[i].normal, center) - planes[i].distance;
        if (dist < -radius) {
            return 0; // sphere is completely outside
        }
    }
    return 1;
}

// Fast AABB cull using positive-vertex trick (drop-in replacement)
byte aabb_in_frustum_fast(const plane *planes, bounds b, float eps) {
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
zox_sys2(ChunkFrustumSystem) {
    byte dbg_log = 0;
    byte frustum_inwards = 1; // we just using this for safety
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Bounds3D);
    zox_sys_in(VoxelNode);
    zox_sys_in(BlocksSpawned);
    zox_sys_in(ChunkEntities);
    zox_sys_out(RenderDisabled);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Bounds3D, bounds3);
        zox_sys_i(VoxelNode, voctree);
        zox_sys_i(BlocksSpawned, spawned);
        zox_sys_i(ChunkEntities, entities);
        zox_sys_o(RenderDisabled, render_disabled);
        // NOTE: Some quick skips for largest voctrees
        if (!voctree->value && !voctree->ptr) {
            continue;
        }
        if (voctree->value && !voctree->ptr) {
            render_disabled->value = 0;
            continue;
        }
        // our bounds3D isn't centred, terrain chunks corner offset!
        bounds chunk_bounds = {
            .center = float3_add(position->value, bounds3->value),
            .extents = bounds3->value
        };
        float3_scale_p(&chunk_bounds.extents, fudge_frustum_extents);
        zox_sys_query_begin();
        byte is_viewed = disable_frustum_culling;
        // For each Camera
        while (zox_sys_query_loop()) {
            if (is_viewed) {
                continue;   // make sure to iterate all flecs query
            }
            zox_sys_begin_2();
            zox_sys_in_2(Position3DBounds);
            zox_sys_in_2(CameraPlanes);
            for (int j = 0; j < it2.count && !is_viewed; j++) {
                zox_sys_i_2(Position3DBounds, frustum_bounds);
                zox_sys_i_2(CameraPlanes, planes);
                // our normals appear to be flipped
                byte inside_sphere = is_sphere_in_frustum(planes->value, chunk_bounds.center, float3_length(chunk_bounds.extents));
                is_viewed = inside_sphere &&  is_bounds_in_position_bounds(frustum_bounds->value, chunk_bounds) &&  is_in_frustum(planes->value, chunk_bounds, frustum_inwards);
                if (dbg_log >= 2) {
                    zox_log("Checking for Camera [%s]", zox_get_name(it2.entities[j]));
                }
            }
        }
        zox_sys_query_end();
        if (render_disabled->value != !is_viewed) {
            render_disabled->value = !is_viewed;
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
                zox_log("Chunk [%s] now Visible? [%s]", zox_get_name(e), is_viewed ? "Visible" : "Invisible");
            }
            // Also set objects inside our terrain chunks!
            // -=- World Blocks -=-
            if (spawned->value) {
                set_chunk_block_spawns_render_disabled(world, voctree, render_disabled->value);
            }
            // -=- -=- -=- -=- -=- -=-
            // NOTE: For characters
            for (int j = 0; j < entities->length; j++) {
                entity e2 = entities->value[j];
                set_entity_render_disabled(world, e2, render_disabled->value);
            }
            // -=- -=- -=- -=- -=- -=-
        }
        zox_sys_increment();
    }
} zox_sys_end(ChunkFrustumSystem);

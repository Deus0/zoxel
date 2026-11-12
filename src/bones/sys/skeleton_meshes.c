// A skeleton gets a child SkeletonMesh per render depth, built from its owners vox (VoxLink)
//  - its owner (parent) holds RenderDepth and RenderDisabled, i.e. a character
//  - like terrain chunk meshes, a mesh is spawned when its depth is first needed, then kept
//  - the PreparingMesh builds while the ActiveMesh renders, then they swap
//  - when voxels change, cached meshes are deleted and the active one rebuilds

entity find_skeleton_mesh(
    ecs* world,
    entity e,
    byte depth)
{
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity mesh = it2.entities[j];
            if (zox_has(mesh, SkeletonMesh) &&
                zox_getv(mesh, RenderDepth) == depth)
            {
                ecs_iter_fini(&it2);
                return mesh;
            }
        }
    }
    return 0;
}

// Built, weighted and uploaded
byte is_skeleton_mesh_ready(
    ecs* world,
    entity mesh)
{
    return zox_has(mesh, MeshBuilt) &&
        !zox_has(mesh, BuildMesh) &&
        !zox_has(mesh, BuildMeshWeights) &&
        !zox_has(mesh, SkeletonMeshDirty) &&
        !zox_has(mesh, MeshDirty);
}

entity spawn_skeleton_mesh(
    ecs* world,
    entity e,
    byte depth,
    byte render_disabled)
{
    entity mesh = zox_ins(world, prefab_skeleton_mesh);
    zox_set_unique_name(mesh, "skeleton_mesh");
    zox_setv(mesh, RenderDepth, depth);
    zox_setv(mesh, RenderDisabled, render_disabled);
    zox_set_parent_non_fragment(world, mesh, e);
    return mesh;
}

// Sets the PreparingMesh for a depth, spawning it if it doesn't exist
//  - is_cache_valid is 0 when cached meshes are being deleted this frame
void prepare_skeleton_mesh(
    ecs* world,
    entity e,
    byte depth,
    entity active,
    entity preparing,
    byte is_cache_valid,
    byte render_disabled,
    byte dbg_log)
{
    entity mesh = 0;
    if (zox_valid(active) && zox_getv(active, RenderDepth) == depth) {
        mesh = active;
    } else if (is_cache_valid) {
        mesh = find_skeleton_mesh(world, e, depth);
    }
    if (mesh && mesh == preparing) {
        return;
    }
    if (zox_valid(preparing)) {
        zox_unlink(world, e, PreparingMesh, preparing);
    }
    // NOTE: Moving back to the active depth, nothing to swap
    if (mesh && mesh == active) {
        return;
    }
    if (!mesh) {
        mesh = spawn_skeleton_mesh(world, e, depth, render_disabled);
        if (dbg_log) {
            zox_log("[%s] spawned skeleton mesh at depth [%i]", zox_getn(e), depth);
        }
    } else if (dbg_log) {
        zox_log("[%s] reusing skeleton mesh at depth [%i]", zox_getn(e), depth);
    }
    zox_link(world, e, PreparingMesh, mesh);
}

// Enables the prepared mesh, disables the rest
void swap_skeleton_mesh(
    ecs* world,
    entity e,
    entity active,
    entity preparing)
{
    zox_unlink(world, e, PreparingMesh, preparing);
    if (zox_valid(active)) {
        zox_unlink(world, e, ActiveMesh, active);
    }
    zox_link(world, e, ActiveMesh, preparing);
    if (zox_has(preparing, Disabled)) {
        zox_remove(preparing, Disabled);
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity mesh = it2.entities[j];
            if (mesh == preparing ||
                !zox_has(mesh, SkeletonMesh) ||
                zox_has(mesh, Disabled))
            {
                continue;
            }
            zox_add(mesh, Disabled);
        }
    }
}

// When voxels change, delete cached meshes and rebuild the active one
void refresh_skeleton_meshes(
    ecs* world,
    entity e,
    entity active)
{
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity mesh = it2.entities[j];
            if (!zox_has(mesh, SkeletonMesh)) {
                continue;
            }
            if (mesh == active) {
                if (!zox_has(mesh, BuildMesh)) {
                    zox_add(mesh, BuildMesh);
                }
            } else {
                zox_delete(mesh);
            }
        }
    }
}

void SkeletonMeshesSystem(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity owner = zox_get_parent(world, e);
        if (!zox_valid(owner) ||
            !zox_has(owner, RenderDepth))
        {
            continue;
        }
        entity vox = zox_get_link(world, owner, VoxLink);
        if (!zox_valid(vox) || !zox_has(vox, ColorRGBs)) {
            continue;
        }
        byte render_depth = zox_getv(owner, RenderDepth);
        zox_geter(vox, ColorRGBs, vcolors);
        byte node_depth = zox_getv(vox, NodeDepth);
        entity active = zox_get_link(world, e, ActiveMesh);
        entity preparing = zox_get_link(world, e, PreparingMesh);
        if (!zox_valid(active)) {
            active = 0;
        }
        if (!zox_valid(preparing)) {
            preparing = 0;
        }
        byte is_voxels_updated = zox_has(vox, ChunkUpdate);
        byte is_depth_updated = zox_has(owner, RenderDepthDirty) &&
            zox_getv(owner, RenderDepthDirty) == zox_dirty_active;
        if (is_voxels_updated) {
            refresh_skeleton_meshes(world, e, active);
            if (preparing) {
                zox_unlink(world, e, PreparingMesh, preparing);
                preparing = 0;
            }
        }
        byte has_voxels = vcolors->length > 0;
        byte has_depth = render_depth != render_depth_uninitialized;
        byte is_prepare = is_voxels_updated ||
            is_depth_updated ||
            (!active && !preparing);
        if (has_voxels && has_depth && is_prepare) {
            byte depth = render_depth;
            if (depth > node_depth) {
                depth = node_depth;
            }
            prepare_skeleton_mesh(
                world,
                e,
                depth,
                active,
                preparing,
                !is_voxels_updated,
                zox_has(owner, RenderDisabled) ? zox_getv(owner, RenderDisabled) : 0,
                dbg_log);
        } else if (preparing && is_skeleton_mesh_ready(world, preparing)) {
            swap_skeleton_mesh(world, e, active, preparing);
            if (dbg_log) {
                zox_log("[%s] swapped to skeleton mesh at depth [%i]",
                    zox_sys_e_name,
                    zox_getv(preparing, RenderDepth));
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(SkeletonMeshesSystem);

// Builds a skeleton mesh from its skeletons owner vox at RenderDepth
void SkeletonMeshBuildSystem(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_out(MeshIndicies);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, vertices);
        zox_sys_o(MeshColorRGBs, colors);
        entity vox = get_skeleton_vox(world, zox_get_parent(world, e));
        byte result = build_vox_mesh_at_depth(
            world,
            vox,
            rdepth->value,
            indicies,
            vertices,
            colors);
        zox_remove(e, BuildMesh);
        if (result == zox_vox_mesh_invalid) {
            zox_loge("SkeletonMesh [%s] has invalid skeleton owner vox", zox_sys_e_name);
            continue;
        }
        if (result != zox_vox_mesh_built) {
            continue;
        }
        zox_add(e, BuildMeshWeights);
        if (dbg_log) {
            zox_log("[%s] built skeleton mesh from [%s] depth [%i] tris [%i]",
                zox_sys_e_name,
                zox_getn(vox),
                rdepth->value,
                indicies->length / 3);
        }
    }
    zox_sys_on_end();
} zoxd_system(SkeletonMeshBuildSystem);


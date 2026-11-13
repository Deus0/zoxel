entity spawn_item_vox_mesh(ecs* world, entity item) {
    entity model = zox_get_link(world, item, ModelLink);
    entity mesh = get_max_model_mesh(world, model);
    if (!mesh) {
        zox_loge("[spawn_item_vox_mesh] Invalid [mesh]: item [%s], model [%s], mesh [%s]",
            zox_getn(item),
            zox_getn(model),
            zox_getn(mesh));
        return 0;
    }
    if (!zox_has(mesh, Mesh)) {
        zox_loge("[spawn_item_vox_mesh] Invalid Components [mesh]: item [%s], model [%s], mesh [%s]",
            zox_getn(item),
            zox_getn(model),
            zox_getn(mesh));
        return 0;
    }
    entity e = spawn_mesh3_clone(world, mesh);
    if (!e) {
        zox_loge("Invalid [mesh_clone]");
        return 0;
    }
    zox_add(e, VoxMesh);
    zox_set_unique_name(e, "item_mesh");
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, quaternion_identity);
    return e;
}

// a vox item's model bounds in its mesh's units, from its solid voxels (vox_filled_bounds), 0 if it has none
//  - its mesh puts voxel i at -bounds + i * BlockScale (build_vox_mesh_at_depth), bounds half its ChunkSize
byte item_vox_bounds(ecs* world, entity meta, float3* low, float3* high) {
    const entity model = zox_valid(meta) && zox_has(meta, ItemVox) ? zox_get_link(world, meta, ModelLink) : 0;
    int3 voxel_low, voxel_high;
    if (!zox_valid(model) || !zox_has(model, BlockScale) || !vox_filled_bounds(world, model, &voxel_low, &voxel_high)) {
        return 0;
    }
    const float scale = zox_getv(model, BlockScale);
    const float3 start = float3_scale(calculate_vox_bounds(zox_getv(model, ChunkSize), scale), -1);
    *low = float3_add(start, float3_scale(int3_to_float3(voxel_low), scale));
    *high = float3_add(start, float3_scale(float3_add(int3_to_float3(voxel_high), float3_single(1)), scale));
    return 1;
}

// a vox item's model mesh size, zero if it has none
float3 item_vox_extent(ecs* world, entity meta) {
    float3 low, high;
    return item_vox_bounds(world, meta, &low, &high) ? float3_sub(high, low) : float3_zero;
}

// an item mesh's scale: a block's mesh is a unit cube, scale wide; a vox item (a cookie) scaled so its biggest side is
//  scale wide too (a flat cookie stays flat)
float item_mesh_scale(ecs* world, entity meta, float scale) {
    const float3 extent = item_vox_extent(world, meta);
    const float biggest = fmaxf(extent.x, fmaxf(extent.y, extent.z));
    return biggest > 0 ? scale / biggest : scale;
}

// an item's held or dropped size, in the world
float3 item_mesh_size(ecs* world, entity meta, float scale) {
    const float3 extent = item_vox_extent(world, meta);
    return extent.x > 0 ? float3_scale(extent, item_mesh_scale(world, meta, scale)) : float3_single(scale);
}

// where to put an item mesh so its voxels' middle is on its parent (a model's voxels needn't fill its grid), in the
//  world, zero for blocks
float3 item_mesh_offset(ecs* world, entity meta, float scale) {
    float3 low, high;
    if (!item_vox_bounds(world, meta, &low, &high)) {
        return float3_zero;
    }
    return float3_scale(float3_mid(low, high), -item_mesh_scale(world, meta, scale));
}


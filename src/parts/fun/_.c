// NOTE: entity_array_d* parts = create_entity_array_d(1);
// NOTE: dispose_entity_array_d(parts);

// Returns the vox of an item model, it holds max depth voxels
entity get_item_model(ecs* world, entity e) {
    if (!zox_valid(e)) {
        zox_logw("Item Invalid");
        return 0;
    }
    if (!zox_is_linked_any(world, e, ModelLink)) {
        zox_logw("Item has no ModelLink [%s]", zox_get_name(e));
        return 0;
    }
    entity model = get_model_first(world, zox_get_link(world, e, ModelLink));
    if (!zox_valid(model) ||
        !zox_has(model, VoxelNode) ||
        !zox_has(model, NodeDepth) ||
        !zox_has(model, ChunkSize))
    {
        zox_logw("Item has Invalid Model [%s]", zox_get_name(e));
        return 0;
    }
    return model;
}

// Depth an item model is used at, its MaxRenderDepth, limited by the vox depth
byte get_item_model_depth(ecs* world, entity e, entity vox) {
    byte vox_depth = zox_getv(vox, NodeDepth);
    if (!zox_has(e, MaxRenderDepth)) {
        return vox_depth;
    }
    byte depth = zox_getv(e, MaxRenderDepth);
    return depth < vox_depth ? depth : vox_depth;
}

// Size of an item model, at the depth the item uses it
int3 get_item_model_size(ecs* world, entity e, entity vox) {
    return get_vox_size_at_depth(
        zox_getv(vox, ChunkSize),
        zox_getv(vox, NodeDepth),
        get_item_model_depth(world, e, vox));
}


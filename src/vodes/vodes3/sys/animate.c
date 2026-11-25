// block voxes whose block's models are VoxAnimations (ModelAnimated, spawn_blueprint_animation): each shows its block's
//  animation for its state, its ActiveModelLink's while its voxel is on in its chunk's ActiveOctree (a gravity cube
//  switched on), its ModelLink's while off; the frame by zox_current_time at the animation's AnimationFps, so every one
//  shows the same frame at once (one of each drawn at a time, the instance renderer holds 64 meshes)
//  - its model and its mesh at its depth swapped

void vode_animate_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelLocalPosition);
    zox_sys_in(RenderDepth);
    zox_sys_in(BlockIndex);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelLocalPosition, local);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(BlockIndex, block_index);
        const entity chunk = zox_get_parent(world, e);
        const entity terrain = zox_valid(chunk) ? zox_get_parent(world, chunk) : 0;
        const entity realm = zox_valid(terrain) ? zox_get_parent(world, terrain) : 0;
        if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
            continue;
        }
        const BlockLinks* blocks = zox_get(realm, BlockLinks);
        // a block vox's BlockIndex is its block's place in the realm's list (its voxel - 1)
        if (block_index->value >= blocks->length) {
            continue;
        }
        const entity block = blocks->value[block_index->value];
        const byte on = zox_has(chunk, ActiveOctree) &&
            getv_ActiveOctree(zox_get(chunk, ActiveOctree), zox_getv(chunk, NodeDepth), local->value) != 0;
        entity animation = on ? zox_get_link(world, block, ActiveModelLink) : 0;
        if (!zox_valid(animation)) {
            animation = zox_get_link(world, block, ModelLink);
        }
        if (!zox_valid(animation) || !zox_has(animation, VoxAnimation)) {
            continue;
        }
        // the frame it should show, its frames looked through only if it shows another
        entity frames[64];
        const uint count = zox_get_children_by_id(world, animation, frames, 64, zox_id(Model));
        if (!count) {
            continue;
        }
        const byte index = (byte) ((uint) (zox_current_time * zox_getv(animation, AnimationFps)) % count);
        const entity model = zox_get_link(world, e, ModelLink);
        if (zox_valid(model) && zox_get_parent(world, model) == animation && zox_getv(model, ModelFrame) == index) {
            continue;
        }
        entity next = 0;
        for (uint j = 0; j < count && !next; j++) {
            next = zox_getv(frames[j], ModelFrame) == index ? frames[j] : 0;
        }
        if (!zox_valid(next)) {
            continue;
        }
        zox_set_link(world, e, ModelLink, next);
        if (depth->value != render_depth_uninitialized && zox_has(next, VoxLods)) {
            zox_set_link(world, e, InstanceLink, get_model_lod_mesh(world, next, depth->value));
        }
    }
    zox_sys_on_end();
} zoxd_system(vode_animate_system);

// a model's mesh at a lod: its child mesh at that RenderDepth, its solid one (a see through one is TransparentMesh),
//  0 if none; read when an instance's lod changes, not every frame
static inline entity get_model_lod_mesh(ecs* world, entity model, byte depth) {
    iter it = zox_children(world, model);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity child = it.entities[i];
            if (zox_has(child, Mesh) && zox_has(child, RenderDepth) && !zox_has(child, TransparentMesh) &&
                zox_getv(child, RenderDepth) == depth)
            {
                zox_children_end(it);
                return child;
            }
        }
    }
    return 0;
}

// an instance links its model's mesh at its lod (get_model_lod_mesh) when its lod changes
void lod_instance_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(RenderDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_i(RenderDepth, rdepth);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity model = zox_get_link(world, e, ModelLink);
        if (rdepth->value == render_depth_uninitialized) { //  || rdepth->value == render_depth_invisible) {
            zox_set_link(world, e, InstanceLink, 0);
            continue;
        }
        if (!zox_valid(model)) {
            zox_loge("[%s] has invalid Model", zox_get_name(e));
            continue;
        }
        const entity vox_lod = get_model_lod_mesh(world, model, rdepth->value);
        if (!zox_valid(vox_lod)) {
            zox_logw("+ %s with model [%s] has invalid vox lod at Depth [%i]", zox_get_name(e), zox_get_name(model), rdepth->value);
            continue;
        }
        zox_set_link(world, e, InstanceLink, vox_lod);
    }
    zox_sys_on_end();
} zoxd_system(lod_instance_system);

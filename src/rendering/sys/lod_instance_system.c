// NOTE: Sets Instance Links Based on ModelLods!
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
        if (!zox_has(model, ModelLods)) {
            zox_loge("Block [%s] Model [%s] has no ModelLods", zox_get_name(e), zox_get_name(model));
            continue;
        }
        const ModelLods* lods = zox_get(model, ModelLods);
        entity vox_lod = lods->value[rdepth->value];
        if (!zox_valid(vox_lod)) {
            zox_logw("+ %s with model [%s] has invalid vox lod at Depth [%i]", zox_get_name(e), zox_get_name(model), rdepth->value);
            continue;
        }
        zox_set_link(world, e, InstanceLink, vox_lod);
    }
    zox_sys_on_end();
} zoxd_system(lod_instance_system);

// NOTE: Sets Instance Links Based on ModelLods!
zox_sys2(LodInstanceSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(ModelLink);
    zox_sys_out(InstanceLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(ModelLink, model);
        zox_sys_o(InstanceLink, instance);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (rdepth->value == render_depth_spawning || rdepth->value == render_depth_invisible) {
            instance->value = 0;
            continue;
        }
        if (!zox_valid(model->value)) {
            zox_loge("[%s] has invalid Model", zox_get_name(e));
            continue;
        }
        if (!zox_has(model->value, ModelLods)) {
            zox_loge("Block [%s] Model [%s] has no ModelLods", zox_get_name(e), zox_get_name(model->value));
            continue;
        }
        zox_geter(model->value, ModelLods, lods);
        entity vox_lod = lods->value[rdepth->value];
        if (!zox_valid(vox_lod)) {
            zox_logw("+ %s with model [%s] has invalid vox lod at Depth [%i]", zox_get_name(e), zox_get_name(model->value), rdepth->value);
            continue;
        }
        instance->value = vox_lod;
    }
} zox_sys_end(LodInstanceSystem);

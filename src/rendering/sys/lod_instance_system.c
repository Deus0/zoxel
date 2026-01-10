zox_sys2(LodInstanceSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(ModelLink);
    zox_sys_out(InstanceLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDepthDirty, renderDepthDirty);
        zox_sys_i(RenderDepth, renderDepth);
        zox_sys_i(ModelLink, modelLink);
        zox_sys_o(InstanceLink, instanceLink);

        if (renderDepthDirty->value != zox_dirty_active || renderDepth->value == render_depth_spawning || renderDepth->value == render_depth_invisible) {
            continue;
        }

        if (zox_valid(modelLink->value) && zox_has(modelLink->value, ModelLods)) {
            // use InstanceLinks to set model based on Lod!
            zox_geter(modelLink->value, ModelLods, modelLods)

            entity vox_lod = modelLods->value[renderDepth->value];
            if (zox_valid(vox_lod)) {
                instanceLink->value = vox_lod;
            } else {
                zox_sys_e();
                zox_logw("+ %s with model [%s] has invalid vox lod at Depth [%i]",
                        zox_get_name(e),
                        zox_get_name(modelLink->value),
                        renderDepth->value);
            }

            // zox_sys_e();
            // zox_log("+ %s with model [%s] has set mesh to [%s]", zox_get_name(e), zox_get_name(modelLink->value), zox_get_name(instanceLink->value));

        }
        // some do not
        /*else {
            zox_sys_e()
            zox_log_error("model has no ModelLods [%s]", zox_get_name(e))
        }*/
    }
} zox_sys_end(LodInstanceSystem);
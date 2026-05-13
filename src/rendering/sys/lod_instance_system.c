extern void copy_block_scale(ecs* world, entity e1, entity e2);

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
        if (zox_valid(model->value) && zox_has(model->value, ModelLods)) {
            // use InstanceLinks to set model based on Lod!
            zox_geter(model->value, ModelLods, lods);
            entity vox_lod = lods->value[rdepth->value];
            if (zox_valid(vox_lod)) {
                instance->value = vox_lod;
                // copy_block_scale(world, vox_lod, e);
                // zox_geter_value(vox_lod, BlockScale, float, bscale);
                // zox_set(e, BlockScale, { bscale });
            } else {
                zox_logw("+ %s with model [%s] has invalid vox lod at Depth [%i]", zox_get_name(e), zox_get_name(model->value), rdepth->value);
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

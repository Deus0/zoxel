// for now just set vox, later we spawn item and set it from BodyDirty
zox_sys2(PlayerBodySpawnSystem) {
    char* player_vox_model = "playerer";
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BodyDirty);
    zox_sys_in(BodyLinks);
    zox_sys_in(RenderDepth);
    //zox_sys_out()
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BodyDirty, state);
        zox_sys_i(BodyLinks, bodys);
        zox_sys_i(RenderDepth, rdepth);

        if (state->value != zox_dirty_active) {
            continue;
        }

        if (!bodys->length) {
            zox_logw("No Body to Generate");
            continue;
        }

        entity part = bodys->value[0];
        if (!zox_valid(part)) {
            continue;
        }

        zox_log("Spawning player body");

        zox_geter_value_non_const(part, ModelLink, entity, vox);

        // entity vox =  string_hashmap_get(files_hashmap_voxes, new_string_data(player_vox_model));


        if (zox_valid(vox) && zox_has(vox, ModelLods)) {
            zox_geter(vox, ModelLods, modelLods);

            entity vox_lod = modelLods->value[rdepth->value];

            if (zox_valid(vox_lod)) {
                vox = vox_lod;
            } else {
                zox_log_error("[player body]: Invalid Vox Model Lod [%s] Depth [%i]", zox_get_name(vox), rdepth->value);
            }
        }

        if (!zox_valid(vox)) {
            zox_log_error("player vox invalid");
            continue;
        }

        if (zox_has(vox, MaxRenderDepth)) {
            zox_geter_value(vox, MaxRenderDepth, byte, max_render_depth);

            zox_set(e, MaxRenderDepth, { max_render_depth });
        }

        zox_set(e, ModelLink, { vox });
        zox_set(e, CloneVoxLink, { vox });
        zox_set(e, CloneVox, { 1 });

        zox_set(e, DisableMovement, { 0 });
    }
} zox_sys_end(PlayerBodySpawnSystem);

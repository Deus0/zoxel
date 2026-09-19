// NOTE: Tells the block textures to generate when the Model Generates!
zox_sys2(BakeVoxSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ModelLink);
    // zox_sys_in(TextureLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ModelLink, model);
        // zox_sys_i(TextureLinks, textures);
        if (!zox_valid(model->value)) {
            continue;
        }
        if (!zox_has(model->value, GenerateModel)) {
            // zox_loge("Model missing GenerateChunk");
            continue;
        }
        byte generate = zox_getv(model->value, GenerateModel);
        if (generate != zox_generate_model_bake) {
            continue;
        }
        zox_setv(model->value, GenerateModel, zox_generate_model_end);
        if (dbg_log) {
            zox_log("Block is Generating Textures: %s",
                zox_getn(e));
        }
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int k = 0; k < it2.count; k++) {
                entity texture = it2.entities[k];
                if (!zox_has(texture, Texture)) {
                    continue;
                }
        /*for (int j = 0; j < textures->length; j++) {
            entity texture = textures->value[j];
            if (!zox_valid(texture)) {
                zox_loge("Block texture is invalid [%i]", j);
                continue;
            }*/
                zox_setv(texture, GenerateTexture,  zox_generate_texture_run);
                if (dbg_log) {
                    zox_log(" - Vox Texture: %s",
                        zox_getn(texture));
                }
            }
        }
    }
} zox_sys_end(BakeVoxSystem);

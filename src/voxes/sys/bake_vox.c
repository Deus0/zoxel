// NOTE: Tells the block textures to generate when the Model Generates!
zox_sys2(BakeVoxSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ModelLink);
    zox_sys_in(TextureLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ModelLink, model);
        zox_sys_i(TextureLinks, textures);
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
        if (dbg_log) {
            zox_log("Block is Generating: %s %i", zox_get_name(e), textures->length);
        }
        for (int j = 0; j < textures->length; j++) {
            entity texture = textures->value[j];
            if (!zox_valid(texture)) {
                zox_loge("Block texture is invalid [%i]", j);
                continue;
            }
            zox_set(texture, GenerateTexture, { zox_generate_texture_run });
            if (dbg_log) {
                zox_log(" - Vox Texture: %s %i", zox_get_name(texture), j);
            }
        }
    }
} zox_sys_end(BakeVoxSystem);

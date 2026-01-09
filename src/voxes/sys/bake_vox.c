zox_sys2(BakeVoxSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ModelLink);
    zox_sys_in(TextureLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ModelLink, model);
        zox_sys_i(TextureLinks, textures);
        if (!zox_valid(model->value) || !zox_has(model->value, GenerateVox)) {
            continue;
        }
        zox_geter_value(model->value, GenerateVox, byte, state);
        if (state != zox_dirty_end) { // zox_dirty_active) {
            continue;
        }

        // zox_sys_e();
        // zox_log("Block is Generating: %s %i", zox_get_name(e), textures->length);

        for (int j = 0; j < textures->length; j++) {
            const entity texture = textures->value[j];
            if (!zox_valid(texture)) {
                zox_log_error("block texture is invalid [%i]", j);
                continue;
            }
            zox_set(texture, GenerateTexture, { zox_dirty_trigger });
        }
    }
} zox_sys_end(BakeVoxSystem);
// NOTE: Triggers texture generation when the layout size changes
void TextureSizeGenerateSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_out(GenerateTexture);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(GenerateTexture, generate);
        generate->value = zox_generate_texture_run;
    }
    zox_sys_on_end();
} zoxd_system(TextureSizeGenerateSystem);

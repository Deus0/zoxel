// a restored texture uploads its data again
void gpu_restore_textures(ecs* world) {
    zox_each_add(TextureData, TextureDirty);
}

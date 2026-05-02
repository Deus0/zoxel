void clear_texture_data(ecs *world, const entity e) {
    if (!zox_valid(e) || !zox_has(e, TextureData)) {
        return;
    }
    zox_mut_begin(e, TextureData, old);
    if (old->value) {
        dispose_TextureData(old);
        zox_mut_end(e, TextureData);
    }
}

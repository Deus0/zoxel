// called when resolution setting changed
void on_terrain_settings_changed(ecs *world, const entity realm) {
    if (!zox_has(realm, BlockLinks)) return;
    const BlockLinks *voxels = zox_get(realm, BlockLinks);
    for (int k = 0; k < voxels->length; k++) {
        const entity voxel = voxels->value[k];
        zox_geter(voxel, TextureLinks, textures);
        for (int l = 0; l < textures->length; l++) {
            zox_setv(textures->value[l], GenerateTexture, 1);
        }
    }
    entity tilemap = zox_get_link(world, realm, Tilemap);
    zox_setv(tilemap, GenerateTexture, zox_dirty_trigger);
}

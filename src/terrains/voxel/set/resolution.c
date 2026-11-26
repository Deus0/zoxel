// called when resolution setting changed
void on_terrain_settings_changed(
    ecs *world,
    entity realm)
{
    if (!zox_has(realm, BlockLinks)) return;
    entity tilemap = zox_get_link(world, realm, Tilemap);
    zox_setv(tilemap, GenerateTexture, zox_dirty_trigger);
}

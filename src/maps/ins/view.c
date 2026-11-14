// a map's view of the land: one texture filling its body (inside its margins), (2 zoom + 1) tunks across, a texel
//  one block or more (no more texels than pixels), drawn by map_view_system
entity spawn_map_view(ecs* world, entity body, entity terrain, byte zoom, int2 size, byte layer, float alpha) {
    const int tunk_length = octree_size(zox_getv(terrain, NodeDepth));
    const int blocks = (2 * zoom + 1) * tunk_length;
    const int pixels = int_max(1, int_min(size.x, size.y));
    const byte blocks_per_texel = (byte) int_max(1, (blocks + pixels - 1) / pixels);
    const entity e = spawn_ui(world, prefab_element, body, float2_half, int2_zero, size, int2_zero);
    zox_set_unique_name(e, "map_view");
    zox_add(e, MapView);
    zox_setv(e, MapBlocksPerTexel, blocks_per_texel);
    zox_setv(e, MapDrawnAt, int2_hidden);
    zox_setv(e, MapDrawnVersion, -1);
    zox_setv(e, MapDrawnWhole, 0);
    zox_setv(e, MapDrawnTime, 0);
    zox_setv(e, Layer, layer);
    zox_setv(e, Alpha, alpha);
    return e;
}

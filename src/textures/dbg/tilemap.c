extern entity local_terrain;
extern entity prefab_element_shell;
extern entity spawn_inspector(ecs*, entity, entity, entity);
extern entity spawn_element_texture(ecs *world, entity p, entity canvas, entity parent, int2 position, int2 size, float2 anchor, byte layer, entity t);

entity dbg_tilemap = 0;

void zox_tst_spawn_tilemap(ecs *world) {
    if (dbg_tilemap) {
        zox_log("- removing tilemap ui");
        zox_delete(dbg_tilemap);
        dbg_tilemap = 0;
        spawn_sound_from_file_index(world, prefab_sound, 0);
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity terrain = local_terrain;
    if (!zox_valid(terrain)) {
        return;
    }
    entity canvas = get_linked_canvas(world, player);
    zox_geter_value(terrain, TilemapLink, entity, tilemap);
    int2 size = int2_single(512);
    int2 position = int2_single(0);
    float2 anchor = float2_half;
    entity parent = canvas;
    byte layer = 2;
    entity texture = tilemap;
    // texture = string_hashmap_get(files_hashmap_textures, new_string_data("cursor_01"));
    // our logic stuff
    zox_log("+ spawning tilemap ui [%s] on canvas [%s]", zox_get_name(tilemap), zox_get_name(canvas));
    // entity e = spawn_element_texture(world, prefab_element_shell, canvas, texture, position, size);
    entity e = spawn_element_texture(world, prefab_element_shell, canvas, parent, position, size, anchor, layer, texture);
    zox_set(e, RenderDisabled, { 0 });
    zox_set(e, MeshAlignment, { 0 });
    zox_set(e, LayoutSize, { size });
    zox_set(e, LayoutSizeDirty, { zox_dirty_trigger });
    zox_set(e, LayoutPositionDirty, { zox_dirty_trigger });
    spawn_sound_from_file_index(world, prefab_sound, 0);
    dbg_tilemap = e;
}

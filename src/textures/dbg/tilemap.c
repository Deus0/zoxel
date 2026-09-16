extern entity local_terrain;
extern entity prefab_element;
extern entity spawn_inspector(ecs*, entity, entity, entity);
extern entity spawn_element_texture(ecs*, entity, entity, int2, int2, float2, byte, entity);

entity dbg_tilemap = 0;

void zox_tst_spawn_tilemap(ecs *world) {
    if (dbg_tilemap) {
        zox_log("- removing tilemap ui");
        zox_delete(dbg_tilemap);
        dbg_tilemap = 0;
        spawn_sound_from_file_index(
            world,
            prefab_sound,
            0);
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        zox_loge("[Tilemap] Invalid Player");
        return;
    }
    entity game = zox_get_parent_by_id(
        world,
        player,
        zox_id(Game));
    if (!zox_valid(game)) {
        zox_loge("[Tilemap] Invalid Game on (%s)",
            zox_getn(player));
        return;
    }
    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        zox_loge("[zox_tst_spawn_tilemap] Invalid [realm]");
        return;
    }
    /*entity realm = zox_get_child_by_id(
        world,
        game,
        zox_id(Realm));
    if (!zox_valid(realm)) {
        zox_loge("[Tilemap] Invalid Realm on (%s)",
            zox_getn(player));
        return;
    }*/
    entity tilemap = zox_get_child_by_id(
        world,
        realm,
        zox_id(Tilemap));
    if (!zox_valid(tilemap)) {
        zox_loge("[Tilemap] Invalid Tilemap on (%s)",
            zox_getn(realm));
        return;
    }
    /*entity terrain = local_terrain;
    if (!zox_valid(terrain)) {
        return;
    }*/
    entity canvas = get_linked_canvas(world, player);
    int2 size = int2_single(512);
    int2 position = int2_single(0);
    float2 anchor = float2_half;
    byte layer = max_layers2D - 10;
    entity texture = tilemap;
    // texture = string_hashmap_get(files_hashmap_textures, new_string_data("cursor_01"));
    // our logic stuff
    zox_log("+ spawning tilemap ui [%s] on canvas [%s]",
        zox_getn(tilemap),
        zox_getn(canvas));
    // entity e = spawn_element_texture(world, prefab_element, canvas, texture, position, size);
    entity e = spawn_element_texture(
        world,
        prefab_element,
        canvas,
        position,
        size,
        anchor,
        layer,
        texture);
    // entity e = spawn_ui(world, entity prefab_element, canvas, anchor, position, size, int2 texture_size);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, MeshAlignment, 0);
    zox_setv(e, LayoutSize, size);
    zox_setv(e, LayoutSizeDirty, zox_dirty_trigger);
    zox_setv(e, LayoutPositionDirty, zox_dirty_trigger);
    spawn_sound_from_file_index(
        world,
        prefab_sound,
        0);
    dbg_tilemap = e;
}

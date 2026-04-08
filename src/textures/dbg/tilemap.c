extern entity local_terrain;
extern entity prefab_element_shell;
extern entity spawn_inspector(ecs*, entity, entity, entity);
extern entity spawn_element_texture(ecs *world, entity p, entity canvas, entity parent, int2 position, int2 size, float2 anchor, byte layer, entity t);

entity tilemap_ui = 0;

void spawn_tilemap_ui(ecs *world, int32_t keycode) {

    if (keycode != SDLK_9) {
        return;
    }

    if (tilemap_ui) {
        zox_log("- removing tilemap ui");

        zox_delete(tilemap_ui);

        tilemap_ui = 0;
    } else {

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

        entity p = prefab_element_shell;
        int2 size = int2_single(512);
        int2 position = int2_single(0);
        float2 anchor = float2_half;
        entity parent = canvas;
        byte layer = 2;

        entity texture = tilemap;

        // texture = string_hashmap_get(files_hashmap_textures, new_string_data("cursor_01"));

        // our logic stuff
        zox_log("+ spawning tilemap ui [%s] on canvas [%s]", zox_get_name(tilemap), zox_get_name(canvas));

        // entity e = spawn_element_texture(world, p, canvas, texture, position, size);

        entity e = spawn_element_texture(world, p, canvas, parent, position, size, anchor, layer, texture);

        tilemap_ui = e;

        zox_set(e, RenderDisabled, { 0 });
        zox_set(e, MeshAlignment, { 0 });
        zox_set(e, LayoutSize, { size });
        zox_set(e, LayoutSizeDirty, { zox_dirty_trigger });
        zox_set(e, LayoutPositionDirty, { zox_dirty_trigger });
        spawn_gpu_texture(world, e);

        spawn_inspector(world, canvas, player, e);

    }

    spawn_sound_from_file_index(world, prefab_sound, 0);
}

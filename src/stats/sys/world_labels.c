// World Labels
const color label3D_font_color_fill = { 122, 222, 199, 255 };
const color label3D_font_color_outline = { 155, 12, 12, 0 };

// TODO: Sometimes this doesnt spawn!

// name labels spawned a frame at most: a label is a glyph entity per letter, npcs streaming in together
//  spawned their labels in one frame (lag spikes), the rest keep SpawnNameLabel for next frame
#define name_labels_per_frame_max 3
// name labels only near a player's camera (any player, local coop): shown within show, deleted past hide
//  (a little further, so walking the edge doesn't spawn and delete them), far npcs carry no label trees
float name_label_show_distance = 16.0f;
float name_label_hide_distance = 20.0f;
uint name_labels_frame = UINT32_MAX;
uint name_labels_this_frame = 0;

// NOTE: Spawns a name label above character
void CharacterNameLabelsSystem(iter* it) {
    zox_sys_on_begin();
    if (disable_npc_uis) {
        return;
    }
    if (name_labels_frame != ecs_run_count) {
        name_labels_frame = ecs_run_count;
        name_labels_this_frame = 0;
    }
    byte dbg_log = 0;
    color fill = (color) { 0, 0, 0, 155 };
    color outline = (color) { 55, 55, 55, 155 };
    color font_fill = (color) { 255, 255, 255, 233 };
    color font_outline = (color) { 255, 255, 255, 233 };
    float ui_scale = zox_ui_scale3 * 1.6f   ;
    zox_sys_world();
    // the players' cameras
    float3 cameras[8];
    int cameras_count = 0;
    byte playing = 0;
    zox_sys_query();
    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        for (int j = 0; j < it2.count && cameras_count < 8; j++) {
            const entity game = zox_get_link(world, it2.entities[j], GameLink);
            const byte game_state = zox_valid(game) ? zox_getv(game, GameState) : zox_game_none;
            playing |= game_state == zox_game_state_playing || game_state == zox_game_state_paused;
            const entity camera = zox_get_link(world, it2.entities[j], CameraLink);
            if (zox_valid(camera) && zox_has(camera, Position3D)) {
                cameras[cameras_count++] = zox_getv(camera, Position3D);
            }
        }
    }
    zox_sys_query_end();
    // only while playing: not in menus or loading, and not while a game ends (its realm and characters
    //  are deleted that frame, a label spawned on them was left half made, tst menu_flow)
    if (!cameras_count || !playing) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(ZoxName);
    zox_sys_in(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        zox_sys_i(Position3D, position);
        if (!strlen(name->value)) {
            continue;
        }
        float closest = -1;
        for (int j = 0; j < cameras_count; j++) {
            const float distance = float3_distance(position->value, cameras[j]);
            if (closest < 0 || distance < closest) {
                closest = distance;
            }
        }
        const entity label = zox_get_link(world, e, NameLabelLink);
        if (zox_valid(label)) {
            if (closest < 0 || closest > name_label_hide_distance) {
                zox_delete(label);
            }
            continue;
        }
        if (closest < 0 || closest > name_label_show_distance) {
            continue;
        }
        if (name_labels_this_frame >= name_labels_per_frame_max) {
            continue;
        }
        entity soul = zox_get_child_by_id(world, e, zox_id(StatSoul));
        if (!zox_valid(soul)) {
            continue;
        }
        zox_geter_value(soul, StatValue, float, soul_value);
        // "Dave Lvl 3"
        char text[128];
        int souli = floorf(soul_value);
        // zox_log("+ soul [%i]", souli)
        sprintf(text, "%s lvl %i", name->value, souli);
        entity2 e2 = spawn_label3(
            world, text,
            text3D_resolution,
            fill,
            outline,
            font_fill,
            font_outline,
            ui_scale,
            e,
            name_trail_offset);
        zox_set_unique_name(e2.x, "name_label3");
        zox_setv(e2.y, OutlineThickness, 1);
        zox_link(world, e, ElementLinks, e2.x);
        zox_link(world, e, NameLabelLink, e2.x);
        name_labels_this_frame++;
        if (dbg_log) {
            zox_log("Spawned Name Label on [%s]: %s", zox_getn(e), text);
        }
    }
    zox_sys_on_end();
} zoxd_system(CharacterNameLabelsSystem);

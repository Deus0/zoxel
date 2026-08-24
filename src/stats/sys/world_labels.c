// World Labels
const color label3D_font_color_fill = { 122, 222, 199, 255 };
const color label3D_font_color_outline = { 155, 12, 12, 0 };

// TODO: Sometimes this doesnt spawn!

// NOTE: Spawns a name label above character
zox_sys2(CharacterNameLabelsSystem) {
    if (disable_npc_uis) {
        return;
    }
    byte dbg_log = 0;
    color fill = (color) { 0, 0, 0, 155 };
    color outline = (color) { 55, 55, 55, 155 };
    color font_fill = (color) { 255, 255, 255, 233 };
    color font_outline = (color) { 255, 255, 255, 233 };
    float ui_scale = zox_ui_scale3 * 1.6f   ;
    zox_sys_world();
    zox_sys_begin();
    // zox_sys_in(GenerateCharacter);
    zox_sys_in(ZoxName);
    zox_sys_out(ElementLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // zox_sys_i(GenerateCharacter, state);
        zox_sys_i(ZoxName, name);
        zox_sys_o(ElementLinks, elements);
        /*if (state->value != zox_dirty_active) {
            continue;
        }*/
        if (!strlen(name->value)) {
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
        zox_setv(e2.x, ElementHolder, e);
        zox_setv(e2.y, OutlineThickness, 1);
        add_to_ElementLinks(elements, e2.x);
        zox_remove(e, SpawnNameLabel);
        if (dbg_log) {
            zox_log("Spawned Name Label on [%s]: %s", zox_getn(e), text);
        }
    }
} zox_sys_end(CharacterNameLabelsSystem);

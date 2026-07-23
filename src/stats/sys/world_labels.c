// World Labels
const color label3D_font_color_fill = { 122, 222, 199, 255 };
const color label3D_font_color_outline = { 155, 12, 12, 0 };

// NOTE: Spawns a name label above character
zox_sys2(CharacterNameLabelsSystem) {
    if (disable_npc_uis) {
        return;
    }
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(ZoxName);
    zox_sys_out(ElementLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(ZoxName, name);
        zox_sys_o(ElementLinks, elements);
        if (state->value != zox_dirty_end) {
            continue;
        }
        entity soul = zox_get_child_by_id(world, e, zox_id(StatSoul));
        if (!zox_valid(soul)) {
            continue;
        }
        zox_geter_value(soul, StatValue, float, soul_value);
        SpawnDataElement3 label3D_spawn_data = {
            .ui_holder = e,
            .prefab = prefab_label3D,
            .trail_offset = name_trail_offset,
            .base_color = (color) { 0, 0, 0, 188 },
            .outline_color = (color) { 255, 255, 255, 222 }
        };
        // "Dave Lvl 3"
        char result[128];
        int souli = floorf(soul_value);
        // zox_log("+ soul [%i]", souli)
        sprintf(result, "%s lvl %i", name->value, souli);
        Text3DData label3D_text_data = {
            .prefab = prefab_text3D,
            .text = result,
        };
        Zigel3DData label3D_zigel_data = {
            .prefab = prefab_zigel3D,
            .font_thickness = text3D_fill_thickness,
            .font_outline = text3D_outline_thickness,
            .resolution = text3D_resolution,
            .fill_color = label3D_font_color_fill,
            .outline_color = label3D_font_color_outline
        };
        entity2 e2 = spawn_label3D(world, label3D_spawn_data, label3D_text_data, label3D_zigel_data, (float3) { 0, name_trail_offset, 0 });
        zox_set_unique_name(e2.x, "name_label3");
        zox_set(e2.x, ElementHolder, { e });
        add_to_ElementLinks(elements, e2.x);
        if (dbg_log) {
            zox_log("Spawned Name Label on [%s]: %s", zox_getn(e), result);
        }
    }
} zox_sys_end(CharacterNameLabelsSystem);

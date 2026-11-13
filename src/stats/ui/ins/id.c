// a character's id: who they are, a line each (its id window, below)
//  - its name and level (its soul stat), then its title, race, class, job (jrpg style, apart from its class) and clan:
//    the name of what each links to (TitleLink, RaceLink, ClassLink, JobLink, ClanLink), none until they're made
static inline const char* character_id_link(ecs* world, entity character, entity link_id) {
    const entity data = zox_get_link_internal(world, character, link_id);
    return zox_valid(data) && zox_has(data, ZoxName) ? zox_getv(data, ZoxName) : "none";
}

void character_id_text(ecs* world, entity character, char* text, int size) {
    // the player's character has no name yet (todo), none like the rest
    const char* name = zox_has(character, ZoxName) && zox_getv(character, ZoxName)[0] ? zox_getv(character, ZoxName) : "none";
    const entity soul = zox_get_child_by_id(world, character, zox_id(StatSoul));
    const int level = zox_valid(soul) ? (int) floorf(zox_getv(soul, StatValue)) : 0;
    snprintf(text, size, "name %s\nlevel %i\ntitle %s\nrace %s\nclass %s\njob %s\nclan %s",
        name, level,
        character_id_link(world, character, zox_id(TitleLink)),
        character_id_link(world, character, zox_id(RaceLink)),
        character_id_link(world, character, zox_id(ClassLink)),
        character_id_link(world, character, zox_id(JobLink)),
        character_id_link(world, character, zox_id(ClanLink)));
}

// the player's id window: who their character is (character_id_text), a line each, sized to its lines
entity spawn_player_menu_id(ecs* world, entity player) {
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(canvas) || !zox_valid(character)) {
        return 0;
    }
    char text[256];
    character_id_text(world, character, text, sizeof(text));
    const byte font_size = 8 * ui_scale;
    const byte2 padding = (byte2) { 4 * ui_scale, 4 * ui_scale };
    const int margin = 6 * ui_scale;
    // its lines' size (text uses font size wide glyphs, line padding between lines)
    int columns = 0, rows = 1, width = 0;
    for (int i = 0; text[i]; i++) {
        if (text[i] == '\n') {
            rows++;
            width = 0;
        } else if (++width > columns) {
            columns = width;
        }
    }
    const int2 size = {
        columns * font_size + (padding.x + margin) * 2,
        rows * font_size + (rows - 1) * default_line_padding + (padding.y + margin) * 2
    };
    const entity3 window = spawn_window(world, prefab_window, prefab_body, "Identity", canvas, int2_zero, size,
        float2_half, 6 * ui_scale, (byte2) { 6 * ui_scale, 3 * ui_scale }, NULL);
    zox_add(window.x, MenuId);
    spawn_text(world, prefab_text, window.z, (int2) { -size.x / 2 + margin, size.y / 2 - margin }, float2_half,
        font_size, zox_alignment_top_left, padding, text, button_font_fill, button_font_outline);
    ui_pop_in(world, window.x);
    return window.x;
}

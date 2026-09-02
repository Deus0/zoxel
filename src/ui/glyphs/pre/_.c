entity prefab_zigel;
entity zox_font_style_monocraft;
#include "font.c"
#include "font_style.c"
#include "glyph.c"
#include "ttf.c"
entity prefab_font_style;
entity prefab_font;
entity zox_font_style_default;

entity get_font_style_using() {
    if (zox_font_style_monocraft) {
        return zox_font_style_monocraft;
    }
    return zox_font_style_default;
}

int get_zigels_count(ecs *world) {
    return zox_count_types(Glyph);
}

void zox_spawn_prefabs_zigels(ecs *world) {
    prefab_font = spawn_font_prefab(world);
    prefab_font_style = spawn_prefab_font_style(
        world,
        prefab_font);
    prefab_zigel = spawn_prefab_glyph(
        world,
        prefab_element);
}

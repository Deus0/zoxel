#ifndef zoxm_colors
#define zoxm_colors

// globals for now...
byte grayscale_mode = 0;
color_rgb viewport_clear_color = { 0, 0, 0 };
zoxc_color(Color);
zoxc_color(SecondaryColor);
zoxc_color_rgb(ColorRGB);
zoxc_color_rgb(SecondaryColorRGB);
zoxc_color_rgb(FogColor);
zoxc_arrayd(Colors, color)
zoxc_arrayd(ColorRGBs, color_rgb)
#include "fun/_.c"
#include "dat/_.c"
#include "sys/_.c"

/*#define zox_biome_grassland 0
#define zox_biome_desert 1
#define zox_biome_savanna 2
#define zox_biome_forest 3
#define zox_biome_swamp 4
#define zox_biome_mountain 5
#define zox_biome_end 6*/

// hmm figure this out later
// extern void set_prefab_debug_label(ecs_world_t *world, DebugLabelEvent value);

void spawn_prefabs_colors(ecs* world) {
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, Colors)
    }
}

zox_begin_module(Colorz)
    zox_define_component_color(Color);
    zox_define_component_color(SecondaryColor);
    zox_define_component_color_rgb(ColorRGB);
    zox_define_component_color_rgb(SecondaryColorRGB);
    zox_define_component_color_rgb(FogColor);
    zoxd_arrayd(ColorRGBs);
    zoxd_arrayd(Colors);
    define_systems_colors(world);
    add_hook_spawn_prefabs(spawn_prefabs_colors);
zox_end_module(Colorz)

#endif
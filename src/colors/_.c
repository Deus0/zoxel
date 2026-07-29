#ifndef zoxm_colors
#define zoxm_colors

// globals for now...
byte grayscale_mode = 0;
color viewport_clear_color = { 0, 0, 0, 255 };
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

void spawn_prefabs_colors(ecs* world) {
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, Colors)
    }
}

zox_begin_module(Colorz) {
    zoxd_color(Color);
    zoxd_color(SecondaryColor);
    zoxd_color_rgb(ColorRGB);
    zoxd_color_rgb(SecondaryColorRGB);
    zoxd_color_rgb(FogColor);
    zoxd_arrayd(ColorRGBs);
    zoxd_arrayd(Colors);
    define_systems_colors(world);
    add_hook_spawn_prefabs(spawn_prefabs_colors);
} zox_end_module(Colorz);

#endif

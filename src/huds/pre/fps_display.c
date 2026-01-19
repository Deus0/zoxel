entity spawn_prefab_fps_display(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("fps_display");
    zox_add_tag(e, FPSDisplay);
    zox_prefab_set(e, FPSDisplayTicker, { 0 });
    zox_prefab_set(e, FontOutlineColor, {{ 200, 80, 80, 255 }});
    zox_prefab_set(e, FontFillColor, {{ 244, 33, 33, 255 }});
    return e;
}

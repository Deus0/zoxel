byte fullscreen = 1;
extern entity main_app;
extern void zox_set_app_fullscreen(ecs* world, entity e, byte fullscreen);

// when setting is set
void set_app_fullscreen(ecs* world, void* value) {
    fullscreen = *(byte*) value;
    if (zox_valid(main_app)) {
        zox_set_app_fullscreen(world, main_app, fullscreen);
    }
}

// use our setting
void zox_toggle_fullscreen(ecs *world) {
    zoxs_set_byte(world, "fullscreen", !fullscreen);
}

void toggle_sound(ecs *world, int32_t keycode) {
    (void) world;
    if (keycode == zox_key_m) {
        nosounds = !nosounds;
        zox_log("Sound %s", nosounds ? "Disabled" : "Enabled");
    }
}

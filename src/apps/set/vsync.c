byte vsync = 1;

void zox_set_vsync(ecs *world, byte value) {
    zoxs_set_byte(world, "vsync", value);
}

/*void zox_set_vsync_silently(ecs *world, const entity e, byte value) {
    vsync = value;
    on_set_vsync(vsync);
    zoxs_set_byte_silently(world, "vsync", value);
}*/

extern void on_set_vsync(byte);

// when setting is set
void set_app_vsync(ecs* world, void* value) {
    vsync = *(byte*) value;
    on_set_vsync(vsync);
}
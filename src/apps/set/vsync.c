byte vsync = 1;

extern void on_set_vsync(byte);

/*void zox_set_vsync(ecs *world, byte value) {
    zoxs_set_byte(world, "vsync", value);
}*/

// when setting is set
void set_app_vsync(ecs* world, void* value) {
    vsync = *(byte*) value;
    on_set_vsync(vsync);
}

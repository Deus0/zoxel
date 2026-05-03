byte vsync = 1;

extern void on_set_vsync(byte);

// when setting is set
void set_app_vsync(ecs* world, void* value) {
    vsync = *(byte*) value;
    on_set_vsync(vsync);
}

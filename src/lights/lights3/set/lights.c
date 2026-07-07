byte disable_lights = 0;

void on_set_disable_lights(ecs* world, void* value) {
    (void) world;
    byte new_value = *(byte*) value;
    disable_lights = new_value;
}

void initialize_settings_lights3(ecs *world) {
    zoxs_new_byte("disable lights", on_set_disable_lights, disable_lights);
}

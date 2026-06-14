extern void zox_app_set_monitor_e(ecs*, entity, byte);

byte is_log_monitors = 1;
byte monitor = 0;

// use our setting
void zox_set_monitor(ecs *world, byte value) {
    zoxs_set_byte(world, "monitor", value);
}

void zox_set_monitor_silently(ecs *world, entity e, byte value) {
    monitor = value;
    zoxs_set_byte_silently(world, "monitor", value);
    zox_set(e, WindowMonitor, { value })
}

// when setting is set
void set_app_monitor(ecs* world, void* value) {
    monitor = *(byte*) value;
    if (zox_valid(main_app)) {
        zox_app_set_monitor_e(world, main_app, monitor);
    }
}

entity debug_entity_text3D;

void key_down_update_text3D(ecs *world) {
    entity e = debug_entity_text3D;
    if (!e) {
        return;
    }
    zox_log("> setting text3D %s", zox_get_name(e))
    set_entity_text(world, e, "Hello World");
}

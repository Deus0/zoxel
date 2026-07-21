entity dbg_text3;

void key_down_update_text3D(ecs *world) {
    if (dbg_text3) {
        zox_delete(dbg_text3);
        return;
    }
    // zox_log("> setting text3D %s", zox_get_name(e))
    // set_entity_text(world, e, "Hello World");
    // dbg_text3 = e;
}

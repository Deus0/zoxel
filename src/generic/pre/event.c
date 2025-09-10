entity spawn_prefab_generic_event(ecs* world) {
    zox_prefab();
    zox_prefab_name("generic_event");
    zox_add_tag(e, DestroyInFrame);
    zox_prefab_set(e, GenericEvent, { 0 });
    return e;
}

entity spawn_generic_event(ecs* world, byte state) {
    zox_instance(prefab_event);
    zox_name("event");
    zox_set(e, GenericEvent, { state });
    return e;
}

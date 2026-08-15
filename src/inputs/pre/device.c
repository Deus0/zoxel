entity spawn_prefab_device(ecs *world) {
    zox_prefab();
    zox_prefab_name("device");
    zox_add(e, Device);
    zox_prefab_set(e, DeviceDisabled, { 1 });
    return e;
}

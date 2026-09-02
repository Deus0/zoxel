entity spawn_prefab_device(ecs *world) {
    zox_prefab();
    zox_prefab_name("device");
    zox_add(e, Device);
    zox_setv(e, DeviceDisabled, 1);
    zox_setv(e, DeviceHasInput, 0);
    return e;
}

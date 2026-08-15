entity spawn_prefab_device_button(ecs *world) {
    zox_prefab();
    zox_prefab_name("zevice_button");
    zox_add(e, Zevice);
    zox_prefab_set(e, ZeviceDisabled, { 0 });
    zox_prefab_set(e, ZeviceButton, { 0 });
    zox_prefab_set(e, DeviceButtonType, { 0 });
    zox_prefab_set(e, RealButtonIndex, { 0 });
    return e;
}

entity spawn_device_button(ecs *world, entity prefab, entity device, byte index, byte rindex) {
    zox_instance(prefab);
    zox_name("zevice_button");
    zox_set_parent(world, e, device);
    zox_set(e, DeviceButtonType, { index });
    zox_set(e, RealButtonIndex, { rindex });
    return e;
}

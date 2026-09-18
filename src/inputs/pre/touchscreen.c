entity spawn_prefab_touchscreen(
    ecs *world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("touchscreen");
    zox_add(e, Touchscreen);
    zox_add(e, PixelSize);
    zox_add(e, DeviceLayout);
    return e;
}
entity spawn_prefab_finger(
    ecs* world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("finger");
    zox_add(e, Finger);
    zox_add(e, Disabled);
    zox_prefab_setv(e, ID, 0);
    zox_prefab_setv(e, DeviceLayout, 0);
    zox_prefab_setv(e, PixelSize, int2_zero);
    return e;
}
entity spawn_prefab_label(
    ecs *world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("label");
    zox_add(e, Label);
    zox_add(e, TextResizer);
    zox_add(e, FixToLayout);
    return e;
}

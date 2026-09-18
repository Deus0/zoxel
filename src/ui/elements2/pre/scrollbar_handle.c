entity spawn_prefab_scrollbar_handle(
    ecs *world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("scrollbar_handle");
    zox_add(e, Scrollbar);
    zox_setv(e, ScrollviewLink, 0);
    return e;
}

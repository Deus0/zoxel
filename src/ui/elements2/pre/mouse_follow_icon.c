entity icon_mouse_follow;

entity spawn_prefab_icon_mouse_follow(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_set(e, RenderDisabled, { 1 });
    zox_add_tag(e, MouseElement);
    zox_prefab_set(e, ZeviceLink, { 0 });
    return e;
}

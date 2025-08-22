entity spawn_prefab_icon_user(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("icon_user");
    zox_add_tag(e, UserIcon);
    zox_prefab_set(e, UserDataLink, { 0 });
    zox_prefab_add(e, TooltipEvent);
    zox_remove(e, GenerateTexture);
    return e;
}
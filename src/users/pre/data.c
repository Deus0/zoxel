entity spawn_prefab_userd(ecs *world) {
    zox_prefab();
    zox_prefab_name("data");
    zox_prefab_add(e, ZoxName);
    zox_prefab_set(e, UserLink, { 0 });
    zox_prefab_set(e, TextureLink, { 0 });
    zox_prefab_set(e, ActivateBegin, { 0 });
    zox_prefab_set(e, Activate, { 0 });
    zox_prefab_set(e, UserDataDirty, { 0 });
    return e;
}
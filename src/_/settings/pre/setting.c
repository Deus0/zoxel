entity spawn_prefab_setting(ecs* world) {
    zox_prefab();
    zox_prefab_name("setting");
    zox_add_tag(e, Setting);
    zox_set(e, SettingDirty, { 0 });
    zox_prefab_set(e, ZoxName, { 0 });
    return e;
}

entity spawn_setting(ecs *world, entity prefab, entity parent, const char* name) {
    zox_instance(prefab);
    zox_name("setting");
    zox_set_parent(world, e, parent);
    set_ZoxName(world, e, name);
    return e;
}

entity spawn_setting_byte(ecs* world, entity parent, const char* name, byte value) {
    entity e = spawn_setting(world, prefab_setting, parent, name);
    zox_set(e, SettingByte, { value });
    return e;
}

entity spawn_setting_byte_slider(ecs* world, entity parent, const char* name, byte value, byte2 bounds) {
    entity e = spawn_setting(world, prefab_setting, parent, name);
    zox_set(e, SettingByte, { value });
    zox_add(e, SettingSlider);
    zox_set(e, SettingByteBounds, { bounds });
    return e;
}

entity spawn_setting_float(ecs* world, entity parent, const char* name, float value, float2 bounds) {
    entity e = spawn_setting(world, prefab_setting, parent, name);
    zox_set(e, SettingFloat, { value });
    zox_set(e, SettingFloatBounds, { bounds });
    return e;
}

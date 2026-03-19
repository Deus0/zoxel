entity spawn_setting(ecs *world, entity p, const char* name) {

    zox_instance(p);
    zox_name("setting");

    set_ZoxName(world, e, name);

    // add to manager
    if (zox_valid(settings_manager)) {
        zox_muter(settings_manager, SettingLinks, settings);
        add_to_SettingLinks(settings, e);
    }

    return e;
}

entity spawn_setting_byte(ecs* world, entity p, const char* name, byte value) {
    entity e = spawn_setting(world, p, name);

    zox_set(e, SettingByte, { value });

    return e;
}

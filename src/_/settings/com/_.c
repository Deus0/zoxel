zox_tag(Setting);
zoxc_entities(SettingLinks);
zoxc_state(SettingDirty);
zoxc_byte(SettingByte);
zoxc_int(SettingInt);
zoxc_float(SettingFloat);

void define_components_settings(ecs* world) {
    zoxd_tag(Setting);
    zoxd_entities(SettingLinks);
    zoxd_state(SettingDirty);
    zoxd_byte(SettingByte);
    zoxd_int(SettingInt);
    zoxd_float(SettingFloat);
}

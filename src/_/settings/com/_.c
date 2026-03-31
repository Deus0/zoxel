zox_tag(Setting);
zoxc_state(SettingDirty);
zoxc_byte(SettingByte);
zoxc_int(SettingInt);
zoxc_float(SettingFloat);
zoxc_entities(SettingLinks);

void define_components_settings(ecs* world) {
    zoxd_tag(Setting);
    zoxd_state(SettingDirty);
    zoxd_byte(SettingByte);
    zoxd_int(SettingInt);
    zoxd_float(SettingFloat);
    zoxd_entities(SettingLinks);
}

zox_tag(Setting);
zox_tag(SettingSlider);
zoxc_state(SettingDirty);
zoxc_byte(SettingBit);
zoxc_byte(SettingByte);
zoxc_byte2(SettingByteBounds);
zoxc_int(SettingInt);
zoxc_float(SettingFloat);
zoxc_float2(SettingFloatBounds);
zoxc_entity(SettingLink);

void zox_define_components_settings(ecs* world) {
    zoxd_tag(Setting);
    zoxd_tag(SettingSlider);
    zoxd_state(SettingDirty);
    zoxd_byte(SettingBit);
    zoxd_byte(SettingByte);
    zoxd_byte2(SettingByteBounds);
    zoxd_int(SettingInt);
    zoxd_float(SettingFloat);
    zoxd_float2(SettingFloatBounds);
    zoxd_entity(SettingLink);
}

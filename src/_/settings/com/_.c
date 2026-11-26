zox_tag(Setting);
// its type, a tag each: a toggle (a byte on or off), a slider of a byte or a float in its bounds
zox_tag(SettingToggle);
zox_tag(SettingByteSlider);
zox_tag(SettingFloatSlider);
// the options menu's tab a setting shows on (huds/ins/settings.c), one each
zox_tag(SettingTabVideo);
zox_tag(SettingTabSound);
zox_tag(SettingTabWorld);
zox_tag(SettingTabGame);
zox_tag(SettingTabDebug);
// can't be used here (i.e. a backend without it): the options menu shows it greyed, not toggled, a tooltip says why
zox_tag(SettingUnsupported);
zoxc_byte(SettingBit);
zoxc_byte(SettingByte);
zoxc_byte2(SettingByteBounds);
zoxc_int(SettingInt);
zoxc_float(SettingFloat);
zoxc_float2(SettingFloatBounds);
zoxc_entity(SettingLink);
zoxc_byte(LoadSettings);
#define zox_load_settings_start 1
#define zox_load_settings_spawn 2
#define zox_load_settings_run 3
#define zox_load_settings_end 0

void zox_components_settings(ecs* world) {
    zoxd_tag(Setting);
    zoxd_tag(SettingToggle);
    zoxd_tag(SettingByteSlider);
    zoxd_tag(SettingFloatSlider);
    zoxd_tag(SettingTabVideo);
    zoxd_tag(SettingTabSound);
    zoxd_tag(SettingTabWorld);
    zoxd_tag(SettingTabGame);
    zoxd_tag(SettingTabDebug);
    zoxd_tag(SettingUnsupported);
    zoxd_byte(SettingBit);
    zoxd_byte(SettingByte);
    zoxd_byte2(SettingByteBounds);
    zoxd_int(SettingInt);
    zoxd_float(SettingFloat);
    zoxd_float2(SettingFloatBounds);
    zoxd_entity(SettingLink);
    zoxd_byte(LoadSettings);
}

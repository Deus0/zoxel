// zox_tag(Player3D)
zox_tag(Character3Player);
zoxc_float3(Movement3);

// its settings, each its own tag (a setting's observer finds it by it, not its name)
zox_tag(SettingAutoJump);
zox_tag(SettingPhysicsTurn);

void zox_components_controllers3D(ecs *world) {
    zoxd_tag(SettingAutoJump);
    zoxd_tag(SettingPhysicsTurn);

    // zoxd_tag(Player3D)
    zoxd_tag(Character3Player);
    zoxd_float3(Movement3);
}
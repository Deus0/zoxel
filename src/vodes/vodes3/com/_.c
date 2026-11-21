zox_tag(WorldBlock);

// its settings, each its own tag (a setting's observer finds it by it, not its name)
zox_tag(SettingNoVodes);

void zox_components_vodes3(ecs* world) {
    zoxd_tag(SettingNoVodes);

    zoxd_tag(WorldBlock);
}

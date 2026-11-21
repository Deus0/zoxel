zoxc(LightLock, spinlock);
zox_tag(GenerateSunlight);
zox_tag(GenerateEmitterLight);  // its voxels are in (generated, loaded): its emitters light (lights3/sys/emitters.c)

// its settings, each its own tag (a setting's observer finds it by it, not its name)
zox_tag(SettingNoAO);
zox_tag(SettingNoLights);
zox_tag(SettingSmoothLighting);

void zoxd_components_lights(ecs* world) {
    zoxd_tag(SettingNoAO);
    zoxd_tag(SettingNoLights);
    zoxd_tag(SettingSmoothLighting);

    zoxd_tag_event(GenerateSunlight);
    zoxd_tag_event(GenerateEmitterLight);
    zoxd(LightLock);
}

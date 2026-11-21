zoxc(LightLock, spinlock);
zox_tag(GenerateSunlight);
zox_tag(GenerateEmitterLight);  // its voxels are in (generated, loaded): its emitters light (lights3/sys/emitters.c)

void zoxd_components_lights(ecs* world) {
    zoxd_tag_event(GenerateSunlight);
    zoxd_tag_event(GenerateEmitterLight);
    zoxd(LightLock);
}

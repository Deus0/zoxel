zoxc(LightLock, spinlock);
zox_tag(GenerateSunlight);

void zoxd_components_lights(ecs* world) {
    zoxd_nf_tag(GenerateSunlight);
    zoxd(LightLock);
}

zox_tag(Cloud);
zoxc_float3(CloudCentre);    // on a realm: where its clouds wrap round (the listening camera), set once it has clouds

void zox_components_sky(ecs* world) {
    zoxd_tag(Cloud);
    zoxd_float3(CloudCentre);
}

zox_tag(Cloud);
zoxc_float3(CloudCentre);
zox_tag(Sun);                // a light source in the sky, a child of its realm (pre/sun.c)
zoxc_float(DayLength);       // seconds of its day, by its realm's clock
zoxc_float(DayStart);        // its time of day as its realm's clock starts: 0 midnight, 0.5 noon    // on a realm: where its clouds wrap round (the listening camera), set once it has clouds

void zox_components_sky(ecs* world) {
    zoxd_tag(Cloud);
    zoxd_float3(CloudCentre);
    zoxd_tag(Sun);
    zoxd_float(DayLength);
    zoxd_float(DayStart);
}

zox_tag(Weather);
zox_tag(Skybox);
// the sky blends from one colour to another (blend_sky_to): SkyBlend 0 to 1, tweened (SkyBlendLink, lerp_float_system)
zoxc_color_rgb(SkyColorFrom);
zoxc_color_rgb(SkyColorTarget);
zoxc_float(SkyBlend);
zox_tag(SkyBlendLink);
zox_tag(SkyBlendStart);     // event: blend to its SkyColorTarget (a worker sets it, the observer starts the tween)

void zox_components_weather(ecs* world) {
    zoxd_tag(Weather);
    zoxd_tag(Skybox);
    zoxd_color_rgb(SkyColorFrom);
    zoxd_color_rgb(SkyColorTarget);
    zoxd_float(SkyBlend);
    zoxd_nf_link(SkyBlendLink);
    zoxd_nf_tag(SkyBlendStart);
}

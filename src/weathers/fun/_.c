void set_skybox_colors(
    ecs *world,
    entity skybox,
    color_rgb top_color,
    color_rgb bottom_color)
{
    if (!zox_valid(skybox)) {
        zox_loge("Skybox Invalid");
        return;
    }
    if (override_sky) {
        top_color = override_sky_fill;
        bottom_color = override_sky_fill;
    }
    zox_setv(skybox, ColorRGB, top_color);
    zox_setv(skybox, SecondaryColorRGB, bottom_color);
    guint material = zox_getv(skybox, MaterialGPULink);
    set_skybox_material_color(material, top_color, bottom_color);
}

void spawn_weather(ecs *world, entity app) {
    entity game = zox_get_child_by_id(world, app, zox_id(Game));
    entity main_camera = main_cameras[0];
    entity skybox = spawn_skybox(world, main_camera, shader_skybox);
    set_skybox_colors(
        world,
        skybox,
        menu_sky_color,
        menu_sky_bottom_color);
    zox_link(world, game, Skybox, skybox);
}

void set_skybox_colors(
    ecs *world,
    entity skybox,
    color_rgb top_color,
    color_rgb bottom_color)
{
    if (!zox_valid(skybox) ||
        !zox_has(skybox, ColorRGB) ||
        !zox_has(skybox, SecondaryColorRGB) ||
        !zox_has(skybox, MaterialGPULink)
    ) {
        zox_loge("[set_skybox_colors] Invalid [skybox]");
        return;
    }
    if (override_sky) {
        top_color = override_sky_fill;
        bottom_color = override_sky_fill;
    }
    guint material = zox_getv(skybox, MaterialGPULink);
    set_skybox_material_color(material, top_color, bottom_color);
    zox_setv(skybox, ColorRGB, top_color);
    zox_setv(skybox, SecondaryColorRGB, bottom_color);
}

void spawn_weather(ecs *world, entity app) {
    if (!zox_valid(app)) {
        zox_loge("[spawn_weather] Invalid [app]");
        return;
    }
    entity game = zox_get_child_by_id(
        world,
        app,
        zox_id(Game));
    if (!zox_valid(game)) {
        zox_loge("[spawn_weather] Invalid [game]");
        return;
    }
    entity camera = main_cameras[0];
    if (!zox_valid(camera)) {
        zox_loge("[spawn_weather] Invalid [game]");
        return;
    }
    entity skybox = spawn_skybox(
        world,
        camera,
        shader_skybox);
    zox_link(world, game, Skybox, skybox);
}

byte last_weather_state;

// NOTE: Used for testing
void refresh_weather(ecs* world) {
    entity game = local_game;
    if (!zox_valid(game)) {
        zox_loge("[refresh_weather] Invalid [game]");
        return;
    }
    entity skybox = zox_get_link(world, game, Skybox);
    if (!zox_valid(skybox)) {
        zox_loge("[refresh_weather] Invalid [skybox]");
        return;
    }
    if (last_weather_state == zox_game_state_play_begin) {
        set_skybox_colors(
            world,
            skybox,
            game_sky_color,
            game_sky_bottom_color);
    } else if (last_weather_state == zox_game_start) {
        set_skybox_colors(
            world,
            skybox,
            menu_sky_color,
            menu_sky_bottom_color);
    }
}

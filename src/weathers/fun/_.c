// the skybox's colours on the gpu and its components, now (main thread)
void apply_skybox_colors(
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

// sets the sky's colours at once (the menu's, a game starting), no blending to them
void set_skybox_colors(
    ecs *world,
    entity skybox,
    color_rgb top_color,
    color_rgb bottom_color)
{
    apply_skybox_colors(world, skybox, top_color, bottom_color);
    if (zox_valid(skybox) && zox_has(skybox, SkyColorTarget)) {
        zox_setv(skybox, SkyColorFrom, top_color);
        zox_setv(skybox, SkyColorTarget, top_color);
        zox_setv(skybox, SkyBlend, 1);
    }
}

const float sky_blend_seconds = 2.5f;

// the sky (and fog) blends from its colour now to sky_color over sky_blend_seconds (sky_blend_system), i.e. walking
//  into another biome: its tween (SkyBlend 0 to 1) restarts if one's running, returns it (it ends with AnimationEnded)
entity blend_sky_to(ecs* world, entity skybox, color_rgb sky_color) {
    if (!zox_valid(skybox) || !zox_has(skybox, SkyColorTarget)) {
        return 0;
    }
    zox_setv(skybox, SkyColorFrom, zox_getv(skybox, ColorRGB));
    zox_setv(skybox, SkyColorTarget, sky_color);
    zox_setv(skybox, SkyBlend, 0);
    entity tween = zox_get_link(world, skybox, SkyBlendLink);
    if (zox_valid(tween) && !zox_has(tween, AnimationEnded)) {
        zox_setv(tween, AnimationStartTime, zox_current_time);
        return tween;
    }
    tween = zox_lerp_float_ins(skybox, SkyBlend, 0, 1, sky_blend_seconds);
    zox_link(world, skybox, SkyBlendLink, tween);
    return tween;
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
    zox_log("SPAWNED SKYBOX");
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
    } else if (last_weather_state == zox_game_none) {
        set_skybox_colors(
            world,
            skybox,
            menu_sky_color,
            menu_sky_bottom_color);
    }
}

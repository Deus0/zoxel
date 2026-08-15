// todo: stretch to parent size!
float app_fade_in_delay_time = 0.25;
float app_fade_in_time = 1.0;
#define canvas_overlay_value 12
color canvas_overlay_color = (color) { canvas_overlay_value, canvas_overlay_value, canvas_overlay_value, 255 };

// TODO: Animate this with new tween system to reduce prefab count
entity spawn_canvas_overlay(
    ecs* world,
    entity prefab,
    entity canvas,
    int2 canvas_size)
{
    entity parent = canvas;
    byte layer = get_game_overlay_layer();
    int2 position = int2_zero;
    float2 position_anchor = float2_half;
    int2 size = canvas_size;
    entity e = spawn_uic(
        world,
        prefab,
        parent,
        position_anchor,
        position,
        size,
        size,
        canvas_overlay_color,
        color_black);
    zox_name("screen_overlay");
    zox_setv(e, Layer, layer);
    zox_setv(e, Alpha, 1);
    zox_setv(e, OutlineThickness, (canvas_size.y + canvas_size.x) * 0.01f);
    trigger_canvas_overlay_fade_out(
        world,
        e,
        app_fade_in_delay_time,
        app_fade_in_time);
    return e;
}

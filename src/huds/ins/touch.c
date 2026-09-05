extern void button_event_switch_action(ecs *world, const ClickEventData event);
extern void button_event_switch_action2(ecs *world, const ClickEventData event);
extern void button_event_jump(ecs *world, const ClickEventData event);
extern void button_event_attack(ecs *world, const ClickEventData event);

entity spawn_menu_game_touch(ecs *world, entity p, entity player, entity canvas) {
    byte touch_buttons_count = 6;
    byte layer = max_layers2D - 10; // 1;
    byte button_size = 66 * ui_scale;
    byte bpadding = 8 * ui_scale;
    byte2 screen_margins = byte2_single(button_size * 0.4f);
    int2 canvas_size = zox_getv(canvas, LayoutSize);
    int2 bsize = int2_single(button_size);
    entity e = spawn_layout2_on_canvas(world, p, canvas, int2_zero, canvas_size, float2_half);
    zox_name("menu_game_touch");
    int2 bposition = (int2) {
        screen_margins.x + button_size / 2,
        screen_margins.y + button_size / 2
    };
    byte blayer = layer + 1;
    color cfill = (color) { 15, 15, 15, 80 };
    color coutline = (color) { 40, 40, 40, 130 };
    char* tnames[] = {
        "taskbar_paused",
        "touch_switch",
        "touch_speak",
        "touch_jump",
        "touch_switch",
        "touch_attack",
    };
    ClickEvent onclicks[] = {
        (ClickEvent) { &button_event_pause_game },
        (ClickEvent) { &button_event_switch_action2 },
        (ClickEvent) { &button_event_speak },
        (ClickEvent) { &button_event_jump },
        (ClickEvent) { &button_event_switch_action },
        (ClickEvent) { &button_event_attack },
    };
    float2 banchors[] = {
        float2_bottom_left,
        float2_bottom_left,
        float2_bottom_left,
        float2_bottom_right,
        float2_bottom_right,
        float2_bottom_right,
    };
    int2 bpositions[] = {
        (int2) { bposition.x, bposition.y },
        (int2) { bposition.x, bposition.y + (button_size + bpadding)  },
        (int2) { bposition.x, bposition.y + 2 * (button_size + bpadding) },
        (int2) { -bposition.x, bposition.y },
        (int2) { -bposition.x, bposition.y + (button_size + bpadding)  },
        (int2) { -bposition.x, bposition.y + 2 * (button_size + bpadding)  },
    };
    for (byte i = 0; i < touch_buttons_count; i++) {
        entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(tnames[i]));
        entity b = spawn_button_icon(world, prefab_button,
            texture,
            canvas,
            e,
            bpositions[i],
            bsize,
            banchors[i],
            blayer,
            cfill,
            coutline,
            0,
            onclicks[i],
            bpadding
        );
        zox_set_parent(world, b, e);
    }
    return e;
}

// called from game state changes
void spawn_in_game_ui_touch(
    ecs *world,
    entity player,
    entity canvas)
{
    if (!zox_valid(canvas)) {
        return;
    }
    entity menu = zox_get_child_by_id(
        world,
        canvas,
        zox_id(MenuPlayTouch));
    if (zox_valid(menu)) {
        zox_log("touch ui already exists");
        return;
    }
    spawn_menu_game_touch(world, prefab_menu_play_touch, player, canvas);
}

void dispose_menu_game_touch(ecs *world, entity player) {
    if (!zox_valid(player)) {
        return;
    }
    entity canvas = zox_get_link(world, player, Canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuPlayTouch));
    if (zox_valid(menu)) {
        zox_delete(menu);
    }
}

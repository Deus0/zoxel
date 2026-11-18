extern void button_event_jump(ecs *world, const ClickEventData event);
extern void button_event_attack(ecs *world, const ClickEventData event);

entity spawn_menu_game_touch(ecs *world, entity p, entity player, entity canvas) {
    // actions are picked by tapping their icon in the actionbar (actions/ui/fun touch_select_action)
    byte touch_buttons_count = 4;
    byte layer = max_layers2D - 10; // 1;
    int2 canvas_size = zox_getv(canvas, LayoutSize);
    // at most a seventh of the screen's short side: on a small screen (or a big ui_scale) the buttons covered most of
    //  it, every touch hit one, no joysticks (ints: as bytes 66 * ui_scale wrapped from ui_scale 4)
    int button_size = 66 * ui_scale;
    const int short_side = int_min(canvas_size.x, canvas_size.y);
    if (short_side > 0 && button_size > short_side / 7) {
        button_size = short_side / 7;
    }
    int bpadding = button_size * 8 / 66;
    int2 screen_margins = int2_single((int) (button_size * 0.4f));
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
        "touch_speak",
        "touch_jump",
        "touch_attack",
    };
    ClickEvent onclicks[] = {
        (ClickEvent) { &button_event_pause_game },
        (ClickEvent) { &button_event_speak },
        (ClickEvent) { &button_event_jump },
        (ClickEvent) { &button_event_attack },
    };
    float2 banchors[] = {
        float2_bottom_left,
        float2_bottom_left,
        float2_bottom_right,
        float2_bottom_right,
    };
    int2 bpositions[] = {
        (int2) { bposition.x, bposition.y },
        (int2) { bposition.x, bposition.y + (button_size + bpadding)  },
        (int2) { -bposition.x, bposition.y },
        (int2) { -bposition.x, bposition.y + (button_size + bpadding)  },
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
        // touch controls (jump, attack..) act as soon as they're touched
        zox_add(b, ClickOnPress);
        zox_set_parent_non_fragment(world, b, e);
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
    entity canvas = zox_get_link(world, player, CanvasLink);
    if (!zox_valid(canvas)) {
        return;
    }
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuPlayTouch));
    if (zox_valid(menu)) {
        zox_delete(menu);
    }
}

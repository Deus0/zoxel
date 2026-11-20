extern void button_event_jump(ecs *world, const ClickEventData event);
extern void button_event_attack(ecs *world, const ClickEventData event);
extern entity zox_get_id_statbars(ecs*);

// the touch buttons' size: at most a seventh of the screen's short side, on a small screen (or a big ui_scale) the
//  buttons covered most of it, every touch hit one, no joysticks (ints: as bytes 66 * ui_scale wrapped from ui_scale 4)
static inline int touch_button_size(ecs* world, entity canvas) {
    const int2 canvas_size = zox_getv(canvas, LayoutSize);
    const int short_side = int_min(canvas_size.x, canvas_size.y);
    const int size = 66 * ui_scale;
    return short_side > 0 && size > short_side / 7 ? short_side / 7 : size;
}

// the top left row (pause, game menus): half sized, they aren't play; the stat bars go under it
static inline int touch_top_row_height(ecs* world, entity canvas) {
    const int size = touch_button_size(world, canvas) / 2;
    return size + size * 8 / 33;
}

// the stat bars' window top at top (pixels down from the canvas top), it keeps its size
void touch_place_statbars(ecs* world, entity canvas, int top) {
    const entity statbars = zox_get_child_by_id(world, canvas, zox_get_id_statbars(world));
    if (!zox_valid(statbars)) {
        return;
    }
    int2 position = zox_getv(statbars, LayoutPosition);
    position.y = -top - zox_getv(statbars, LayoutSize).y / 2;
    zox_setv(statbars, LayoutPosition, position);
    zox_add(statbars, LayoutPositionDirty);
}

entity spawn_menu_game_touch(ecs *world, entity p, entity player, entity canvas) {
    // actions are picked by tapping their icon in the actionbar (actions/ui/fun touch_select_action)
    byte touch_buttons_count = 5;
    byte layer = max_layers2D - 10; // 1;
    int2 canvas_size = zox_getv(canvas, LayoutSize);
    const int button_size = touch_button_size(world, canvas);
    const int small_size = button_size / 2;
    int bpadding = button_size * 8 / 66;
    int2 screen_margins = int2_single((int) (button_size * 0.4f));
    entity e = spawn_layout2_on_canvas(world, p, canvas, int2_zero, canvas_size, float2_half);
    zox_name("menu_game_touch");
    int2 bposition = (int2) {
        screen_margins.x + button_size / 2,
        screen_margins.y + button_size / 2
    };
    // the top left row, its own padding round it
    const int spadding = small_size * 8 / 33;
    int2 sposition = (int2) { spadding + small_size / 2, -(spadding + small_size / 2) };
    byte blayer = layer + 1;
    color cfill = (color) { 15, 15, 15, 80 };
    color coutline = (color) { 40, 40, 40, 130 };
    char* tnames[] = {
        "taskbar_paused",
        "taskbar_items",
        "touch_speak",
        "touch_jump",
        "touch_attack",
    };
    ClickEvent onclicks[] = {
        (ClickEvent) { &button_event_pause_game },
        (ClickEvent) { &button_event_game_menus },     // the game menus, the game going on
        (ClickEvent) { &button_event_speak },
        (ClickEvent) { &button_event_jump },
        (ClickEvent) { &button_event_attack },
    };
    float2 banchors[] = {
        float2_top_left,
        float2_top_left,
        float2_bottom_left,
        float2_bottom_right,
        float2_bottom_right,
    };
    int2 bpositions[] = {
        sposition,
        (int2) { sposition.x + small_size + spadding, sposition.y },
        (int2) { bposition.x, bposition.y },
        (int2) { -bposition.x, bposition.y },
        (int2) { -bposition.x, bposition.y + (button_size + bpadding)  },
    };
    int bsizes[] = { small_size, small_size, button_size, button_size, button_size };
    for (byte i = 0; i < touch_buttons_count; i++) {
        entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(tnames[i]));
        entity b = spawn_button_icon(world, prefab_button,
            texture,
            canvas,
            e,
            bpositions[i],
            int2_single(bsizes[i]),
            banchors[i],
            blayer,
            cfill,
            coutline,
            0,
            onclicks[i],
            bsizes[i] * 8 / 66
        );
        // touch controls (jump, attack..) act as soon as they're touched
        zox_add(b, ClickOnPress);
        zox_set_parent_non_fragment(world, b, e);
    }
    touch_place_statbars(world, canvas, touch_top_row_height(world, canvas));
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
        touch_place_statbars(world, canvas, 0);
    }
}

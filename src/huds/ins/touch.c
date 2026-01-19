
extern void button_event_switch_action(ecs *world, const ClickEventData event);
extern void button_event_jump(ecs *world, const ClickEventData event);
extern void button_event_attack(ecs *world, const ClickEventData event);

entity spawn_menu_game_touch(ecs *world, entity p, entity player, entity canvas) {
    byte button_size = 35 * ui_scale;
    byte button_padding = 5 * ui_scale;
    byte2 screen_margins = (byte2) { button_size / 2, 15 * ui_scale };

    int2 canvas_size = zox_get_value(canvas, LayoutSize)
    entity e = spawn_layout2_on_canvas(world, p, canvas, int2_zero, canvas_size, float2_half);
    zox_name("menu_game_touch");

    Children children = (Children) { 0 };

#ifndef zox_disable_touch_buttons

    int2 spawn_position = (int2) {
        screen_margins.x + button_size / 2,
        screen_margins.y + button_size
    };

    // Left Side, Row 1
    add_to_Children(&children, spawn_button_game(world, canvas, e, canvas_size, spawn_position, float2_zero, button_size, (ClickEvent) { &button_event_pause_game }));

    // Left Side, Row 2
    spawn_position.y += button_size + button_padding;
    add_to_Children(&children, spawn_button_game(world, canvas, e, canvas_size, spawn_position, float2_zero, button_size, (ClickEvent) { &button_event_switch_action }));


    // right side - jump and attack
    float2 anchor_right = (float2) { 1, 0 };
    spawn_position.x = -(screen_margins.x + button_size / 2);
    spawn_position.y = screen_margins.y + button_size;

    // Right Side, Row 1
    add_to_Children(&children, spawn_button_game(world, canvas, e, canvas_size, spawn_position, anchor_right, button_size, (ClickEvent) { &button_event_jump }));

    // Right Side, Row 2
    spawn_position.y += button_size + button_padding;
    add_to_Children(&children, spawn_button_game(world, canvas, e, canvas_size, spawn_position, anchor_right, button_size, (ClickEvent) { &button_event_attack }));

#endif

    // link to character
    zox_geter(player, CharacterLink, characterLink);
    zox_muter(characterLink->value, ElementLinks, elementLinks);
    add_to_ElementLinks(elementLinks, e);
    zox_set(e, ElementHolder, { characterLink->value });
    zox_set_ptr(e, Children, children);

    return e;
}

// called from game state changes
void spawn_in_game_ui_touch(ecs *world, entity player, entity canvas) {
    if (!zox_valid(canvas)) {
        return;
    }

    find_child_with_tag(canvas, MenuPlayTouch, game_menu_touch);

    if (!zox_valid(game_menu_touch)) {
        spawn_menu_game_touch(world, prefab_menu_play_touch, player, canvas);
    }
}

void dispose_menu_game_touch(ecs *world, entity player) {
    if (!zox_valid(player)) {
        return;
    }

    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        return;
    }

    find_child_with_tag(canvas, MenuPlayTouch, game_menu_touch);
    if (zox_valid(game_menu_touch)) {
        zox_delete(game_menu_touch)
    }
}
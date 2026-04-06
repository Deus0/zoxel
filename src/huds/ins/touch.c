extern void button_event_switch_action(ecs *world, const ClickEventData event);
extern void button_event_jump(ecs *world, const ClickEventData event);
extern void button_event_attack(ecs *world, const ClickEventData event);

entity spawn_menu_game_touch(ecs *world, entity p, entity player, entity canvas) {

    byte layer = 1;
    byte button_size = 36 * ui_scale;
    byte bpadding = 6 * ui_scale;
    byte2 screen_margins = (byte2) { button_size / 4, button_size / 4 };

    int2 canvas_size = zox_get_value(canvas, LayoutSize);
    int2 bsize = int2_single(button_size);

    entity e = spawn_layout2_on_canvas(world, p, canvas, int2_zero, canvas_size, float2_half);
    zox_name("menu_game_touch");

    Children children = (Children) { 0 };

#ifndef zox_disable_touch_buttons

    int2 bposition = (int2) {
        screen_margins.x + button_size / 2,
        screen_margins.y + button_size / 2
    };

    byte blayer = layer + 1;

    color cfill = (color) { 15, 15, 15, 80 };
    color coutline = (color) { 40, 40, 40, 130 };

    char* tnames[] = {
        "paused",
        "touch_switch",
        "touch_attack",
        "touch_jump"
    };
    ClickEvent onclicks[] = {
        (ClickEvent) { &button_event_pause_game },
        (ClickEvent) { &button_event_switch_action },
        (ClickEvent) { &button_event_attack },
        (ClickEvent) { &button_event_jump },
    };
    float2 banchors[] = {
        (float2) { 1, 1 },
        float2_zero,
        (float2) { 0, 0 }, (float2) { 1, 0 },
    };
    int2 bpositions[] = {
        (int2) { -bposition.x, -bposition.y },
        (int2) { bposition.x, bposition.y + button_size + bpadding  },
        (int2) { bposition.x, bposition.y },
        (int2) { -bposition.x, bposition.y },
    };

    for (byte i = 0; i < 4; i++) {
        entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(tnames[i]));
        entity b = spawn_button_icon(
            world,
            prefab_button,
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
        add_to_Children(&children, b);
    }

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

    // find_child_with_tag(canvas, MenuPlayTouch, game_menu_touch);

    entity menu = find_child_with_tag_recursive(world, canvas, zox_id(MenuPlayTouch));
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

    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        return;
    }

    // find_child_with_tag(canvas, MenuPlayTouch, game_menu_touch);
    entity menu = find_child_with_tag_recursive(world, canvas, zox_id(MenuPlayTouch));
    if (zox_valid(menu)) {
        zox_delete(menu);
    }
}

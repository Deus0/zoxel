// tst: item_split, right clicking item slots in the inventory with the fake mouse
//  - gives the character a stack of 10, pauses, closes the pause menu and opens the inventory
//  - right clicking the stack picks up half: 5 on the mouse, 5 left in the slot
//  - right clicking an empty slot places one (mouse 4), again stacks one more there (mouse 3)
//  - right clicking the first stack again adds one to it (6, mouse 2)
//  - run: bash bsh/tst.sh item_split
#define zox_tst_split_hold_frames 4
#define zox_tst_split_quantity 10
int zox_tst_split_stage = 0;
int zox_tst_split_frames = 0;
entity zox_tst_split_item = 0;
entity zox_tst_split_slot = 0;
entity zox_tst_split_empty_slot = 0;
int2 zox_tst_split_on_stack;
int2 zox_tst_split_on_empty;

// the DataUI icon showing slot, 0 if it's not open
entity zox_tst_split_find_icon(ecs* world, entity e, entity slot) {
    if (zox_has(e, DataUI) && zox_has(e, Icon) && zox_get_link(world, e, SlotLink) == slot) {
        return e;
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity found = zox_tst_split_find_icon(world, it2.entities[j], slot);
            if (found) {
                zox_children_end(it2);
                return found;
            }
        }
    }
    return 0;
}

static inline entity zox_tst_split_mouse_data(ecs* world, entity canvas) {
    const entity mouse_ui = zox_get_child_by_id(world, canvas, zox_id(DataMouse));
    return zox_valid(mouse_ui) ? zox_get_link(world, mouse_ui, DataLink) : 0;
}

// one right click: press for a few frames, then release for a few
static inline byte zox_tst_split_right_click(int2 position) {
    zox_tst_mouse_right(position, zox_tst_split_frames <= zox_tst_split_hold_frames);
    return zox_tst_split_frames >= 2 * zox_tst_split_hold_frames;
}

byte zox_tst_item_split_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_split_frames++;
    switch (zox_tst_split_stage) {
        case 0: {
            zox_tst_waiting = "playing, with an inventory";
            const entity inventory = zox_valid(character) ? zox_get_child_by_id(world, character, zox_id(Inventory)) : 0;
            if (zox_getv(player, PlayerState) != zox_player_state_playing || !zox_valid(inventory) || zox_tst_split_frames < 10) {
                return 0;
            }
            // a stackable item from the realm
            const entity realm = zox_get_link(world, character, RealmLink);
            entity stackable = 0;
            iter it2 = zox_children(world, realm);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count && !stackable; j++) {
                    if (zox_has(it2.entities[j], Item) && zox_has(it2.entities[j], Quantity)) {
                        stackable = it2.entities[j];
                    }
                }
            }
            entity slots[2];
            const uint empty = zox_get_empty_slots(world, inventory, slots, 2);
            zox_tst_check(stackable, "realm has no stackable item");
            zox_tst_check(empty == 2, "inventory has [%u] empty slots, expected 2", empty);
            if (!stackable || empty < 2) {
                return 1;
            }
            zox_tst_split_item = spawn_user_item(world, character, stackable);
            zox_setv(zox_tst_split_item, Quantity, zox_tst_split_quantity);
            zox_tst_split_slot = slots[0];
            zox_tst_split_empty_slot = slots[1];
            zox_set_link(world, zox_tst_split_slot, DataLink, zox_tst_split_item);
            zox_add(zox_tst_split_slot, DataDirty);
            zox_log("[tst] gave a stack of [%i] [%s], pausing", zox_tst_split_quantity, zox_getn(stackable));
            button_event_pause_game(world, (ClickEventData) { .clicker = player });
            break;
        }
        case 1:
            zox_tst_waiting = "paused";
            if (zox_getv(player, PlayerState) != zox_player_state_paused || zox_tst_split_frames < 10) {
                return 0;
            }
            {
                // the pause menu (spawned after a delay) sits over the middle of the screen,
                //  closes it to reach the inventory
                const entity paused = zox_get_child_by_id(world, canvas, zox_id(MenuPaused));
                if (!zox_valid(paused)) {
                    return 0;
                }
                zox_delete(paused);
            }
            spawn_menu_inventory(world, player);
            break;
        case 2: {
            zox_tst_waiting = "the inventory's layout";
            if (zox_tst_split_frames < 15) {
                return 0;
            }
            const entity stack_icon = zox_tst_split_find_icon(world, canvas, zox_tst_split_slot);
            const entity empty_icon = zox_tst_split_find_icon(world, canvas, zox_tst_split_empty_slot);
            zox_tst_check(stack_icon && empty_icon, "inventory has no icons for the test's slots");
            if (!stack_icon || !empty_icon ||
                !zox_tst_ui_pointer_position(world, canvas, stack_icon, &zox_tst_split_on_stack) ||
                !zox_tst_ui_pointer_position(world, canvas, empty_icon, &zox_tst_split_on_empty))
            {
                return 1;
            }
            zox_tst_check(zox_get_link(world, stack_icon, DataLink) == zox_tst_split_item, "the stack's icon doesn't show it");
            // a left click on nothing, so the player uses the mouse (raycasting the ui)
            zox_tst_mouse((int2) { 4, 4 }, zox_tst_split_frames < 18);
            if (zox_tst_split_frames < 22) {
                return 0;
            }
            break;
        }
        case 3: {
            zox_tst_waiting = "right clicking the stack";
            if (!zox_tst_split_right_click(zox_tst_split_on_stack)) {
                return 0;
            }
            const entity mouse_data = zox_tst_split_mouse_data(world, canvas);
            if (!zox_valid(mouse_data)) {
                const entity target = zox_tst_mouse_target(world, player);
                zox_log("[tst] right clicked at [%ix%i] over [%s]", zox_tst_split_on_stack.x, zox_tst_split_on_stack.y,
                    zox_valid(target) ? zox_getn(target) : "nothing");
            }
            zox_tst_check(zox_valid(mouse_data), "right clicking the stack didn't pick up half");
            zox_tst_check(get_item_quantity(world, mouse_data) == zox_tst_split_quantity / 2,
                "mouse holds [%i], expected half [%i]", get_item_quantity(world, mouse_data), zox_tst_split_quantity / 2);
            zox_tst_check(get_item_quantity(world, zox_tst_split_item) == zox_tst_split_quantity - zox_tst_split_quantity / 2,
                "slot kept [%i], expected [%i]", get_item_quantity(world, zox_tst_split_item), zox_tst_split_quantity - zox_tst_split_quantity / 2);
            if (!zox_valid(mouse_data)) {
                return 1;
            }
            break;
        }
        case 4:
        case 5: {
            zox_tst_waiting = "right clicking the empty slot";
            if (!zox_tst_split_right_click(zox_tst_split_on_empty)) {
                return 0;
            }
            const int placed = zox_tst_split_stage - 3;
            const entity placed_item = zox_get_link(world, zox_tst_split_empty_slot, DataLink);
            const entity mouse_data = zox_tst_split_mouse_data(world, canvas);
            zox_tst_check(get_item_quantity(world, placed_item) == placed, "right click [%i] on the empty slot: it holds [%i], expected [%i]",
                placed, get_item_quantity(world, placed_item), placed);
            zox_tst_check(get_item_quantity(world, mouse_data) == zox_tst_split_quantity / 2 - placed, "right click [%i] on the empty slot: mouse holds [%i], expected [%i]",
                placed, get_item_quantity(world, mouse_data), zox_tst_split_quantity / 2 - placed);
            break;
        }
        default: {
            zox_tst_waiting = "right clicking the first stack again";
            if (!zox_tst_split_right_click(zox_tst_split_on_stack)) {
                return 0;
            }
            const entity mouse_data = zox_tst_split_mouse_data(world, canvas);
            const int expected_stack = zox_tst_split_quantity - zox_tst_split_quantity / 2 + 1;
            zox_tst_check(get_item_quantity(world, zox_tst_split_item) == expected_stack, "first stack holds [%i], expected [%i]",
                get_item_quantity(world, zox_tst_split_item), expected_stack);
            zox_tst_check(get_item_quantity(world, mouse_data) == zox_tst_split_quantity / 2 - 3, "mouse holds [%i], expected [%i]",
                get_item_quantity(world, mouse_data), zox_tst_split_quantity / 2 - 3);
            zox_tst_mouse_end();
            return 1;
        }
    }
    zox_tst_split_stage++;
    zox_tst_split_frames = 0;
    return 0;
}

void zox_tst_item_split_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}

// tst: mouse_click, clicks the main menu with the fake mouse, through the real input path
//  - mouse device > element_raycast_system > zevice_click_system > button_click_event_system
//  - clicking on the start menu opens the main menu (mouse buttons count as any input, movement doesn't)
//  - clicking empty space does nothing
//  - pressing on empty space, then dragging onto new game and releasing there, doesn't click it
//  - hovering new game targets it, pressing it doesn't click yet (buttons click on release)
//  - pressing it then dragging off and releasing elsewhere cancels the click
//  - pressing and releasing on it clicks it once: one new realm menu, main menu closed
//  - a ClickOnPress element (touch controls, item slots) clicks on press: the new realm menu's
//      enter button is tagged, pressing it enters the realm before the mouse is released
//  - run: bash bsh/tst.sh mouse_click
#define zox_tst_mouse_hold_frames 4
#define zox_tst_mouse_layout_frames 10
int zox_tst_mouse_stage = 0;
int zox_tst_mouse_frames = 0;
entity zox_tst_mouse_button = 0;
entity zox_tst_mouse_enter = 0;
int2 zox_tst_mouse_on_enter;
int2 zox_tst_mouse_on_button;
int2 zox_tst_mouse_on_empty;

static inline void zox_tst_mouse_next() {
    zox_tst_mouse_stage++;
    zox_tst_mouse_frames = 0;
}

// one step of the mouse script: holds the mouse at position for a few frames
//  - returns 1 on the last frame, when its result can be checked
static inline byte zox_tst_mouse_hold(
    int2 position,
    byte left)
{
    zox_tst_mouse(position, left);
    return zox_tst_mouse_frames >= zox_tst_mouse_hold_frames;
}

// counts canvas children with a component, i.e. MenuNewRealm (a double click would spawn two)
int zox_tst_mouse_count_menus(ecs* world, entity canvas, entity id) {
    int count = 0;
    iter it2 = zox_children(world, canvas);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            if (zox_has_id(it2.entities[j], id)) {
                count++;
            }
        }
    }
    return count;
}

byte zox_tst_mouse_click_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_mouse_frames++;
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const int new_realms = zox_tst_mouse_count_menus(world, canvas, zox_id(MenuNewRealm));
    const entity target = zox_tst_mouse_target(world, player);
    switch (zox_tst_mouse_stage) {
        case 0:
            // clicks until the start menu opens the main menu
            zox_tst_waiting = "a mouse click to open the main menu";
            zox_tst_mouse((int2) { 100, 100 }, (zox_tst_mouse_frames / zox_tst_mouse_hold_frames) % 2);
            if (!main_menu) {
                return 0;
            }
            zox_tst_check(!zox_valid(zox_get_link(world, player, MenuStart)), "start menu is still open with the main menu");
            break;
        case 1: {
            // waits for the main menu's layout, then finds where to click
            zox_tst_waiting = "the main menu's layout";
            zox_tst_mouse((int2) { 100, 100 }, 0);
            if (zox_tst_mouse_frames < zox_tst_mouse_layout_frames) {
                return 0;
            }
            zox_tst_mouse_button = zox_tst_find_click(world, main_menu, &button_event_new_game);
            zox_tst_check(zox_tst_mouse_button, "main menu has no new game button");
            if (!zox_tst_mouse_button) {
                return 1;
            }
            const byte found = zox_tst_ui_pointer_position(world, canvas, zox_tst_mouse_button, &zox_tst_mouse_on_button);
            zox_tst_check(found, "new game button has no screen position");
            if (!found) {
                return 1;
            }
            // top right corner of the canvas, away from the menu and the device gizmo
            const entity camera = zox_get_link(world, canvas, CameraLink);
            const int2 canvas_position = zox_getv(camera, ScreenPosition);
            const int2 canvas_size = zox_getv(camera, PixelSize);
            zox_tst_mouse_on_empty = (int2) {
                canvas_position.x + canvas_size.x - 4,
                canvas_position.y + canvas_size.y - 4
            };
            zox_log("[tst] new game button at [%ix%i], empty space at [%ix%i]",
                zox_tst_mouse_on_button.x, zox_tst_mouse_on_button.y,
                zox_tst_mouse_on_empty.x, zox_tst_mouse_on_empty.y);
            break;
        }
        // click empty space
        case 2:
            zox_tst_waiting = "moving to empty space";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_empty, 0)) {
                return 0;
            }
            zox_tst_check(!zox_tst_is_within(world, target, main_menu), "empty space targets [%s] in the main menu", zox_getn(target));
            break;
        case 3:
            zox_tst_waiting = "pressing on empty space";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_empty, 1)) {
                return 0;
            }
            break;
        case 4:
            zox_tst_waiting = "releasing on empty space";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_empty, 0)) {
                return 0;
            }
            zox_tst_check(main_menu, "clicking empty space closed the main menu");
            zox_tst_check(new_realms == 0, "clicking empty space opened [%i] new realm menus", new_realms);
            break;
        // press on empty space, drag onto the button, release on it
        case 5:
            zox_tst_waiting = "pressing on empty space, again";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_empty, 1)) {
                return 0;
            }
            break;
        case 6:
            zox_tst_waiting = "dragging onto new game";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_button, 1)) {
                return 0;
            }
            zox_tst_check(main_menu && new_realms == 0, "dragging a held mouse onto new game clicked it");
            break;
        case 7:
            zox_tst_waiting = "releasing on new game, after dragging onto it";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_button, 0)) {
                return 0;
            }
            zox_tst_check(main_menu && new_realms == 0, "releasing on new game, after dragging onto it, clicked it");
            break;
        // press on the button, drag off it, release elsewhere: cancelled
        case 8:
            zox_tst_waiting = "hovering new game";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_button, 0)) {
                return 0;
            }
            zox_tst_check(zox_tst_is_within(world, target, zox_tst_mouse_button),
                "hovering new game targets [%s]", zox_valid(target) ? zox_getn(target) : "nothing");
            break;
        case 9:
            zox_tst_waiting = "pressing new game";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_button, 1)) {
                return 0;
            }
            zox_tst_check(main_menu && new_realms == 0, "pressing new game clicked it before release");
            break;
        case 10:
            zox_tst_waiting = "dragging off new game";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_empty, 1)) {
                return 0;
            }
            break;
        case 11:
            zox_tst_waiting = "releasing off new game";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_empty, 0)) {
                return 0;
            }
            zox_tst_check(main_menu && new_realms == 0, "releasing off new game still clicked it");
            break;
        // click the button
        case 12:
            zox_tst_waiting = "hovering new game again";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_button, 0)) {
                return 0;
            }
            break;
        case 13:
            zox_tst_waiting = "pressing new game again";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_button, 1)) {
                return 0;
            }
            zox_tst_check(main_menu && new_realms == 0, "pressing new game again clicked it before release");
            break;
        case 14:
            zox_tst_waiting = "releasing on new game";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_button, 0)) {
                return 0;
            }
            zox_tst_check(new_realms == 1, "releasing on new game opened [%i] new realm menus, expected 1", new_realms);
            break;
        case 15: {
            zox_tst_waiting = "the new realm menu's layout";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_button, 0) || zox_tst_mouse_frames < zox_tst_mouse_layout_frames) {
                return 0;
            }
            zox_tst_check(!main_menu, "main menu wasn't closed after clicking new game");
            zox_tst_check(new_realms == 1, "after clicking new game once, [%i] new realm menus are open, expected 1", new_realms);
            const entity new_realm = zox_tst_menu(world, canvas, zox_id(MenuNewRealm));
            zox_tst_mouse_enter = new_realm ? zox_tst_find_click(world, new_realm, &on_confirmed_new_realm) : 0;
            zox_tst_check(zox_tst_mouse_enter, "new realm menu has no enter button");
            if (!zox_tst_mouse_enter ||
                !zox_tst_ui_pointer_position(world, canvas, zox_tst_mouse_enter, &zox_tst_mouse_on_enter))
            {
                zox_tst_check(0, "enter button has no screen position");
                return 1;
            }
            zox_add(zox_tst_mouse_enter, ClickOnPress);
            break;
        }
        case 16:
            zox_tst_waiting = "hovering enter (click on press)";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_enter, 0)) {
                return 0;
            }
            zox_tst_check(new_realms == 1, "hovering enter clicked it");
            break;
        default:
            zox_tst_waiting = "pressing enter (click on press)";
            if (!zox_tst_mouse_hold(zox_tst_mouse_on_enter, 1)) {
                return 0;
            }
            zox_tst_check(new_realms == 0, "pressing a ClickOnPress enter button didn't click it before release");
            zox_tst_mouse_end();
            return 1;
    }
    zox_tst_mouse_next();
    return 0;
}

void zox_tst_mouse_click_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}
